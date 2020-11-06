#pragma once
#include "scene.h"

class Mandelbrot : public Scene
{
public:
    Mandelbrot(int width, int height);
    void Init();
    void Update(const glm::mat4 &MVP, const glm::mat4 &Model, const int shaderIndx);

    //void WhenRotate();
    //void WhenTranslate();
    //void Motion();

    ~Mandelbrot(void);
    inline void ResetCounter()
    {
        tmp = counter;
        counter = 0;
    }
    inline void SetCounter() { counter = tmp; }
    void change_rgb(bool plus);
    unsigned int TextureDesine(int width, int height);

private:
    unsigned int counter;
    unsigned int tmp, colors, p, width, height;
    float x, y;
};