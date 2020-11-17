#pragma once
#include "scene.h"
#include "sceneParser.h"

class Raytracing : public Scene
{
public:
    Raytracing(const std::string& fileName);
    void Init();
    void Update(const glm::mat4 &MVP, const glm::mat4 &Model, const int shaderIndx);

    //void WhenRotate();
    //void WhenTranslate();
    //void Motion();

    ~Raytracing(void);

private:
    unsigned int counter;
    SceneData* data;
};