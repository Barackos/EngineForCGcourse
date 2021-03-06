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

void Bezier::AddNewShape(Shape *s, int parent){
	Shape *plane = shapes.back();
	shapes.pop_back();
	chainParents.pop_back();
	chainParents.push_back(parent);
	chainParents.push_back(-1); // plane parent
	shapes.push_back(s);
	shapes.push_back(plane);
}

Bezier::Bezier() : Scene()
{
	bezier = 0;
	counter = 10;
	v = glm::mat4(1);
	cp = std::vector<int>();
	continuity = false;
	r0 = glm::vec2(-1, 0);
	r1 = glm::vec2(0, 0);
	rShapes = std::vector<int>();
	fillRshapes = false;
}

//Bezier::Bezier(float angle ,float relationWH, float near, float far) : Scene(angle,relationWH,near,far)
//{ 	
//}

void Bezier::Init()
{		
	unsigned int texIDs[3] = { 0 , 1, 2};
	unsigned int slots[3] = { 0 , 1, 2 };
	AddShader("../res/shaders/pickingShader");	
	AddShader("../res/shaders/basicShader");
	AddShader("../res/shaders/basicShader2");
	AddShader("../res/shaders/cubemapShader");
	AddShader("../res/shaders/outlineShader");
	AddTexture("../res/textures/box0.bmp", 2);
	AddTexture("../res/textures/grass.bmp", 2);

	AddShape(Cube, -1, TRIANGLES);
	SetShapeShader(0, 3);
	shapes[0]->MyScale(glm::vec3(4,4,4));
	std::vector<std::string> faces{ "Daylight Box_Right.bmp", "Daylight Box_Left.bmp", "Daylight Box_Top.bmp", "Daylight Box_Bottom.bmp", "Daylight Box_Front.bmp", "Daylight Box_Back.bmp" };
	textures.push_back(new Texture(faces));
	AddMaterial(texIDs,slots, 3);

	AddShape(Axis, -1, LINES);
	shapes[1]->AddViewport(1);
	shapes[1]->RemoveViewport(0);
	shapes.push_back(bezier);

	AddShape(Plane, -1, TRIANGLES);
	SetShapeShader(shapes.size() - 1, 4);
	shapes[shapes.size() - 1]->MyTranslate(glm::vec3(0,0,-1.0f), 0);
	startPosition(3); // uses AddNewShape so need to be called after the plane is added
}

void Bezier::BeforeDraw(int ref){
	if(pickedShape == shapes.size() - 1){
		if(pickingState == CREATING_RECTANGLE){
			glm::vec2 wvct = glm::vec2(vpx / 840.0f, vpy / 840.0f);
			glm::vec2 start = wvct * 840.0f * glm::vec2(glm::min(r0.x, r1.x), glm::min(r0.y, r1.y));
			glm::vec2 end = wvct * 840.0f * glm::vec2(glm::max(r0.x, r1.x), glm::max(r0.y, r1.y));
			glScissorIndexed(0, start.x, start.y, end.x - start.x, end.y - start.y);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		} else
			glScissorIndexed(0, 0, 0, 0, 0);
	} else if(std::find(rShapes.begin(), rShapes.end(), pickedShape) != rShapes.end()){
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glStencilFunc(GL_ALWAYS, ref, 0xFF); 
		glStencilMask(0xFF); 
	}
}

void Bezier::AfterDraw(const glm::mat4& MVP){
	if(pickedShape == shapes.size() - 1){
		glScissorIndexed(0, 0, 0, vpx, vpy);
		glBlendFunc(GL_ONE, GL_ZERO);
	} else if(std::find(rShapes.begin(), rShapes.end(), pickedShape) != rShapes.end()){
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00); 
		shapes[pickedShape]->MyScale(glm::vec3(1.06,1.06,1.06));
		Update(MVP, MakeTrans() * shapes[pickedShape]->MakeTrans(), 4);
		shapes[pickedShape]->Draw(shaders[4], false);
		shapes[pickedShape]->MyScale(glm::vec3(1/1.06,1/1.06,1/1.06));
	}
}

void Bezier::Draw(int shaderIndx, const glm::mat4& MVP, int viewportIndx, Camera *c, unsigned int flags){
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	for(int s : rShapes){
		pickedShape = s;
		BeforeDraw(0);
	}
	// glClearStencil(0);
	// glClear(GL_STENCIL_BUFFER_BIT);
	Scene::Draw(shaderIndx, MVP, viewportIndx, c, flags);
	if(fillRshapes){
		SelectShapesByRectangle();
		fillRshapes = false;
	}
}

void Bezier::Update(const glm::mat4 &MVP,const glm::mat4 &Model,const int  shaderIndx)
{	
	Shader *s = shaders[shaderIndx];
	int r = (pickedShape+1) & 0xFF;
	if (shapes[pickedShape]->GetMaterial() >= 0 && !materials.empty())
		BindMaterial(s, shapes[pickedShape]->GetMaterial());
	s->Bind();
	s->SetUniformMat4f("View", MVP);
	s->SetUniformMat4f("Model", shaderIndx == 3 ? glm::mat4(glm::mat3(Model)) : Model);
	s->SetUniform1i("sampler", materials[shapes[pickedShape]->GetMaterial()]->GetSlot(pickedShape == 2 ? 1: 0));
	s->SetUniform1i("skybox", materials[shapes[pickedShape]->GetMaterial()]->GetSlot(2));
	s->SetUniformMat4f("Obj", shapes[pickedShape]->MakeTrans());
	s->SetUniform4f("lightColor", r, 1, 1, r / 255.0f);
	s->Unbind();
}

void Bezier::UpdatePosition(float xpos,  float ypos)
{
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	x = xpos / viewport[2]; // 840 as window
	y = 1 - ypos / 840.0f;
}

void Bezier::addControlPoint(int i, int j){
	AddNewShape(new Shape(Octahedron, TRIANGLES), -1);
	int cpShape = shapes.size() - 2; // since plane is the last
	cp.push_back(cpShape);
	glm::vec4 point = bezier->GetControlPoint(i, j);
	shapes[cpShape]->MyTranslate(glm::vec3(point.x,point.y,0), 0);
	shapes[cpShape]->MyScale(glm::vec3(0.05,0.05,0.05));
	shapes[cpShape]->RemoveViewport(0);
	shapes[cpShape]->AddViewport(1);
}


void Bezier::redrawControlPoints(){
	std::vector<Shape*> cpShapes = std::vector<Shape*>();
	for(int j = 0; j < cp.size(); j++) {
		Shape *shape = shapes[cp[j]];
		cpShapes.push_back(shape);
	}
	for (int i = 0; i < cpShapes.size(); i++) {
		shapes.erase(std::remove(shapes.begin(), shapes.end(), cpShapes[i]), shapes.end());
	}
	cp.clear();
	for(int i = 0; i < bezier->GetSegmentsNum(); i++)
		for(int j = 0; j < 3; j++)
			addControlPoint(i, j);
	addControlPoint(bezier->GetSegmentsNum() - 1, 3);
}

void Bezier::startPosition(int segNum){
	if(bezier) delete bezier;;
	bezier = new Bezier1D(segNum, 50, LINE_STRIP, 1);
	shapes[2] = bezier;
	// bezier->CurveUpdate(0, -0.9 - bezier->GetControlPoint(0, 0).x, 0);
	// bezier->CurveUpdate(1, -0.6 - bezier->GetControlPoint(0, 1).x, 0.2);
	// bezier->CurveUpdate(2, -0.6 - bezier->GetControlPoint(0, 2).x, 0.7);
	// bezier->CurveUpdate(3, -0.2 - bezier->GetControlPoint(0, 3).x, 0.2);
	// bezier->CurveUpdate(4, 0.2 - bezier->GetControlPoint(1, 1).x, 0.7);
	// bezier->CurveUpdate(5, 0.4 - bezier->GetControlPoint(1, 2).x, 0.3);
	// bezier->CurveUpdate(6, 0.9 - bezier->GetControlPoint(1, 3).x, 0.71);
	// bezier->CurveUpdate(7, 0.91 - bezier->GetControlPoint(2, 1).x, 0.61);
	// bezier->CurveUpdate(8, 0.88 - bezier->GetControlPoint(2, 2).x, 0.4);
	// bezier->CurveUpdate(9, 0.9 - bezier->GetControlPoint(2, 3).x, 0);
	bezier->CurveUpdate(1, -0.6666 - bezier->GetControlPoint(0, 1).x, 0.37267799625);
	bezier->CurveUpdate(2, -0.3333 - bezier->GetControlPoint(0, 2).x, 0.47140452079);
	for(int i = 3; i <= (segNum - 1) * 3; i++)
		bezier->CurveUpdate(i, -bezier->GetControlPoint(i / 3, i % 3).x, 0.5);
	bezier->CurveUpdate(segNum * 3 - 2, 0.3333 - bezier->GetControlPoint(segNum - 1, 1).x, 0.47140452079);
	bezier->CurveUpdate(segNum * 3 - 1, 0.6666 - bezier->GetControlPoint(segNum - 1, 2).x, 0.37267799625);
	redrawControlPoints();
}

void Bezier::WhenPicked(){
	if(x < 0.5){ // 3d area
		if(pickedShape != -1){
			if(rShapes.empty() || std::find(rShapes.begin(), rShapes.end(), pickedShape) != rShapes.end())
				pickingState = PICKING_SHAPES;
		} else {
			if(rShapes.empty()){ // start creating rectangle
				r0.x = x;
				r0.y = y;
				r1.x = r0.x;
				r1.y = r0.y;
				pickingState = CREATING_RECTANGLE;
			} else { // clear rectangle
				rShapes.clear();
			}
		}
	} else { // 2d area
		std::vector<int>::iterator i = std::find(cp.begin(), cp.end(), pickedShape);
		if(i != cp.end()){ // looking for control point
			controlPoint = i - cp.begin();
			pickingState = PICKING_CONTROL_POINT;
		} else { // looking for segment
			for(int i = 0; i < bezier->GetSegmentsNum(); i++)
				if(bezier->isInConvexHull(i, glm::vec2(x * 4 - 3, y * 2 - 1))) {
					segment = i;
					pickingState = PICKING_SEGMENT;
				}
		}
	}
}

void Bezier::checkRectangleEdge(int x, int y, int size, bool isX, int vpy){
	unsigned char *data = new unsigned char[size * 4];
	glReadPixels(x, vpy - y, isX ? size : 1, isX ? 1: size, GL_RGBA, GL_UNSIGNED_BYTE, data);
	for (int j = 0; j < size * 4; j += 4)
		if(std::find(rShapes.begin(), rShapes.end(), (int)(data + j)[3] - 1) != rShapes.end())
			rShapes.erase(std::remove(rShapes.begin(), rShapes.end(), (int)(data + j)[3] - 1), rShapes.end());
	delete[] data;
}

void Bezier::SelectShapesByRectangle() {
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glm::vec2 wvct = glm::vec2(vpx / 840.0f, vpy / 840.0f);
	glm::vec2 start = wvct * 840.0f * glm::vec2(glm::min(r0.x, r1.x), glm::min(r0.y, r1.y));
	glm::vec2 end = wvct * 840.0f * glm::vec2(glm::max(r0.x, r1.x), glm::max(r0.y, r1.y));
	int sizeX = end.x - start.x, sizeY = end.y - start.y;
	// std::cout << "POSITION: " << start.x << ", " << start.y << "\n";
	// std::cout << "END: " << end.x << ", " << end.y << "\n";
	// std::cout << "SIZE: " << sizeX << ", " << sizeY << "\n";
	if(sizeX <= 0) return;
	unsigned char *data = new unsigned char[sizeX * 4];
	for(int i = 0; i < sizeY; i++){
		glReadPixels(start.x, viewport[3] - start.y - i, sizeX, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
		for (int j = 0; j < sizeX * 4; j += 4){
			if((data + j)[3] > 2 && (data + j)[3] < shapes.size()
				&& std::find(rShapes.begin(), rShapes.end(), (int)(data + j)[3] - 1) == rShapes.end()){
				// std::cout << "[" << (int)(data + j)[0] << ", " << (int)(data + j)[1] << ", " << (int)(data + j)[2] << ", " << (int)(data + j)[3] << "]\n"; 
				rShapes.push_back((data + j)[3] - 1);
			}
		}
	}
	delete[] data;

	checkRectangleEdge(start.x, start.y - 1, sizeX, true, viewport[3]);
	checkRectangleEdge(start.x, end.y + 1, sizeX, true, viewport[3]);
	checkRectangleEdge(start.x - 1, start.y, sizeY, false, viewport[3]);
	checkRectangleEdge(end.x + 1, start.y, sizeY, false, viewport[3]);
}

void Bezier::updateRectangle(){
	r1.x = x < 0.5 ? x : 0.5;
	r1.y = y;
}

void Bezier::stopPicking(){ 
	if(pickingState == CREATING_RECTANGLE && r1 != r0)
		fillRshapes = true;
	pickingState = NO_PICKING;
}

void Bezier::RotateObj(int obj, int axisPoint, int dir){
	glm::vec4 cpt = shapes[axisPoint]->MakeTrans()[3];
	shapes[obj]->MyTranslate(-glm::vec3(cpt), 0);
	shapes[obj]->MyRotate(dir * yrel / 2.0f, glm::vec3(1,0,0), 1);
	shapes[obj]->MyRotate(dir * xrel / 2.0f, glm::vec3(0,1,0), 1);
	shapes[obj]->MyTranslate(glm::vec3(cpt), 0);
}

void Bezier::RotateCP(int cPoint, int axisPoint, int dir) {
	glm::vec4 cpt = shapes[cp[axisPoint]]->MakeTrans()[3];
	shapes[cp[cPoint]]->MyTranslate(-glm::vec3(cpt), 0);
	shapes[cp[cPoint]]->MyRotate(dir * yrel / 2.0f, glm::vec3(0,0,1), 1);
	shapes[cp[cPoint]]->MyTranslate(glm::vec3(cpt), 0);
}

void Bezier::RotateControlPoint(int cPoint, int centerPoint){
	glm::vec4 delta = glm::vec4(shapes[cp[cPoint]]->MakeTrans()[3]);
	RotateCP(cPoint, centerPoint, -1); // around control point
	glm::vec2 pd = glm::vec2(delta);
	delta = shapes[cp[cPoint]]->MakeTrans()[3] - delta;
	if(pd.y + delta.y < 0 || pd.y + delta.y > 1 || pd.x + delta.x < -1 || pd.x + delta.x > 1)
		RotateCP(cPoint, centerPoint, 1);
	else{
		bezier->CurveUpdate(cPoint, delta.x, delta.y);
		RotateCP(cPoint, cPoint, -1); // around itself
	}
}

void Bezier::AlignPrevPathControlPoint(int cPoint){
	glm::vec4 p0 = shapes[cp[cPoint]]->MakeTrans()[3];
	glm::vec4 p2 = shapes[cp[cPoint - 1]]->MakeTrans()[3];
	glm::vec4 p1 = shapes[cp[cPoint + 1]]->MakeTrans()[3];
	if(p0 == p1) p1.x += 1;
	glm::vec4 lineDirection = glm::normalize(p0 - p1);
	float distance = glm::distance(p2, p0);
	glm::vec4 np = p0 + distance * lineDirection;
	glm::vec3 delta = glm::vec3(np - p2);
	MoveCP(cPoint - 1, p2, delta);
}

void Bezier::WhenRotate()
{
	if(x < 0.5){
		switch (pickingState){
		case PICKING_SHAPES:
			if(rShapes.empty())
				RotateObj(pickedShape, pickedShape, -1);
			else
				for(int s : rShapes)
					RotateObj(s, s, -1);
			break;
		case CREATING_RECTANGLE:
			updateRectangle();
			break;
		}
	} else {
		switch (pickingState){
		case PICKING_CONTROL_POINT:
			switch(controlPoint % 3){
				case 0:
					if(controlPoint != 0)
						AlignPrevPathControlPoint(controlPoint);
					break;
				case 1:
					RotateControlPoint(controlPoint, controlPoint - 1);
					if(continuity && controlPoint / 3 != 0)
						RotateControlPoint(controlPoint - 2, controlPoint - 1);
					break;
				case 2:
					RotateControlPoint(controlPoint, controlPoint + 1);
					if(continuity && controlPoint / 3 < bezier->GetSegmentsNum() - 1)
						RotateControlPoint(controlPoint + 2, controlPoint + 1);
					break;
			}
			break;
		case PICKING_SEGMENT:
			if(bezier->GetSegmentsNum() < 6){
				bezier->SplitSegment(segment, 0.5);
				redrawControlPoints();
			}
			stopPicking();
			break;
		}
	}
}

void Bezier::MoveCP(int cPoint, glm::vec4 sp, glm::vec3 delta){
	if(sp.y + delta.y < 0 || sp.y + delta.y > 1)
			delta.y = 0;
	if(sp.x + delta.x < -1 || sp.x + delta.x > 1)
			delta.x = 0;
	shapes[cp[cPoint]]->MyTranslate(delta, 0);
	bezier->CurveUpdate(cPoint, delta.x, delta.y);
}

void Bezier::MoveControlPoint(int cPoint){
	glm::vec4 sp = shapes[cp[cPoint]]->MakeTrans()[3]; // bezier->GetControlPoint(i, j);
	glm::vec3 delta = glm::vec3(xrel / 200.0f, yrel / 200.0f, 0);
	MoveCP(cPoint, sp, delta);
}

void Bezier::MoveControlPointContinuity(int cPoint){
	glm::vec4 sp = shapes[cp[cPoint]]->MakeTrans()[3];
	glm::vec3 delta = glm::vec3(xrel / 200.0f, yrel / 200.0f, 0);
	glm::vec4 op = shapes[cp[cPoint + (cPoint % 3 == 1 ? -1 : 1)]]->MakeTrans()[3];
	if(op == sp) return;
	glm::vec4 vecDirection = glm::normalize(op - sp);
	float distance = glm::dot(delta, glm::vec3(vecDirection));
	glm::vec4 np = sp + distance * vecDirection;
	delta = glm::vec3(np - sp);
	MoveCP(cPoint, sp, delta);
}

void Bezier::WhenTranslate()
{
	if(x < 0.5){
		switch (pickingState){
		case PICKING_SHAPES:
			if(rShapes.empty())
				shapes[pickedShape]->MyTranslate(glm::vec3(xrel / 80.0f, yrel / 80.0f, 0), 0);
			else
				for(int s : rShapes)
					shapes[s]->MyTranslate(glm::vec3(xrel / 80.0f, yrel / 80.0f, 0), 0);
			break;
		case CREATING_RECTANGLE:
			updateRectangle();
			break;
		}
	} else {
		switch (pickingState){
		case PICKING_CONTROL_POINT:
			if(continuity && (controlPoint % 3 == 1 || controlPoint % 3 == 2))
				MoveControlPointContinuity(controlPoint);
			else
				MoveControlPoint(controlPoint);

			if(controlPoint % 3 == 0){
				if(controlPoint != 0) MoveControlPoint(controlPoint - 1);
				if(controlPoint != bezier->GetSegmentsNum() * 3)
					MoveControlPoint(controlPoint + 1);
			}
			break;
		case PICKING_SEGMENT:
			if(segment > 0)
				MoveControlPointContinuity(segment * 3 - 1);
			if(segment < bezier->GetSegmentsNum() - 1)
				MoveControlPointContinuity(segment * 3 + 4);
			for(int i = 0; i < 4; i++)
				MoveControlPoint(segment * 3 + i);
			break;
		}
	}
}

void Bezier::scrollCB(float amt){
	if(x < 0.5){
		if(pickingState == PICKING_SHAPES){
			if(rShapes.empty())
				shapes[pickedShape]->MyTranslate(glm::vec3(0, 0, amt), 0);
			else
				for(int s : rShapes)
					shapes[s]->MyTranslate(glm::vec3(0, 0, amt), 0);
		}
	} else {
		xrel = 0;
		yrel = amt;
		int i;
		for(i = 0; i < bezier->GetSegmentsNum(); i++)
			for(int j = 0; j < 3; j++)
				MoveControlPoint(i * 3 + j);
		MoveControlPoint(i * 3);
	}
}

void Bezier::movePlane(int type, float amt){
	pickedShape = shapes.size() - 1;
	ShapeTransformation(type, amt);
}

void Bezier::createShape(){
	Bezier2D *s = new Bezier2D(bezier, 4, 50, 50, TRIANGLES);
	s->SetShader(2);
	s->MyTranslate(glm::vec3(0,0,-10.0f), 0);
	AddNewShape(s, -1);
}

void Bezier::Motion()
{
	if(isActive)
	{
	
	}
}

Bezier::~Bezier(void)
{

}
