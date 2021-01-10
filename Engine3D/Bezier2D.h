#pragma once
#include "Bezier1D.h"

class Bezier2D : public Shape
{
	int circularSubdivision; //usualy 4 how many subdivision in circular direction
	std::vector <glm::vec4> controlPoints;
	int resT; 
	int resS;
	int subNum;

	void AddToModel(IndexedModel& model, float s, float t, const std::vector<glm::vec4> subSurf ,int subIndx);  // add a new Nurb to the model
	glm::mat4 CalcNurb(float p, bool isT, const std::vector<glm::vec4> subSurf);
	glm::vec3 CalcNormal(float s, float t, const std::vector<glm::vec4> subSurf);
	void CalcControlPoints(const Bezier1D* c);  // calculates control points cubic Bezier manifold.
	std::vector<glm::vec4> getSubSurf(int segmentS, int segmentT);
	glm::vec4 calcBezierPoint(glm::mat4 curve, float t);
	glm::vec2 getTexCoords(int segmentS, int segmentT, float s, float t);

public:
	// Bezier2D(void);
	Bezier2D(const Bezier1D* c, int circularSubdivision,int _resS,int _resT,int mode, int viewport = 0);
	IndexedModel GetSurface();	//generates a model for MeshConstructor Constructor
	Vertex* GetPointOnSurface(int segmentS, int segmentT, int s, int t);  //returns a point on the surface in the requested segment for value of t and s
	glm::vec3 GetNormal(int segmentS, int segmentT, float s, float t); //returns a normal of a point on the surface in the requested segment for value of t and s

	inline std::vector <glm::vec4>& c() { return controlPoints; }
	//void MoveControlPoint(int segmentS, int segmentT, int indx, bool preserveC1);
	~Bezier2D(void);
};