#pragma once
#include "scene.h"
#include "Bezier1D.h"

class Bezier : public Scene
{
public:
	
	Bezier();
	void Init();
	void Update(const glm::mat4 &MVP,const glm::mat4 &Model,const int  shaderIndx);
	
	void WhenRotate();
	void WhenTranslate();
	void WhenPicked();
	void BeforeDraw(int ref);
	void AfterDraw(const glm::mat4& MVP);
	void Draw(int shaderIndx, const glm::mat4& MVP, int viewportIndx, Camera *c, unsigned int flags);
	void Motion();
	void avi(glm::mat4& a);
	void stopPicking();
	void startPosition(int segNum);
	void scrollCB(float amt);
	inline void setContinuity() { continuity = !continuity; }
	
	unsigned int TextureDesine(int width, int height);
	~Bezier(void);
	inline void ResetCounter() { tmp = counter; counter = 0; }
	inline void SetCounter() { counter = tmp; }

	void UpdatePosition( float xpos, float ypos);
private:
	enum pickingState { NO_PICKING, PICKING_SHAPES, CREATING_RECTANGLE, PICKING_CONTROL_POINT, PICKING_SEGMENT };
	glm::mat4 v;
	unsigned int counter;
	unsigned int tmp;
	int controlPoint;
	int segment;
	bool continuity;
	float x, y, xabs, yabs;
	glm::vec2 r0, r1;
	Bezier1D* bezier;
	std::vector<int> cp;
	std::vector<int> rShapes;
	void AddNewShape(int type, int parent, unsigned int mode);
	void addControlPoint(int i, int j);
	void redrawControlPoints();
	void MoveCP(int cPoint, glm::vec4 sp, glm::vec3 delta);
	void RotateCP(int cPoint, int axisPoint, int dir);
	void RotateObj(int obj, int axisPoint, int dir);
	void RotateControlPoint(int cPoint, int centerPoint);
	void MoveControlPoint(int cPoint);
	void MoveControlPointContinuity(int cPoint);
	void AlignPrevPathControlPoint(int cPoint);
	void SelectShapesByRectangle();
	void checkRectangleEdge(int x, int y, int size, bool isX, int vpy);
};

