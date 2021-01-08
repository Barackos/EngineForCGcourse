#include "Bezier2D.h"
#include "Bezier2D.h"
#include "glm/gtx/rotate_vector.hpp"

// Bezier2D::Bezier2D(void) : Shape(0, mode){
    
// }

Bezier2D::Bezier2D(const Bezier1D* c, int circularSubdivision, int _resS, int _resT, int mode, int viewport) : Shape(0, mode)
{
    this->circularSubdivision = circularSubdivision; // SHOULD BE 4. EACH ONE IS THE SAME BEZIER SURFACE, forming a cirecle in space together
    resS = _resS;
    resT = _resT;
    subNum = c->GetSegmentsNum();
    controlPoints = std::vector<glm::vec4>();
    CalcControlPoints(c);
    delete mesh;
    mesh = new MeshConstructor(GetSurface(), true);
}

void Bezier2D::CalcControlPoints(const Bezier1D* c){
    glm::vec4 p0, p1, p2, p3;
    float rotAngle = 360 / (circularSubdivision * 4);
    for(int i = 0; i < subNum; i++){ // each surface is 16 control points.
        p0 = c->GetControlPoint(i, 0);
        p1 = c->GetControlPoint(i, 1);
        p2 = c->GetControlPoint(i, 2);
        p3 = c->GetControlPoint(i, 3); // all points come with z = 0
        for(int j = 0; j < 4; j++){
            controlPoints.insert(controlPoints.end(), { p0, p1, p2, p3 });
            p0 = glm::rotateY(p0, rotAngle);
            p1 = glm::rotateY(p1, rotAngle);
            p2 = glm::rotateY(p2, rotAngle);
            p3 = glm::rotateY(p3, rotAngle);
        }
    }
    // for each segment - u should be the segment's control points,
    // v should form a quater of circle with it's control points.
    // in general, v should form 1/circularSubdivision of a circle.
    // when 4, we get 4 surfaces from each segment of the 1D curve.
}  // calculates control points cubic Bezier manifold.

static void rotateSurface(std::vector<glm::vec4> &surface, float angle){
    for(int i = 0; i < surface.size(); i++)
        surface[i] = glm::rotateY(surface[i], angle);
}

glm::vec4 Bezier2D::CalcNurbs(float s, float t, const std::vector<glm::vec4> subSurf){
    // calc the mid curve. first go on s/t to find place, then calc the nurb.

} 

std::vector<glm::vec4> Bezier2D::getSubSurf(int segmentS, int segmentT){
    std::vector<glm::vec4> subSurf = std::vector<glm::vec4>
        (controlPoints.begin() + (segmentT * 16), controlPoints.begin() + (segmentT * 16) + 16);
    // segmentS should be 0 - circularSubdivision - 1
    rotateSurface(subSurf, 360 / circularSubdivision * segmentS);
    return subSurf;
}

Vertex* Bezier2D::GetPointOnSurface(int segmentS, int segmentT, int s, int t){
    std::vector<glm::vec4> subSurf = getSubSurf(segmentS, segmentT);
    glm::mat4 sCurve = CalcNurbs(s, subSurf);
    
    glm::mat4 M = glm::mat4(-1, 3, -3, 1,
                             3, -6, 3, 0,
                            -3, 3, 0, 0,
                             1, 0, 0, 0);
    glm::mat4 MG = M * sCurve;
    glm::vec4 T = glm::vec4(t*t*t, t*t, t, 1);
    glm::vec3 normal = GetNormal(segmentS, segmentT, s, t);
    return new Vertex(glm::vec3(T * MG), glm::vec2(s, t), normal, glm::vec3(1, 0, 0));
    // s is the v circular path, t is the place on the curve
    // get the sub surface according to segments, then call CalcNurbs
    // to find the curve and get the vertex. (
    // or maybe returns a control point, we construct a curve from 4 of them and then
    // find the vertex using t?)
}  //returns a point on the surface in the requested segment for value of t and s

IndexedModel Bezier2D::GetSurface(){
    IndexedModel model;
	std::vector<Vertex> vertices = std::vector<Vertex>();
    int size = circularSubdivision * subNum * (resS + 1) * (resT + 1), counter = 0;
    unsigned int indices[size];
}	//generates a model for MeshConstructor Constructor

glm::vec3 Bezier2D::CalcNormal(float s, float t, const std::vector<glm::vec4> subSurf){
    // should be a cross product of two vectors originated from the same control point,
    // one in s direction and one in t direction
}

glm::vec3 Bezier2D::GetNormal(int segmentS, int segmentT, int s, int t){
    // find sub surface according to segment, then call CalcNormal
    std::vector<glm::vec4> subSurf = getSubSurf(segmentS, segmentT); 
    return CalcNormal(s, t, subSurf);
} //returns a normal of a point on the surface in the requested segment for value of t and s

void Bezier2D::AddToModel(IndexedModel& model, float s, float t, const std::vector<glm::vec4> subSurf ,int subIndx){

}  // add a new Nurb to the model

Bezier2D::~Bezier2D(void){

}