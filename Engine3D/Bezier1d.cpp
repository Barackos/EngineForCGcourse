#include "Bezier1D.h"
#include <iostream>


Bezier1D::Bezier1D(int segNum, int res,int mode, int viewport) : Shape(0, mode)
{
    segments = std::vector<glm::mat4>();
    float pace = 2 / (segNum * 3.0);
    for(int i = 0; i < segNum; i++){
        glm::vec4 p = i > 0 ? segments[i - 1][3] : glm::vec4(-1.0,0,0,0);
        glm::vec4 p1 = glm::vec4(p.x + pace, 0, 0, 0);
        glm::vec4 p2 = glm::vec4(p1.x + pace, 0, 0, 0);
        glm::vec4 p3 = glm::vec4(p2.x + pace, 0, 0, 0);
        segments.push_back(glm::mat4(p, p1, p2, p3));
    }
    resT = res;
    M = glm::mat4(-1, 3, -3, 1,
                  3, -6, 3, 0,
                  -3, 3, 0, 0,
                  1, 0, 0, 0);
    RemoveViewport(0);
    AddViewport(viewport);
    delete mesh;
    mesh = new MeshConstructor(GetLine(), false);
}

IndexedModel Bezier1D::GetLine(){
    IndexedModel model;
	std::vector<LineVertex> vertices = std::vector<LineVertex>();
    int size = segments.size() * (resT + 1), counter = 0;
    unsigned int indices[size];

    for(int i = 0; i < segments.size(); i++){
        glm::mat4 MG = M * glm::transpose(segments[i]);
        for(float t = 0; t <= 1.0; t += (1.0 / resT)){
            glm::vec4 T = glm::vec4(t*t*t, t*t, t, 1);
            vertices.push_back(LineVertex(glm::vec3(T * MG),glm::vec3(1,0,0)));
            indices[counter++] = counter;
        }
    }

	 for(unsigned int i = 0; i < size; i++)
	{
		model.positions.push_back(*(vertices[i].GetPos()));
		model.colors.push_back(*(vertices[i].GetColor()));
        model.indices.push_back(indices[i]);
	}
        
	return model;
}	//generates a model for MeshConstructor Constructor

glm::vec4 Bezier1D::GetControlPoint(int segment, int indx) const{
    return segments[segment][indx];
} //returns a control point in the requested segment. indx will be 0,1,2,3, for p0,p1,p2,p3

glm::vec4 Bezier1D::GetPointOnCurve(int segment, int t){
    glm::mat4 MG = M * segments[segment];
    glm::vec4 T = glm::vec4(t*t*t, t*t, t, 1);
    return T * MG;
} //returns point on curve in the requested segment for the value of t

// glm::vec3 Bezier1D::GetVelosity(int segment, int t){

// } //returns the derivative of the curve in the requested segment for the value of t

void Bezier1D::SplitSegment(int segment, int t){
    if(segments.size() == 6) return;
    glm::mat4 curr = segments[segment];
    glm::mat4 l = glm::mat4(curr[0], (float)t * (curr[0] + curr[1]), 
                (float)t * (curr[1] + curr[2]), glm::vec4(1));
    l[2] = (float)t * (l[2] + l[1]);
    glm::mat4 r = glm::mat4(glm::vec4(1), (float)t * (curr[1] + curr[2]),
                (float)t * (curr[2] + curr[3]), curr[3]);
    r[1] = (float)t * (r[2] + r[1]);
    l[3] = (float)t * (l[2] + r[1]);
    r[0] = l[3];

    std::vector<glm::mat4> tmp = std::vector<glm::mat4>();
    for(int i = 0; i < segment; i++){
        tmp.push_back(segments[i]);
    }
    tmp.push_back(l);
    tmp.push_back(r);
    for(int i = segment + 1; i < segments.size(); i++){
        tmp.push_back(segments[i]);
    }
}  // split a segment into two parts

void Bezier1D::AddSegment(glm::vec4 p1, glm::vec4 p2, glm::vec4 p3){
    segments.push_back(glm::mat4(segments[segments.size()-1][3], p1, p2, p3));
}  // add a segment at the end of the curve

void Bezier1D::MoveControlPoint(int segment, int indx, float dx, float dy, bool preserveC1){
    if (segment == segments.size()) {
        segments[segment - 1][3].x += dx;
        segments[segment - 1][3].y += dy;
    } else {
        segments[segment][indx].x += dx;
        segments[segment][indx].y += dy;
        if(indx == 0 && segment > 0){
            segments[segment - 1][3].x += dx;
            segments[segment - 1][3].y += dy;
        }
    }

    if(preserveC1); // TODO @@@@@@@@@@@@@@@@@@@
}  //change the position of one control point. when preserveC1 is true it may affect other  control points 

void Bezier1D::CurveUpdate(int pointIndx, float dx, float dy, bool preserveC1){
    MoveControlPoint(pointIndx / 3, pointIndx % 3, dx, dy, preserveC1);
    IndexedModel model = GetLine();
    mesh->ChangeLine(model);
}  //change the line in by using ChangeLine function of MeshConstructor and MoveControlPoint 

Bezier1D::~Bezier1D(void){

}
