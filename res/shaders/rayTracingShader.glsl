#version 330 core
out vec4 FragColor;

uniform vec4 eye;
uniform vec4 ambient;
uniform vec4[20] objects;
uniform vec4[20] objColors;
uniform vec4[10] lightsDirection;
uniform vec4[10] lightsIntensity;
uniform vec4[10] lightPosition;
uniform ivec4 sizes; //{number of objects , number of lights , rCounter, hight}  

in vec3 position;

struct Intersection{
    vec3 intersectionPoint;
    float dist;
    int idxInter;
};

vec4 intersection(vec3 srcPoint, vec3 direction, vec4 obj)
{
    vec4 noInter = vec4(0, 0, 0, 0); // { x , y , z , distance }
    float t;
    
    // Plane
    if (obj.w < 0) {
        // Ax + By + Cz + D
        vec3 normal = obj.xyz;
        float d = obj.w;
        float denominator = dot(direction, normal);
        if (denominator == 0) return noInter; // no intersection
        t = -(dot(srcPoint, normal) + d) / denominator;
    } else { //Sphere
        // x = s + td
        // t = -(v * d) ± sqrt((v*d)^2 -(v^2-r^2))
        vec3 v = srcPoint - obj.xyz; // obj.xyz = center of sphere
        float vd = dot(direction, v);
        float sqrCalc = dot(vd, vd) - (dot(v, v) - dot(obj.w, obj.w));
        if(sqrCalc < 0) return noInter; // no intersection
        sqrCalc = sqrt(sqrCalc);
        float t1 = -vd + sqrCalc, t2 = -vd - sqrCalc;
        if (t1 < 0 && t2 < 0) return noInter; // sphere is behind us
        t = (t1 < 0) ? t2 : t1;
    }
    vec3 ip = srcPoint + t * direction;
    return vec4(ip.xyz, t);
}

Intersection get_intersecting_object(vec3 srcPoint, vec3 direction, int currObj){
    vec4 inter = vec4(0, 0, 0, 0), curr_inter;
    int idxInter = -1;
    for(int i = 0; i < sizes.x; i++){ // iterate through objects
        if(i == currObj) continue; // skips the same object
        curr_inter = intersection(srcPoint, direction, objects[i]);
        if(curr_inter.w > 0 && (idxInter == -1 || curr_inter.w < inter.w)){
            idxInter = i;
            inter = curr_inter;
        }
    }
    return Intersection(inter.xyz, inter.w, idxInter);
}

#define IS_SPOT_LIGHT(j) (lightsDirection[j].w > 0.0)
#define IS_DARK_SQUARE mod(int(1.5*intersectionPoint.x), 2) == mod(int(1.5*intersectionPoint.y), 2)
#define KS vec3(0.7, 0.7, 0.7)

vec3 object_color(int objIndex, vec3 V, vec3 intersectionPoint, vec3 normal){
    float NL, VRN, light_dist, o_dist, dp;
    vec3 L, IL;
    vec3 color = (ambient * objColors[objIndex]).xyz; // ambient color of object
    for(int j = 0; j < sizes.y; j++) { // iterate through light sources
        L = IS_SPOT_LIGHT(j) ? normalize(lightPosition[j].xyz - intersectionPoint)
            : normalize(-lightsDirection[j].xyz); // direction to light source
        // Check for shadow
        light_dist = IS_SPOT_LIGHT(j) ? length(lightPosition[j].xyz - intersectionPoint)
            : -1; // infinite distance for directional light
        o_dist = get_intersecting_object(intersectionPoint, L, objIndex).dist;
        if(o_dist != 0.0 && (light_dist == -1 || o_dist < light_dist)) continue;
        // Diffuse calculation
        // GL_SPOT_CUTOFF should be added to spot calculation
        IL = lightsIntensity[j].xyz * dot(lightsDirection[j].xyz, L); // Intensity's A comp is redundant
        NL = dot(L, normal) / (length(L) * length(normal));
        dp = (objects[objIndex].w >= 0 || IS_DARK_SQUARE) ? 1.0 : 0.5;
        color += dp * objColors[objIndex].xyz * NL * IL; 
        // Specular calculation
        // @@ LOOKS GOOD ON POWERS OF 1,3,5
        // @@ CHECK REFLECT IMPLEMENTATION
        VRN = pow(dot(V, reflect(-L, normal)), objColors[objIndex].w); // power with shininess
        color += KS * VRN * IL;
    }
    return color;
}

vec3 calcNormal(int objIndex, vec3 intersectionPoint){
    return (objects[objIndex].w < 0) ? normalize(objects[objIndex].xyz) // normal of shape
        : normalize(intersectionPoint - objects[objIndex].xyz);
}

#define REF_MAX_LEVEL 1

vec3 colorCalc(vec3 eye)
{
    vec3 V = normalize(position - eye); // light ray direction vector
    vec3 color = ambient.xyz; // background color
    Intersection inter = get_intersecting_object(eye, V, -1);
    if(inter.idxInter != -1) {
        vec3 normal = calcNormal(inter.idxInter, inter.intersectionPoint);
        color += object_color(inter.idxInter, V, inter.intersectionPoint, normal);
        vec3 R; // mirrored ray direction
        for(int level = 0; level < REF_MAX_LEVEL; level++){
            if(inter.idxInter > sizes.z) break; // object is not reflective
            R = normalize(reflect(V, normal));
            inter = get_intersecting_object(inter.intersectionPoint, R, inter.idxInter);
            if(inter.dist == 0.0) break;
            normal = calcNormal(inter.idxInter, inter.intersectionPoint);
            // @@ should call colorCalc, but can't have recursion.
            color += KS * object_color(inter.idxInter, R, inter.intersectionPoint, normal);
        }
    }
    return clamp(color, vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0));
}

void main()
{ 
    FragColor = vec4(colorCalc(eye.xyz),1);      
}
 