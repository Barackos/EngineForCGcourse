
#include <iostream>
#include <res/includes/GL/glew.h>
#include "RubiksCube.h"

RubiksCube::RubiksCube()
{
    this->counter = 0;
    colors = 16;
    p = 2;
    x = 0;
    y = 0;
    zoom = 1;
}

void RubiksCube::Init()
{
    unsigned int texIDs[3] = {0, 1, 2}; //texture ids for the material (currently all 3)
    unsigned int slots[3] = {0, 0, 0};  // ?
    AddShader("../res/shaders/myShader");
    AddTexture("../res/textures/pal.png", 2);
    AddMaterial(texIDs, slots, 1);
    AddShape(Plane, -1, TRIANGLES);
    SetShapeShader(0, 0);
}

void RubiksCube::Update(const glm::mat4 &MVP, const glm::mat4 &Model, const int shaderIndx)
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
    s->SetUniform1f("x", x);
    s->SetUniform1f("y", y);
    s->SetUniform1f("zoom", zoom);

    s->Unbind();
}

float boundOffset(float offset) {
    if (offset > 1.5) return 1.5;
    if (offset < -1.5) return -1.5;
    return offset;
}

void RubiksCube::updateOffsets(float rel_x, float rel_y) {
    x = boundOffset(x + rel_x * zoom);
    y = boundOffset(y + rel_y * zoom);
}

void RubiksCube::updateZoom(float z) {
    float prevZoom = zoom;
    if (zoom + z > 2)
        zoom = 2;
    else if (zoom + z > 0)
        zoom += z;
    
    if (prevZoom != zoom)
        std::cout << "Pixel Width: " << (2 / 840.0 * zoom) << std::endl;
}

RubiksCube::~RubiksCube(void)
{
}
