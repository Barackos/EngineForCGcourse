#pragma once
#include "scene.h"
#include <queue>

class RubiksCube : public Scene
{
public:
    RubiksCube(int c);
    void Init();
    void Update(const glm::mat4 &MVP, const glm::mat4 &Model, const int shaderIndx);
    virtual void WhenPicked();

    ~RubiksCube(void);
    enum walls { LEFT, RIGHT, UP, BOTTOM, FRONT, BACK };
    enum tasks { ROTATE_LEFT, ROTATE_RIGHT, ROTATE_UP, ROTATE_BOTTOM, 
                 ROTATE_FRONT, ROTATE_BACK, ROTATE_SLOWER, ROTATE_FASTER, 
                 SWITCH_DIRECTION };
            
    void startRotation(int wall);
    void updateIndexes();
    void rotateCube(int i);
    void runTask(int task);
    void addTask(int task);
    void zoom(float z);

private:
    int c, c2, c3, counter, speed, rotations, rotatingWall;
    float degrees;
    glm::mat4 Proj;
    int *bricks, *toRotate;
    bool clockwise;
    std::queue<int> tasks;
};