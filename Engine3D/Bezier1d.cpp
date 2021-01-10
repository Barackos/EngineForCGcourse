#include "Bezier1D.h"
#include <iostream>
#include "cmath"
#include "glm/gtx/rotate_vector.hpp"


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

glm::vec4 Bezier1D::GetPointOnCurve(int segment, float t){
    glm::mat4 MG = M * glm::transpose(segments[segment]);
    glm::vec4 T = glm::vec4(t*t*t, t*t, t, 1);
    return T * MG;
} //returns point on curve in the requested segment for the value of t

float Bezier1D::angleBetweenX(glm::vec2 a) {
    if(a == glm::vec2(0)) return 0;
    glm::vec2 da=glm::normalize(a);
    glm::vec2 db=glm::vec2(1, 0);
    return glm::acos(glm::dot(da, db));
}

static std::vector<glm::vec2> GrahamsScan(glm::mat4x2 points){
    glm::vec2 fp;
    float minY = 2.0;
    std::vector<glm::vec2> rp = std::vector<glm::vec2>{points[0], points[1], points[2], points[3]};
    for(int i = 0; i < 4; i ++){
        if(rp[i].y < minY){
            fp = rp[i];
            minY = rp[i].y;
        }
    }
    rp.erase(std::remove(rp.begin(), rp.end(), fp), rp.end());
    std::sort(rp.begin(), rp.end(), [&fp](const glm::vec2& v, const glm::vec2& u) -> bool
    { 
        return Bezier1D::angleBetweenX(v - fp) < Bezier1D::angleBetweenX(u - fp);
    });
    rp.insert(rp.begin(), fp);

    for(int i = 1; i < 3; i ++){
        glm::vec2 pLine = rp[i] == rp[i - 1] ? glm::vec2(0,0) : glm::normalize(rp[i] - rp[i - 1]);
        glm::vec2 cLine = rp[i] == rp[i + 1] ? glm::vec2(0,0) : glm::normalize(rp[i + 1] - rp[i]);
        if(glm::acos(glm::dot(pLine, cLine)) > M_PI){
            rp.erase(std::remove(rp.begin(), rp.end(), rp[i]), rp.end());
            break; // no more than 1 point should be removed
        }
    }
    return rp;
}

static bool isIntersectingFromRight(glm::vec2 point, glm::vec2 e0, glm::vec2 e1){
    glm::vec2 &s = e0.y < e1.y ? e0 : e1;
    glm::vec2 &f = e0.y > e1.y ? e0 : e1;
    // std::cout << "CHECKING INTERSECTION: POINT IS [" << point.x << ", " << point.y << "]\n";
    // std::cout << "CHECKING INTERSECTION: EDGE IS  [" << s.x << ", " << s.y << "]\n";
    // std::cout << "                                [" << f.x << ", " << f.y << "]\n";
    if(point.y < s.y || point.y > f.y) 
        return false; // if point is not within the edge's y range there's no intersection
    // std::cout << "CHECKING INTERSECTION: WITHIN Y RANGE\n";
    if(e0 == e1) return e0.x >= point.x; // edge case when edge is a point
    glm::vec2 d = glm::normalize(f - s);
    float t = (point.y - s.y) / d.y; // s.y + t * d.y = point.y
    // std::cout << "CHECKING INTERSECTION: T IS " << t << ", X IS " << s.x + t * d.x << "\n";
    return s.x + t * d.x >= point.x; // >= means the edge is in right of the point
}

static bool areOnSameLine(glm::mat4x2 points, glm::vec2& direction){
    glm::vec2 vs[3];
    glm::vec2 v0 = glm::vec2(0,0);
    // sort points by y in ascending order????????
    direction = glm::vec2(1,0); // in case all points are equal
    for(int i = 0; i < 2; i++){
        if(points[i] != points[i + 1]){
            vs[i] = glm::normalize(points[i] - points[i + 1]);
            direction = vs[i];
        } else
            vs[i] = v0;
    }
    for(int i = 0; i < 2; i++)
        if(vs[i] != v0 && direction != vs[i]) 
            return false;
    return true;
}

bool Bezier1D::isInConvexHull(int segNum, glm::vec2 point) {
    std::vector<glm::vec2> ch;
    glm::mat4x2 points = glm::mat4x2(segments[segNum]);
    glm::vec2 direction;
    // std::cout << "CHECKING IF IN CH OF " << segNum << "\n";
    if(areOnSameLine(points, direction)){
        // std::cout << "POINTS ARE ON THE SAME LINE\n";
        // std::cout << "DIRECTION : [" << direction.x << ", " << direction.y << "]\n";
        glm::vec2 vp = 0.125f * glm::rotate(direction, 90.0f); // 1 is all the seen quarter
        ch = std::vector<glm::vec2>{ points[0] - vp, points[0] + vp, 
            points[3] + vp, points[3] - vp,}; // RECTANGLE CONVEX HULL
        // std::cout << "CREATED RECTANGLE CH\n";
    } else
        ch = GrahamsScan(points);
    // std::cout << "CH IS :\n";
    // for(glm::vec2 c : ch)
    //     std::cout << "[" << c.x << ", " << c.y << "]\n";
    ch.push_back(ch.front()); // copy start vertex to form a circle
    // detect if point is within ch
    int intersections = 0;
    for(int i = 0; i < ch.size() - 1; i++)
        if(isIntersectingFromRight(point, ch[i], ch[i + 1]))
            intersections++;
    // std::cout << "INTERSECTIONS " << intersections << "\n";
    return intersections % 2;
}


// glm::vec3 Bezier1D::GetVelosity(int segment, float t){

// } //returns the derivative of the curve in the requested segment for the value of t

void Bezier1D::SplitSegment(int segment, float t){
    glm::mat4 curr = segments[segment];
    glm::mat4 l = glm::mat4(curr[0], t * (curr[0] + curr[1]), 
                t * (curr[1] + curr[2]), glm::vec4(1));
    l[2] = t * (l[2] + l[1]);
    glm::mat4 r = glm::mat4(glm::vec4(1), t * (curr[1] + curr[2]),
                t * (curr[2] + curr[3]), curr[3]);
    r[1] = t * (r[2] + r[1]);
    l[3] = t * (l[2] + r[1]);
    r[0] = l[3];

    segments.erase(std::remove(segments.begin(), segments.end(), curr), segments.end());
    segments.insert(segments.begin() + segment, r);
    segments.insert(segments.begin() + segment, l);
    delete mesh;
    mesh = new MeshConstructor(GetLine(), false);
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

    // if(preserveC1); // TODO @@@@@@@@@@@@@@@@@@@
}  //change the position of one control point. when preserveC1 is true it may affect other  control points 

void Bezier1D::CurveUpdate(int pointIndx, float dx, float dy, bool preserveC1){
    MoveControlPoint(pointIndx / 3, pointIndx % 3, dx, dy, preserveC1);
    IndexedModel model = GetLine();
    mesh->ChangeLine(model);
}  //change the line in by using ChangeLine function of MeshConstructor and MoveControlPoint 

Bezier1D::~Bezier1D(void){

}
