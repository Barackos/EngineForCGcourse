
#include <iostream>
#include <res/includes/GL/glew.h>
#include "raytracing.h"
#include <math.h>

Raytracing::Raytracing(const std::string& fileName)
{
    this->data = new SceneData();
    SceneParser parser(fileName, this->data);
}

void Raytracing::Init()
{
    unsigned int texIDs[3] = {0, 1, 2}; //texture ids for the material (currently all 3)
    unsigned int slots[3] = {0, 0, 0};  // ?
    AddShader("../res/shaders/rayTracingShader");  
    //AddTexture("../res/textures/grass.bmp", 2);
    //AddMaterial(texIDs, slots, 1);
    AddShape(Plane, -1, TRIANGLES);
    SetShapeShader(0, 0);
}

void Raytracing::Update(const glm::mat4 &MVP, const glm::mat4 &Model, const int shaderIndx)
{
    Shader *s = shaders[shaderIndx];
    if (shapes[pickedShape]->GetMaterial() >= 0 && !materials.empty())
    {
        BindMaterial(s, shapes[pickedShape]->GetMaterial());
    }
    s->Bind();
    s->SetUniformMat4f("MVP", MVP);
    s->SetUniformMat4f("Normal", Model);
    s->SetUniform4f("eye", this->data->eye.x, this->data->eye.y, this->data->eye.z, this->data->eye.w);
    s->SetUniform4f("ambient", this->data->ambient.x, this->data->ambient.y, this->data->ambient.z, this->data->ambient.w);
    s->SetUniform4i("sizes", this->data->sizes.x, this->data->sizes.y, this->data->sizes.z, this->data->sizes.w);
    s->SetUniform4fv("objects", &(this->data->objects[0]), this->data->objects.size());
    s->SetUniform4fv("objColors", &(this->data->colors[0]), this->data->colors.size());
    s->SetUniform4fv("lightsDirection", &(this->data->directions[0]), this->data->directions.size());
    s->SetUniform4fv("lightsIntensity", &(this->data->intensities[0]), this->data->intensities.size());
    s->SetUniform4fv("lightPosition", &(this->data->lights[0]), this->data->lights.size());
    s->Unbind();
}

void Raytracing::moveEye(float x, float y, float z){
    this->data->eye.x += x;
    this->data->eye.y += y;
    this->data->eye.z += z;
}

float dot(glm::vec3& v1, glm::vec3& v2){
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}


float Raytracing::intersection(glm::vec3& srcPoint, glm::vec3& direction, int i){
    glm::vec3 obj(this->data->objects[i].x, this->data->objects[i].y, this->data->objects[i].z);
    glm::vec3 v(srcPoint.x - obj.x, srcPoint.y - obj.y, srcPoint.z - obj.z); // obj.xyz = center of sphere
    float vd = dot(direction, v);
    float sqrCalc = vd * vd - (dot(v, v) - this->data->objects[i].w * this->data->objects[i].w);
    if(sqrCalc < 0) return -1; // no intersection
    sqrCalc = sqrt(sqrCalc);
    float t1 = abs(-vd + sqrCalc), t2 = abs(-vd - sqrCalc);
    std::cout << t1 << ", " << t2 << std::endl;
    if (t1 <= 0 && t2 <= 0) return -1; // sphere is behind us
    float t = (t2 <= 0) ? t1 : t2;
    std::cout << srcPoint.x + t * direction.x << ", " << srcPoint.y + t * direction.y <<  ", " << srcPoint.z + t * direction.z << std::endl;
    return t;
}

void normalize(glm::vec3& vec){
    float sum = vec.x + vec.y + vec.z;
    vec.x /= sum;
    vec.y /= sum;
    vec.z /= sum;
}

int Raytracing::pickSphere(float x, float y){
    int index = -1, dist = -1;
    glm::vec3 eye(this->data->eye.x, this->data->eye.y, this->data->eye.z);
    glm::vec3 pos(x, y, 0);
    glm::vec3 direction(pos.x - eye.x, pos.y - eye.y, pos.z - eye.z);
    normalize(direction);
        std::cout << pos.x << ", " << pos.y << ", " << pos.z << std::endl;

    for(int i = 0; i < this->data->objects.size(); i++){
        glm::vec4 obj = this->data->objects[i];
        if(obj.w < 0) continue; // skipping planes
        /*
        if((x - obj.x) * (x - obj.x) + (y - obj.y) * (y - obj.y) <= obj.w * obj.w 
            && (dist == -1 || obj.z > dist)){
            index = i;
            dist = obj.z;
        }*/
    std::cout << "Object " << i << std::endl;

        float curr = intersection(pos, direction, i);
        if(curr != -1 && (dist == -1 || curr < dist)){
            index = i;
            dist = curr;
        }
    }
    return index;
}

void Raytracing::moveSphere(int index, float xr, float yr){
    if(index != -1){
        this->data->objects[index].x += 0.015 * xr;
        this->data->objects[index].y += 0.015 * yr;
    }
}

Raytracing::~Raytracing(void)
{
    delete this->data;
}
