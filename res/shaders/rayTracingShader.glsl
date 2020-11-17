#version 330 core
out vec4 FragColor;

uniform vec4 eye;
uniform vec4 ambient;
uniform vec4[20] objects;
uniform vec4[20] objColors;
uniform vec4[10] lightsDirection;
uniform vec4[10] lightsIntensity;
uniform vec4[10] lightPosition;
uniform ivec4 sizes; //{number of objects , number of lights , width, hight}  

in vec3 position1;

vec4 intersection(vec3 srcPoint, vec3 direction, vec4 obj)
{
    vec4 res = vec4(0, 0, 0, 0); // { x , y , z , distance }
    float t;
    
    // Plane
    if (obj.w < 0) {
        // Ax + By + Cz + D
        vec3 normal = obj.xyz;
        float d = -obj.w;
        float denominator = dot(direction, normal);
        if (denominator == 0) return res; // no intersection
        t = -(dot(srcPoint, normal) + d) / denominator;
    } else { //Sphere
        // x = s + td
        // t = -(v * d) ± sqrt((v*d)^2 -(v^2-r^2))
        vec3 v = srcPoint - obj.xyz; // obj.xyz = center of sphere
        float vd = dot(direction, v);
        float sqrCalc = dot(vd, vd) - (dot(v, v) - dot(obj.w, obj.w));
        if(sqrCalc < 0) return res; // no intersection
        sqrCalc = sqrt(sqrCalc);
        float t1 = -vd + sqrCalc, t2 = -vd - sqrCalc;
        if (t1 < 0 && t2 < 0) return res; // sphere is behind us
        t = (t1 < 0) ? t2 : t1;
    }
    vec3 ip = srcPoint + t * direction;
    return vec4(ip.xyz, t);
}

// Creates a normalized vector from 2 points
vec3 buildVector(vec3 pointStart, vec3 pointEnd) {
    return normalize((pointEnd - pointStart));
}

vec3 colorCalc(vec3 intersectionPoint)
{
    vec3 V = buildVector(position1, intersectionPoint); // construct light ray from pixel
    vec3 color = ambient.xyz; // background color
    vec4 inter = vec4(0, 0, 0, 0), curr_inter;
    int idxInter = -1;
    for(int i = 0; i < sizes.x; i++){ // iterate through objects
        curr_inter = intersection(intersectionPoint, V, objects[i]);
        if(curr_inter.w > 0 && (idxInter == -1 || curr_inter.w < inter.w)){
            idxInter = i;
            inter = curr_inter;
        }
    }
    if(idxInter != -1) {
        float NL, VRN;
        vec3 L, normal;
        color = (ambient * objColors[idxInter]).xyz; // ambient color of object
        for(int j = 0; j < sizes.y; j++) { // iterate through light sources
            L = (lightPosition[j].w > 0) ? // ray to light source
                buildVector(inter.xyz, lightPosition[j].xyz) : lightPosition[j].xyz;
            // Diffuse calculation
            normal = (objects[j].w < 0) ? // normal of shape
                objects[j].xyz : buildVector(inter.xyz, objects[j].xyz);
            NL = dot(L, normal) / (length(L) * length(normal));
            color += (objColors[idxInter] * NL * lightsIntensity[j]).xyz; 
            // Specular calculation
            VRN = pow(dot(V, reflect(-L, normal)), objColors[idxInter].w);
            color += vec3(0.7, 0.7, 0.7) * VRN * lightsIntensity[j].xyz;
        }
    }
    return clamp(color, vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0));
}

void main()
{  
    FragColor = vec4(0.5, 0.5, 0.5, 0.5);
    //FragColor = vec4(colorCalc(eye.xyz),1);      
}
 