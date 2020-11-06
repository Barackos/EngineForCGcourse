#pragma once
#include "scene.h"

class Mandelbrot : public Scene
{
public:
    Mandelbrot();
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
    void increaseP() { p++; }
    void decreaseP() { if (p > 2) p--; }
    void raiseColors() { colors *= 2; }
    void lowerColors() { if (colors > 2) colors /= 2; }
    void updateScreen(int width, int height);

private:
    unsigned int counter;
    unsigned int tmp, colors, p;
    float x, y;
};