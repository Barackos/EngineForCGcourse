
#include <iostream>
#include <res/includes/GL/glew.h>
#include "mandelbrot.h"

Mandelbrot::Mandelbrot() {
    counter = 0;
    k = -1.0;
    diff = 0.01f;
    rgb = 2;
    tex = 0;
}

void Mandelbrot::Init() {
    unsigned int texIDs[3] = { 0 , 1, 2}; //texture ids for the material (currently all 3)
    unsigned int slots[3] = { 0 , 0, 0 }; // ?
    AddShader("../res/shaders/myShader");

    AddTexture("../res/textures/pal.png", 2);
    AddTexture("../res/textures/grass.bmp", 2);
    AddTexture("../res/textures/box0.bmp", 2);
    //TextureDesine(800,800);

    AddMaterial(texIDs,slots, 3); // creates new material with all 3 textures
    //materials[0]->rotateTexs();
    AddMaterial(texIDs+1,slots+1, 2); //only 2 textures, starting from second

    AddShape(Plane, -1, TRIANGLES);
    SetShapeShader(0, 0);

    //AddShape(Axis, -1, TRIANGLES);
    //SetShapeShader(1, 0);
    //SetShapeMaterial(1, 1);
}

unsigned int Mandelbrot::TextureDesine(int width, int height)
{
    unsigned char* data = new unsigned char[width * height * 4];
    for (size_t i = 0; i < width; i++)
    {
        for (size_t j = 0; j < height; j++)
        {
            data[(i * height + j) * 4] = (i + j) % 256;
            data[(i * height + j) * 4 + 1] = (i + j * 2) % 256;
            data[(i * height + j) * 4 + 2] = (i * 2 + j) % 256;
            data[(i * height + j) * 4 + 3] = (i * 3 + j) % 256;
        }
    }
    textures.push_back(new Texture(width, height));
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data); //note GL_RED internal format, to save
    glBindTexture(GL_TEXTURE_2D, 0);
    delete[] data;
    return(textures.size() - 1);
}

void Mandelbrot::Update(const glm::mat4 &MVP, const glm::mat4 &Model, const int shaderIndx) {
    Shader *s = shaders[shaderIndx];
    if (shapes[pickedShape]->GetMaterial() >= 0 && !materials.empty()) {
        BindMaterial(s, shapes[pickedShape]->GetMaterial());
    }
    s->Bind();
    s->SetUniformMat4f("MVP", MVP);
    s->SetUniformMat4f("Normal", Model);
    //s->SetUniform1i("sampler", materials[shapes[pickedShape]->GetMaterial()]->GetSlot(0));
    s->SetUniform4f("ourColor", 0.2 - k, 0.2 - k, 0.2 - k, 0.2 - k);
    k += diff;
    if(++counter == 200){
        diff *= -1;
        counter = 0;
        tex = (tex + 1) % 3;
        s->SetUniform1i("sampler", tex);
    }
    s->SetUniform1f("k", k);
    s->SetUniform1i("rgb", rgb);
    s->Unbind();
}

Mandelbrot::~Mandelbrot(void) {

}

void Mandelbrot::change_rgb(bool plus) {
    if(plus && rgb < 2) rgb++;
    if(!plus && rgb > 0) rgb--;
}


