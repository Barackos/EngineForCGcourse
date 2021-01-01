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
	virtual void WhenPicked();
	void Motion();
	void avi(glm::mat4& a);
	void stopPicking();
	void startPosition(int segNum);
	inline void setContinuity() { continuity = !continuity; }
	
	unsigned int TextureDesine(int width, int height);
	~Bezier(void);
	inline void ResetCounter() { tmp = counter; counter = 0; }
	inline void SetCounter() { counter = tmp; }

	void UpdatePosition( float xpos, float ypos);
private:
	glm::mat4 v;
	unsigned int counter;
	unsigned int tmp;
	int controlPoint;
	bool continuity;
	float x, y;
	Bezier1D* bezier;
	std::vector<int> cp;
	void addControlPoint(int i, int j);
	void clean();
	void MoveCP(int cPoint, glm::vec4 sp, glm::vec3 delta);
	void MoveControlPoint(int cPoint);
	void MoveControlPointContinuity(int cPoint);
	void AlignPrevPathControlPoint(int cPoint);
};

