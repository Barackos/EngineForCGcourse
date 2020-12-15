
#include <iostream>
#include <res/includes/GL/glew.h>
#include "RubiksCube.h"

RubiksCube::RubiksCube(int c) {
    this->counter = 0;
    Proj = glm::mat4(1, 0, 0, 0,
                     0, 1, 0, 0,
                     0, 0, 1, 0,
                     0, 0, 0, 10);
    this->c = c;
    c2 = c * c;
    c3 = c2 * c;
    bricks = new int[c3];
    toRotate = new int[c2];
    clockwise = true;
    speed = 1;
    rotations = 0;
    degrees = 0;
    tasks = std::queue<int>();
}

void RubiksCube::Init() {
    unsigned int texIDs[3] = {0, 1, 2}; //texture ids for the material (currently all 3)
    unsigned int slots[3] = {0, 0, 0};
    AddShader("../res/shaders/pickingShader");
    AddShader("../res/shaders/basicShader2");
    AddTexture("../res/textures/plane.png", 2);
    AddMaterial(texIDs, slots, 1);
    for (int i = 0; i < c3; i++) {
        AddShape(Cube, -1, TRIANGLES);
        SetShapeShader(i, 1);
        pickedShape = i;
        SetShapeMaterial(i, 0);
        bricks[i] = i;
        float d = 0.5 * (c - 1);
        ShapeTransformation(xTranslate, 2 * (i % c - d));
        ShapeTransformation(yTranslate, 2 * ((i % c2) / c - d));
        ShapeTransformation(zTranslate, -2 * (i / c2 - d));
    }
    pickedShape = -1;
}

void RubiksCube::startRotation(int wall) {
    for (int i = 0; i < c2; i++) {
        switch (wall) {
        case LEFT:
            toRotate[i] = c * i;
            break;
        case RIGHT:
            toRotate[i] = c * i + c - 1;
            break;
        case UP:
            toRotate[i] = c2 * (i / c) + i % c + c * (c - 1);
            break;
        case BOTTOM:
            toRotate[i] = c2 * (i / c) + i % c;
            break;
        case FRONT:
            toRotate[i] = i + (c - 1) * c2;
            break;
        case BACK:
            toRotate[i] = i;
            break;
        }
    }
    degrees = speed == 0 ? 0.05f : speed == 1 ? 0.1f : 0.5f;
    rotations = 90 / degrees;
    degrees *= clockwise ? 1 : -1;
    if (wall == BACK || wall == UP || wall == RIGHT)
        degrees *= -1;
    this->rotatingWall = wall;
}

void transpose(int n, int **rotationMatrix) {
    for (int i = 0; i < n; i++) {
        for (int j = i; j < n; j++) {
            int temp = rotationMatrix[i][j];
            rotationMatrix[i][j] = rotationMatrix[j][i];
            rotationMatrix[j][i] = temp;
        }
    }
}

void switchLines(int n, int **rotationMatrix) {
    for (int i = 0; i < n / 2; i++) {
        int *temp = rotationMatrix[i];
        rotationMatrix[i] = rotationMatrix[n - 1 - i];
        rotationMatrix[n - 1 - i] = temp;
    }
}

void rotate(int n, int **rotationMatrix) {
    switchLines(n, rotationMatrix);
    transpose(n, rotationMatrix);
}

void adTranspose(int n, int **rotationMatrix) {
    transpose(n, rotationMatrix);
    rotate(n, rotationMatrix);
    rotate(n, rotationMatrix);
}

void RubiksCube::updateIndexes() {
    std::cout << "BEFORE:\n";
    for (int i = 0; i < c3; i++)
        std::cout << bricks[i] << ", ";
    std::cout << "\n";

    int **rotationMatrix = new int*[c];
    for (int i = 0; i < c2; i++) {
        if (i % c == 0)
            rotationMatrix[i / c] = new int[c];
        rotationMatrix[i / c][i % c] = toRotate[i];
    }
    std::cout << "MATRIX:\n";
    for (int i = 0; i < c; i++){
        for (int j = 0; j < c; j++)
            std::cout << rotationMatrix[i][j] << "\t";
        std::cout << "\n";
    }
    std::cout << "\n";
    std::cout << "CUBES TO ROTATE:\n";
    std::cout << "INDEXES: \t";
    for (int i = 0; i < c2; i++)
        std::cout << toRotate[i] << ", ";
    std::cout << "\n";
    std::cout << "VALUES: \t";
    for (int i = 0; i < c2; i++)
        std::cout << bricks[toRotate[i]] << ", ";
    std::cout << "\n";
    switchLines(c, rotationMatrix);
    if (clockwise) {
        switch(rotatingWall) {
            case LEFT:
            case UP:
            case BACK:
                adTranspose(c, rotationMatrix);
                break;
            default:
                transpose(c, rotationMatrix);
                break;
        }
    }
    else {
        switch(rotatingWall) {
            case RIGHT:
            case BOTTOM:
            case FRONT:
                adTranspose(c, rotationMatrix);
                break;
            default:
                transpose(c, rotationMatrix);
                break;
        }
    }

    //if(rotatingWall == LEFT || rotatingWall == RIGHT)
    //    adTranspose(c, rotationMatrix);
    //switchLines(c, rotationMatrix);
    std::cout << "MATRIX AFTER:\n";
    for (int i = 0; i < c; i++){
        for (int j = 0; j < c; j++)
            std::cout << rotationMatrix[i][j] << "\t";
        std::cout << "\n";
    }
    int * values = new int[c3];
    for (int i = 0; i < c3; i++)
        values[i] = bricks[i]; // copy current bricks
    for (int i = 0; i < c2; i++)
        bricks[toRotate[i]] = values[rotationMatrix[i / c][i % c]];
    delete[] values;
    delete[] rotationMatrix;
    std::cout << "AFTER:\n";
    for (int i = 0; i < c3; i++)
        std::cout << bricks[i] << ", ";
    std::cout << "\n";
}

void RubiksCube::runTask(int task) {
    switch(task) {
        case ROTATE_LEFT:
        case ROTATE_RIGHT:
        case ROTATE_UP:
        case ROTATE_BOTTOM:
        case ROTATE_FRONT:
        case ROTATE_BACK:
            startRotation(task);
            break;
        case ROTATE_SLOWER:
            if(speed > 0) speed--;
            else std::cout << "reached minimum speed" << std::endl;
            break;
        case ROTATE_FASTER:
            if(speed < 2) speed++;
            else std::cout << "reached maximum speed" << std::endl;
            break;
        case SWITCH_DIRECTION:
            clockwise = !clockwise;
            break;   
    }
}

void RubiksCube::Update(const glm::mat4 &MVP, const glm::mat4 &Model, const int shaderIndx) {
    Shader *s = shaders[shaderIndx];
    if (shapes[pickedShape]->GetMaterial() >= 0 && !materials.empty())
        BindMaterial(s, shapes[pickedShape]->GetMaterial());
    //textures[0]->Bind(0);
    s->Bind();
    //rotatingWall = BACK;
    //degrees = 0.5f;
    //rotateCube(bricks[0]);
    if (rotations){
        for (int i = 0; i < c2; i++)
            rotateCube(bricks[toRotate[i]]);
        if (--rotations == 0)
            updateIndexes();
    } else if (!tasks.empty()) {
        runTask(tasks.front());
        tasks.pop();
    }

    if (shaderIndx != 0) {
        s->SetUniformMat4f("Model", Model);
        s->SetUniformMat4f("View", MVP);
        s->SetUniformMat4f("Proj", Proj);
    }
    else
    {
        s->SetUniformMat4f("Model", glm::mat4(1));
        s->SetUniformMat4f("View", glm::mat4(1));
        s->SetUniformMat4f("Proj", Proj);
    }
    s->SetUniform1i("sampler1", materials[shapes[pickedShape]->GetMaterial()]->GetSlot(0));
    if (shaderIndx != 0)
        s->SetUniform1i("sampler", materials[shapes[pickedShape]->GetMaterial()]->GetSlot(1));

    s->Unbind();
}

void RubiksCube::rotateCube(int i) {
    int tmp = pickedShape;
    pickedShape = i;
    switch (rotatingWall) {
        case LEFT:
        case RIGHT:
            ShapeTransformation(xRotate, degrees);        
            break;
        case UP:
        case BOTTOM:
            ShapeTransformation(yRotate, degrees);
            break;
        case FRONT:
        case BACK:
            ShapeTransformation(zRotate, degrees);
            break;
    }
    pickedShape = tmp;
}

void RubiksCube::addTask(int task) {
    tasks.push(task);
}

RubiksCube::~RubiksCube(void) {
    delete bricks;
    delete toRotate;
}
