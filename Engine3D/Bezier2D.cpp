#include "Bezier2D.h"
#include "Bezier2D.h"
#include "glm/gtx/rotate_vector.hpp"
#include <iostream>

// Bezier2D::Bezier2D(void) : Shape(0, mode){
    
// }

Bezier2D::Bezier2D(const Bezier1D* c, int circularSubdivision, int _resS, int _resT, int mode, int viewport) : Shape(0, mode)
{
    this->circularSubdivision = circularSubdivision;
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
    float rotAngle = 360.0f / (circularSubdivision * 3);
    for(int i = 0; i < subNum; i++){ // each surface is 16 control points.
        p0 = glm::rotateZ(c->GetControlPoint(i, 0), -90.0f);
        p1 = glm::rotateZ(c->GetControlPoint(i, 1), -90.0f);
        p2 = glm::rotateZ(c->GetControlPoint(i, 2), -90.0f);
        p3 = glm::rotateZ(c->GetControlPoint(i, 3), -90.0f); // all points come with z = 0
        for(int j = 0; j < 4; j++){
            // std::cout << "CP " << i*16 + j * 4 << ": [" << p0.x << "\t" << p0.y << "\t" << p0.z << "]\n";
            // std::cout << "CP " << i*16 + j * 4 + 1 << ": [" << p1.x << "\t" << p1.y << "\t" << p1.z << "]\n";
            // std::cout << "CP " << i*16 + j * 4 + 2 << ": [" << p2.x << "\t" << p2.y << "\t" << p2.z << "]\n";
            // std::cout << "CP " << i*16 + j * 4 + 3 << ": [" << p3.x << "\t" << p3.y << "\t" << p3.z << "]\n";
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

glm::vec4 Bezier2D::calcBezierPoint(glm::mat4 curve, float t){
    glm::mat4 M = glm::mat4(-1, 3, -3, 1,
                             3, -6, 3, 0,
                            -3, 3, 0, 0,
                             1, 0, 0, 0);
    glm::mat4 MG = M * glm::transpose(curve);
    glm::vec4 T = glm::vec4(t*t*t, t*t, t, 1);
    return T * MG;
}

glm::mat4 Bezier2D::CalcNurb(float p, bool isT, const std::vector<glm::vec4> subSurf){
    // calc the mid curve. first go on s/t to find place, then calc the nurb.
    glm::mat4 curve = glm::mat4();
    for(int i = 0; i < 4; i++){
        glm::mat4 c = isT ? glm::mat4(subSurf[i * 4], subSurf[i * 4 + 1], subSurf[i * 4 + 2], subSurf[i * 4 +3])
         : glm::mat4(subSurf[i], subSurf[i + 4], subSurf[i + 8], subSurf[i + 12]);
        curve[i] = calcBezierPoint(c , p);
    }
    return curve;
} 

std::vector<glm::vec4> Bezier2D::getSubSurf(int segmentS, int segmentT){
    std::vector<glm::vec4> subSurf = std::vector<glm::vec4>
        (controlPoints.begin() + (segmentT * 16), controlPoints.begin() + (segmentT * 16) + 16);
    // segmentS should be 0 - circularSubdivision - 1
    rotateSurface(subSurf, 360.0f / circularSubdivision * segmentS);
    if(segmentS) controlPoints.insert(controlPoints.end(), subSurf.begin(), subSurf.end());
    return subSurf;
}

glm::vec2 Bezier2D::getTexCoords(int segmentS, int segmentT, float s, float t){
    float sizeSg = (float)(segmentS) / circularSubdivision;
    float sizeTg = (float)(segmentT) / subNum;
    // std::cout << sizeSg + s * 1 / circularSubdivision << ", " << sizeTg + t * 1 / subNum << "\n";
    return glm::vec2(sizeSg + s / circularSubdivision, sizeTg + t / subNum);
}

Vertex* Bezier2D::GetPointOnSurface(int segmentS, int segmentT, float s, float t){
    std::vector<glm::vec4> subSurf = getSubSurf(segmentS, segmentT);
    glm::mat4 sCurve = CalcNurb(s, false, subSurf);
    glm::vec3 position = glm::vec3(calcBezierPoint(sCurve, t));
    glm::vec3 normal = CalcNormal(s, t, subSurf);
    glm::vec2 texCoords = getTexCoords(segmentS, segmentT, s, t);
    return new Vertex(position, texCoords, normal, glm::vec3(1, 0, 0));
}  //returns a point on the surface in the requested segment for value of t and s

IndexedModel Bezier2D::GetSurface(){
    IndexedModel model;
	std::vector<Vertex> vertices = std::vector<Vertex>();
	std::vector<unsigned int> indicesT = std::vector<unsigned int>(), indicesB = std::vector<unsigned int>();
    int sizeS = circularSubdivision * (resS + 1);
    int sizeT = subNum * (resT + 1);
    unsigned int counter = 0;
    unsigned int indicesMap[sizeS][sizeT];
    for(int j = 0; j < circularSubdivision; j++) // segmentS
        for(float s = 0; s <= 1.0; s += 1.0f / resS)
            for(int i = 0; i < subNum; i++) // segmentT  
                for(float t = 0; t <= 1.0f; t += 1.0f / resT){
                    vertices.push_back(*GetPointOnSurface(j, i, s, t));
                    int idxS = j * (resS + 1) + (int)(s * resS);
                    int idxT = i * (resT + 1) + (int)(t * resT);
                    indicesMap[idxS][idxT] = counter++;
                    if(t == 0 && i == 0)
                        indicesT.push_back(counter - 1);
                    else if((int)(t * resT + 1) == resT && i == subNum - 1){
                        indicesB.push_back(counter - 1);
                    }
                    // std::cout << "[ " << idxT << ", " << idxS << " ]\n";
                    // std::cout << "VERTEX " << counter - 1 << ": [" << vertices.back().GetPos()->x << "\t" << vertices.back().GetPos()->y << "\t" << vertices.back().GetPos()->z << "]\n";
                }

    glm::vec3 tcc = glm::vec3(0, controlPoints[0].y, 0);
    glm::vec3 bcc = glm::vec3(0, controlPoints[(subNum - 1) * 16 + 3].y, 0);
    Vertex t = Vertex(tcc, glm::vec2(0.5f, 0.5f), glm::vec3(0, 1, 0), glm::vec3(1, 0, 0));
    Vertex b = Vertex(bcc, glm::vec2(0.5f, 0.5f), glm::vec3(0, -1, 0), glm::vec3(1, 0, 0));
    vertices.insert(vertices.end(), { t, b });
    for(int i = 0; i < indicesT.size() - 1; i++){
        model.indices.insert(model.indices.end(), { counter, indicesT[i], indicesT[i + 1] });
        model.indices.insert(model.indices.end(), { counter + 1, indicesB[i], indicesB[i + 1] });
    }

    for(unsigned int i = 0; i < vertices.size(); i++) {
		model.positions.push_back(*vertices[i].GetPos());
		model.colors.push_back(*vertices[i].GetColor());
		model.normals.push_back(*vertices[i].GetNormal());
		model.texCoords.push_back(*vertices[i].GetTexCoord());
	}

    for (int i = 0; i < sizeS - 1; i++) {
        for (int j = 0; j < sizeT - 1; j++) {
            // model.indices.insert(model.indices.end(), { 
            model.indices.push_back(indicesMap[i][j]);
            model.indices.push_back(indicesMap[i][j + 1]);
            model.indices.push_back(indicesMap[i + 1][j + 1]);
            model.indices.push_back(indicesMap[i][j]);
            model.indices.push_back(indicesMap[i + 1][j]);
            model.indices.push_back(indicesMap[i + 1][j + 1]);
            // std::cout << "[ " << indicesMap[i][j] << "\t" << indicesMap[i][j + 1]
            //           << "\t" << indicesMap[i + 1][j + 1] << "\t" << indicesMap[i + 1][j] << "]\n";
        }
    }

	return model;
}	//generates a model for MeshConstructor Constructor

glm::vec3 Bezier2D::CalcNormal(float s, float t, const std::vector<glm::vec4> subSurf){
    glm::mat4 Mt = glm::mat4(0, -3,  6, -3,
                             0,  9, -12, 3,
                             0, -9,  6,  0,
                             0,  3,  0,  0);
    glm::mat4 G = CalcNurb(t, true, subSurf);
    glm::vec4 S = glm::vec4(s*s*s ,s*s, s, 1);
    glm::vec3 dS = glm::vec3(S * Mt * glm::transpose(G));
    G = CalcNurb(s, false, subSurf);
    glm::vec4 T = glm::vec4(t*t*t ,t*t, t, 1);
    glm::vec3 dT = glm::vec3(T * Mt * glm::transpose(G));
    glm::vec3 normal = glm::cross(dS, dT);
    return normal == glm::vec3(0) ? normal : glm::normalize(-normal);
}

glm::vec3 Bezier2D::GetNormal(int segmentS, int segmentT, float s, float t){
    // find sub surface according to segment, then call CalcNormal
    std::vector<glm::vec4> subSurf = getSubSurf(segmentS, segmentT); 
    return CalcNormal(s, t, subSurf);
} //returns a normal of a point on the surface in the requested segment for value of t and s

void Bezier2D::AddToModel(IndexedModel& model, float s, float t, const std::vector<glm::vec4> subSurf ,int subIndx){

}  // add a new Nurb to the model

Bezier2D::~Bezier2D(void){

}