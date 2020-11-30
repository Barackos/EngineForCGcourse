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
uniform vec4 screen;

in vec3 position;

struct Intersection{
    vec3 intersectionPoint;
    float dist;
    int idxInter;
};

vec4 intersection(vec3 srcPoint, vec3 direction, vec4 obj, bool c)
{
    vec4 noInter = vec4(0, 0, 0, 0); // { x , y , z , distance }
    float t;
    
    // Plane
    if (obj.w < 0) {
        // Ax + By + Cz + D
        vec3 normal = obj.xyz;
        float denominator = dot(direction, normal);
        if (denominator == 0) return noInter; // no intersection
        t = -(dot(srcPoint, normal) + obj.w) / denominator;
    } else { //Sphere
        // x = s + td
        // t = -(v * d) ± sqrt((v*d)^2 -(v^2-r^2))
        vec3 v = srcPoint - obj.xyz; // obj.xyz = center of sphere
        float vd = dot(direction, v);
        float sqrCalc = vd * vd - (dot(v, v) - obj.w * obj.w);
        if(sqrCalc < 0) return noInter; // no intersection
        sqrCalc = sqrt(sqrCalc);
        float t1 = -vd + sqrCalc, t2 = -vd - sqrCalc;
        if (t1 <= 0 && t2 <= 0) return noInter; // sphere is behind us
        if(c && abs(t1) < obj.w) return noInter;
        t = (t2 <= 0) ? t1 : t2; // t2 is always smaller when both positive
    }
    vec3 ip = srcPoint + t * direction;
    return vec4(ip.xyz, t);
}

Intersection get_intersecting_object(vec3 srcPoint, vec3 direction, int currObj){
    vec4 inter = vec4(0, 0, 0, 0), curr_inter;
    int idxInter = -1;
    float thr = 0.0;
    for(int i = 0; i < sizes.x; i++){ // iterate through objects
        if(i == currObj && objects[currObj].w < 0) continue; // skips the same object
        curr_inter = intersection(srcPoint, direction, objects[i], i == currObj);
        if(curr_inter.w > thr && (idxInter == -1 || curr_inter.w < inter.w)){
            idxInter = i;
            inter = curr_inter;
        }
    }
    return Intersection(inter.xyz, inter.w, idxInter);
}

#define IS_REFLECTIVE_OBJECT(j) (j < sizes.z)
#define IS_SPOT_LIGHT(j) (lightsDirection[j].w > 0.0)
#define KS vec3(0.7, 0.7, 0.7)

bool isBrightSquare(vec3 intersectionPoint) {
    bool evenPlanes = intersectionPoint.x * intersectionPoint.y >= 0;
    float equalModulus = mod(int(1.5*intersectionPoint.x), 2) - mod(int(1.5*intersectionPoint.y), 2);
    return evenPlanes ? equalModulus == 0 : equalModulus != 0;
}

vec3 object_color(int objIndex, vec3 V, vec3 intersectionPoint, vec3 normal){
    float NL, VRN, light_dist, dp;
    Intersection o;
    vec3 L, IL, ips, ips2;
    vec3 color = (ambient * objColors[objIndex]).xyz; // ambient color of object
    for(int j = 0; j < sizes.y; j++) { // iterate through light sources
        L = IS_SPOT_LIGHT(j) ? normalize(intersectionPoint - lightPosition[j].xyz)
            : normalize(lightsDirection[j].xyz); // direction from light source
        // Check for shadow
        light_dist = IS_SPOT_LIGHT(j) ? length(lightPosition[j].xyz - intersectionPoint)
            : -1; // infinite distance for directional light
        o = get_intersecting_object(intersectionPoint, -L, objIndex);
        if(objects[o.idxInter].w > 0 && o.dist > 0.0 // planes do not cause shadow
            && (light_dist == -1 || o.dist < light_dist)) continue;
        // Light intensity calculation
        if(IS_SPOT_LIGHT(j) && // no lighting from this spot if we're outside it's range
            (dot(L, lightsDirection[j].xyz) / (length(L) * length(lightsDirection[j].xyz)) <= lightPosition[j].w)) continue;
        IL = lightsIntensity[j].xyz * dot(lightsDirection[j].xyz, IS_SPOT_LIGHT(j) ? L : normal); // Intensity's A comp is redundant
        // Diffuse calculation
        NL = dot(L, normal) / (length(L) * length(normal));
        dp = (objects[objIndex].w >= 0 || isBrightSquare(intersectionPoint)) ? 1.0 : 0.5;
        color += dp * objColors[objIndex].xyz * NL * IL; 
        // Specular calculation
        VRN = pow(dot(V, reflect(L, normal)), objColors[objIndex].w); // power with shininess
        color += KS * VRN * IL;
    }
    return IS_REFLECTIVE_OBJECT(objIndex) ? 0.5 * color : color;
}

vec3 calcNormal(int objIndex, vec3 intersectionPoint){
    return (objects[objIndex].w < 0) ? normalize(objects[objIndex].xyz) // normal of shape
        : -normalize(intersectionPoint - objects[objIndex].xyz);
}

#define REF_MAX_LEVEL 5

vec3 colorCalc(vec3 eye)
{
    vec3 V = normalize(position + screen.xyz - eye), normal; // light ray direction vector
    vec3 color = vec3(0.0, 0.0, 0.0); // background color
    Intersection inter = Intersection(eye, 0, -1);

    for(int level = 0; level < REF_MAX_LEVEL; level++){
        inter = get_intersecting_object(inter.intersectionPoint, V, inter.idxInter);
        if(inter.idxInter == -1) break; // no intersecting object
        normal = calcNormal(inter.idxInter, inter.intersectionPoint);
        color += object_color(inter.idxInter, V, inter.intersectionPoint, normal);
        if(!IS_REFLECTIVE_OBJECT(inter.idxInter)) break; // stop tracing when object is not reflective
        V = normalize(reflect(V, normal)); // mirrored ray direction for Reflection calculation
    }
    return clamp(color, vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0));
}

void main()
{ 
    FragColor = vec4(colorCalc(eye.xyz),1);      
}
 