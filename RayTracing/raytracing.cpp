
#include <iostream>
#include <res/includes/GL/glew.h>
#include "raytracing.h"

Raytracing::Raytracing(const std::string& fileName)
{
    this->data = new SceneData();
    SceneParser parser(fileName, this->data);
}

void Raytracing::Init()
{
    unsigned int texIDs[3] = {0, 1, 2}; //texture ids for the material (currently all 3)
    unsigned int slots[3] = {0, 0, 0};  // ?
    int si = AddShader("../res/shaders/rayTracingShader");
    // set uniforms
    Shader *s = shaders[si];
    
    s->Bind();
    s->SetUniform4f("eye", this->data->eye.x, this->data->eye.y, this->data->eye.z, this->data->eye.w);
    s->SetUniform4f("ambient", this->data->ambient.x, this->data->ambient.y, this->data->ambient.z, this->data->ambient.w);
    s->SetUniform4i("sizes", this->data->sizes.x, this->data->sizes.y, this->data->sizes.z, this->data->sizes.w);
    s->SetUniform4fv("objects", &(this->data->objects[0]), this->data->objects.size());
    s->SetUniform4fv("objColors", &(this->data->colors[0]), this->data->colors.size());
    s->SetUniform4fv("lightsDirection", &(this->data->directions[0]), this->data->directions.size());
    s->SetUniform4fv("lightsIntensity", &(this->data->intensities[0]), this->data->intensities.size());
    s->SetUniform4fv("lightPosition", &(this->data->lights[0]), this->data->lights.size());
    s->Unbind();
    
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
    s->Unbind();
}

Raytracing::~Raytracing(void)
{
    delete this->data;
}
