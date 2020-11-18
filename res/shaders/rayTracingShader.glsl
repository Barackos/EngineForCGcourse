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

in vec3 position;

vec4 intersection(vec3 srcPoint, vec3 direction, vec4 obj)
{
    vec4 noInter = vec4(0, 0, 0, 0); // { x , y , z , distance }
    float t;
    
    // Plane
    if (obj.w < 0) {
        // Ax + By + Cz + D
        vec3 normal = obj.xyz;
        float d = -obj.w;
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

vec3 colorCalc(vec3 intersectionPoint)
{
    vec3 V = normalize(position - intersectionPoint); // light ray direction vector
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
        vec3 L, normal, IL;
        color = (ambient * objColors[idxInter]).xyz; // ambient color of object
        for(int j = 0; j < sizes.y; j++) { // iterate through light sources
            L = normalize(lightPosition[j].xyz - inter.xyz); // direction to light source
            // Diffuse calculation
            normal = (objects[idxInter].w < 0) ? // normal of shape
                normalize(objects[idxInter].xyz) : normalize(inter.xyz - objects[idxInter].xyz);
            //(lightPosition[j].w > 0) ? SHOULD BE APPLIED TO LIGHTS DIRECTION OR IL
            // GL_SPOT_CUTOFF should be added to sport calculation
            IL = lightsIntensity[j].xyz * dot(lightsDirection[j].xyz, L); // Intensity's A comp is redundant
            NL = dot(L, normal) / (length(L) * length(normal));
            color += objColors[idxInter].xyz * NL * IL; 
            // Specular calculation
            VRN = pow(dot(V, reflect(-L, normal)), objColors[idxInter].w); // power with shininess
            color += vec3(0.7, 0.7, 0.7) * VRN * IL;
        }
    }
    return clamp(color, vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0));
}

void main()
{ 
    FragColor = vec4(colorCalc(eye.xyz),1);      
}
 