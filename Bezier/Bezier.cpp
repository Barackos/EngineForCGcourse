#include "Bezier.h"
#include <iostream>
#include "GL/glew.h"
#include <algorithm>

static void printMat(const glm::mat4 mat)
{
	std::cout<<" matrix:"<<std::endl;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
			std::cout<< mat[j][i]<<" ";
		std::cout<<std::endl;
	}
}

Bezier::Bezier() : Scene()
{
	bezier = 0;
	counter = 10;
	v = glm::mat4(1);
	cp = std::vector<int>();
	continuity = false;
}

//Bezier::Bezier(float angle ,float relationWH, float near, float far) : Scene(angle,relationWH,near,far)
//{ 	
//}

void Bezier::addControlPoint(int i, int j){
	AddShape(Octahedron, -1, TRIANGLES);
	int cpShape = shapes.size() - 1;
	cp.push_back(cpShape);
	glm::vec4 point = bezier->GetControlPoint(i, j);
	shapes[cpShape]->MyTranslate(glm::vec3(point.x,point.y,0), 0);
	shapes[cpShape]->MyScale(glm::vec3(0.05,0.05,0.05));
	shapes[cpShape]->RemoveViewport(0);
	shapes[cpShape]->AddViewport(1);
}

void Bezier::Init()
{		
	unsigned int texIDs[3] = { 0 , 1, 2};
	unsigned int slots[3] = { 0 , 1, 2 };
	isActive = false; // TODO REMOVE @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	AddShader("../res/shaders/pickingShader");	
	AddShader("../res/shaders/basicShader");
	AddShader("../res/shaders/cubemapShader");
	AddTexture("../res/textures/box0.bmp", 2);
	AddTexture("../res/textures/grass.bmp", 2);

	AddShape(Cube, -1, TRIANGLES);
	SetShapeShader(0, 2);
	shapes[0]->MyScale(glm::vec3(4,4,4));//2.5,2.5,2.5));
	// shapes[0]->MyTranslate(glm::vec3(0,0,4.1), 0);
	std::vector<std::string> faces{ "right.jpg", "left.jpg", "top.jpg", "bottom.jpg", "front.jpg", "back.jpg" };
	textures.push_back(new Texture(faces));
	AddMaterial(texIDs,slots, 3);
	
	AddShape(Axis, -1, LINES);
	shapes[1]->AddViewport(1);
	shapes[1]->RemoveViewport(0);
	shapes.push_back(bezier);
	startPosition(3);

	AddShape(Cube, -1, TRIANGLES);
	AddShape(Octahedron, -1, TRIANGLES);
	
	AddShapeCopy(1, -1, TRIANGLES);

	// pickedShape = 3;
	// ShapeTransformation(zTranslate, -4);
	// pickedShape = 4;
	// ShapeTransformation(yTranslate, 4);
	// pickedShape = 5;
	// ShapeTransformation(xScale, 5);
	// ShapeTransformation(yScale, 5);
	// ShapeTransformation(zScale, 5);
	// pickedShape = 5;
	// ShapeTransformation(xTranslate, 7);
	// pickedShape = -1;
}

void Bezier::Update(const glm::mat4 &MVP,const glm::mat4 &Model,const int  shaderIndx)
{	
	Shader *s = shaders[shaderIndx];
	int r = ((pickedShape+1) & 0x000000FF) >>  0;
	// int g = ((pickedShape+1) & 0x0000FF00) >>  8;
	// int b = ((pickedShape+1) & 0x00FF0000) >> 16;
	if (shapes[pickedShape]->GetMaterial() >= 0 && !materials.empty())
		BindMaterial(s, shapes[pickedShape]->GetMaterial());
	//textures[0]->Bind(0);
	s->Bind();

	// if(shaderIndx != 2){
		s->SetUniformMat4f("View", MVP);
		s->SetUniformMat4f("Model", Model);
		// s->SetUniformMat4f("avi", v);
	// }else{
	// 	glm::mat4 a = glm::mat4(MVP);
	// 	a[3][0] = 0;
	// 	a[3][1] = 0;
	// 	a[3][2] = 0;
	// 	s->SetUniformMat4f("View", a);
	// 	// s->SetUniformMat4f("Normal", glm::mat4(1));
	// }
	s->SetUniform1i("sampler", materials[shapes[pickedShape]->GetMaterial()]->GetSlot(pickedShape == 2 ? 1: 0));
	s->SetUniform1i("skybox", materials[shapes[pickedShape]->GetMaterial()]->GetSlot(2));
	// if(shaderIndx!=1)
	// 	s->SetUniform1i("sampler2", materials[shapes[pickedShape]->GetMaterial()]->GetSlot(1));
	if (shaderIndx == 0){
        s->SetUniformMat4f("Obj", shapes[pickedShape]->MakeTrans());
	}
	s->SetUniform4f("lightColor", r, 1, 1, r / 255.0);
	// s->SetUniform1ui("counter", counter);
	// s->SetUniform1f("x", x);
	// s->SetUniform1f("y", y);
	s->Unbind();
}

void Bezier::UpdatePosition(float xpos,  float ypos)
{
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	x = xpos / viewport[2];
	y =  1 - ypos / viewport[3]; 
}

void Bezier::clean() {
	delete bezier;
	std::vector<Shape*> cpShapes = std::vector<Shape*>();
	for(int j = 0; j < cp.size(); j++) {
		Shape *shape = shapes[cp[j]];
		cpShapes.push_back(shape);
	}
	for (int i = 0; i < cpShapes.size(); i++) {
		shapes.erase(std::remove(shapes.begin(), shapes.end(), cpShapes[i]), shapes.end());
	}
	cp.clear();
}

void Bezier::startPosition(int segNum){
	if(bezier) clean();
	bezier = new Bezier1D(segNum, 50, LINE_STRIP, 1);
	shapes[2] = bezier;
	bezier->CurveUpdate(1, -0.6666 - bezier->GetControlPoint(0, 1).x, 0.37267799625);
	bezier->CurveUpdate(2, -0.3333 - bezier->GetControlPoint(0, 2).x, 0.47140452079);
	for(int i = 3; i <= (segNum - 1) * 3; i++)
		bezier->CurveUpdate(i, -bezier->GetControlPoint(i / 3, i % 3).x, 0.5);
	bezier->CurveUpdate(segNum * 3 - 2, 0.3333 - bezier->GetControlPoint(segNum - 1, 1).x, 0.47140452079);
	bezier->CurveUpdate(segNum * 3 - 1, 0.6666 - bezier->GetControlPoint(segNum - 1, 2).x, 0.37267799625);
	for(int i = 0; i < segNum; i++)
		for(int j = 0; j < 3; j++)
			addControlPoint(i, j);
	addControlPoint(segNum - 1, 3);
}

void Bezier::WhenPicked(){
	if(isPicking){ // picking extra action

	} else { // set picking
		isPicking = true;
		for(int i = 0; i < cp.size(); i++)
			if(cp[i] == pickedShape) controlPoint = i;
		if(controlPoint != -1) return;
		// check convex hull

		if(pickedShape < 3 || pickedShape >= shapes.size()){ // check if 3d area shape
			pickedShape = -1;
			isPicking = false;
		}
	}
}

void Bezier::stopPicking(){ 
	isPicking = false;
	controlPoint = -1;
}

void Bezier::WhenRotate()
{
	if(controlPoint == -1){ // 3d area

	} else { //2d area
		switch(controlPoint % 3){
			case 0:
				if(controlPoint != 0)
					AlignPrevPathControlPoint(controlPoint);
				break;
			case 1:
				// TODO rotate around controlPoint - 1
				break;
			case 2:
				// TODO rotate around controlPoint + 1
				break;
		}
	}
}

void Bezier::MoveCP(int cPoint, glm::vec4 sp, glm::vec3 delta){
	if(sp.y + delta.y < 0)
			delta.y = 0;
	shapes[cp[cPoint]]->MyTranslate(delta, 0);
	bezier->CurveUpdate(cPoint, delta.x, delta.y);
}

void Bezier::MoveControlPoint(int cPoint){
	glm::vec4 sp = shapes[cp[cPoint]]->MakeTrans()[3];
	glm::vec3 delta = glm::vec3(xrel / 200.0f, yrel / 200.0f, 0);
	MoveCP(cPoint, sp, delta);
}

void Bezier::MoveControlPointContinuity(int cPoint){
	glm::vec4 sp = shapes[cp[cPoint]]->MakeTrans()[3];
	glm::vec3 delta = glm::vec3(xrel / 200.0f, yrel / 200.0f, 0);
	glm::vec4 op = shapes[cp[cPoint + (cPoint % 3 == 1 ? -1 : 1)]]->MakeTrans()[3];
	glm::vec4 vecDirection = glm::normalize(op - sp);
	float distance = glm::dot(delta, glm::vec3(vecDirection));
	glm::vec4 np = sp + distance * vecDirection;
	delta = glm::vec3(np - sp);
	MoveCP(cPoint, sp, delta);
}

void Bezier::AlignPrevPathControlPoint(int cPoint){
	glm::vec4 p0 = shapes[cp[cPoint - 1]]->MakeTrans()[3];
	glm::vec4 p2 = shapes[cp[cPoint - 2]]->MakeTrans()[3];
	glm::vec4 lineDirection = glm::normalize(p0 - shapes[cp[cPoint]]->MakeTrans()[3]);
	float distance = glm::distance(p2, p0);
	glm::vec4 np = p0 + distance * lineDirection;
	glm::vec3 delta = glm::vec3(np - p2);
	MoveCP(cPoint - 2, p2, delta);
}

void Bezier::WhenTranslate()
{
	if(controlPoint == -1){ // 3d area
		ShapeTransformation(xTranslate, xrel / 80.0f);
		ShapeTransformation(yTranslate, yrel / 80.0f);
	} else { //2d area
		if(continuity && (controlPoint % 3 == 1 || controlPoint % 3 == 2))
			MoveControlPointContinuity(controlPoint);
		else
			MoveControlPoint(controlPoint);

		if(controlPoint % 3 == 0){
			if(controlPoint != 0) MoveControlPoint(controlPoint - 1);
			if(controlPoint != bezier->GetSegmentsNum() * 3)
				MoveControlPoint(controlPoint + 1);
		}
	}
}

void Bezier::avi(glm::mat4& a){
	v = a;
}

void Bezier::Motion()
{
	if(isActive)
	{
		pickedShape = 4;
		// shapes[0]->MyTranslate(glm::vec3(0,0,0.01), 0);
		// ShapeTransformation(yRotate, 0.07);
		pickedShape = 0;
		// shapes[0]->MyScale(glm::vec3(1.001, 1.001, 1.001));
		// shapes[2]->MyTranslate(glm::vec3(0,0,0.001), 0);
		// bezier->CurveUpdate(1, 0.00, 0.001);
		// bezier->CurveUpdate(2, 0.00, 0.001);
	}
}

unsigned int Bezier::TextureDesine(int width, int height)
{
	printMat(shapes[0]->MakeTrans());
	std::cout << "C: " << counter << "\n";
}

Bezier::~Bezier(void)
{

}
