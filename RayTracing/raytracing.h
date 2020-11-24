#pragma once
#include "scene.h"
#include "sceneParser.h"

class Raytracing : public Scene
{
public:
    Raytracing(const std::string& fileName);
    void Init();
    void Update(const glm::mat4 &MVP, const glm::mat4 &Model, const int shaderIndx);

    void moveEye(float x, float y, float z);
    int pickSphere(float x, float y);
    void moveSphere(int index, float xr, float yr);

    ~Raytracing(void);

private:
    unsigned int counter;
    SceneData* data;

    float intersection(glm::vec3& srcPoint, glm::vec3& dirction, int i);
};