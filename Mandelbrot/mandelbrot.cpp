
#include <iostream>
#include <res/includes/GL/glew.h>
#include "mandelbrot.h"

Mandelbrot::Mandelbrot()
{
    this->counter = 0;
    colors = 16;
    p = 2;
}

void Mandelbrot::Init()
{
    unsigned int texIDs[3] = {0, 1, 2}; //texture ids for the material (currently all 3)
    unsigned int slots[3] = {0, 0, 0};  // ?
    AddShader("../res/shaders/myShader");
    AddTexture("../res/textures/pal.png", 2);
    AddMaterial(texIDs, slots, 1);
    AddShape(Plane, -1, TRIANGLES);
    SetShapeShader(0, 0);
}

void Mandelbrot::Update(const glm::mat4 &MVP, const glm::mat4 &Model, const int shaderIndx)
{
    Shader *s = shaders[shaderIndx];
    if (shapes[pickedShape]->GetMaterial() >= 0 && !materials.empty())
    {
        BindMaterial(s, shapes[pickedShape]->GetMaterial());
    }
    s->Bind();
    s->SetUniformMat4f("MVP", MVP);
    s->SetUniformMat4f("Normal", Model);
    s->SetUniform1i("p", p);
    s->SetUniform1i("colors", colors);

    s->Unbind();
}

Mandelbrot::~Mandelbrot(void)
{
}
