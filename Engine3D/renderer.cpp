#include "renderer.h"
#include "GL/glew.h"
#include <array>
#include <iostream>


Renderer::Renderer()
{
	cameras.push_back(new Camera(60.0f, 1.0, 0.1f, 100.0f));

	xold = 0;
	yold = 0;
	
}

Renderer::Renderer(float angle, float relationWH, float near, float far)
{
	glLineWidth(5);
	cameras.push_back(new Camera(angle, relationWH, near, far));

	xold = 0;
	yold = 0;

}

void Renderer::Init(Scene* scene,  std::list<int>xViewport,  std::list<int>yViewport)
{
	scn = scene;
	// MoveCamera(0, zTranslate, 10);
	glm::ivec4 viewport;
	glGetIntegerv(GL_VIEWPORT, &viewport[0]);
	vpx = viewport[2];
	vpy = viewport[3];
	drawInfo.push_back(new DrawInfo(0, 0, 0, 0,   inAction | toClear | blackClear | depthTest));
	drawInfo.push_back(new DrawInfo(1, 1, 0, 0,   inAction | toClear | blackClear | depthTest));
	buffers.push_back(new DrawBuffer());

	if (xViewport.empty() && yViewport.empty())
	{
		viewports.push_back(viewport);
		drawInfo.push_back(new DrawInfo(0, 0, 1, 0,  toClear | blackClear | depthTest));
	}
	else
	{
		xViewport.pop_back();
		xViewport.push_back(viewport.z / 2);
		xViewport.push_front(viewport.x);
		yViewport.push_front(viewport.y);
		xViewport.push_back(viewport.z);
		yViewport.push_back(viewport.w);
		std::list<int>::iterator xit = xViewport.begin();
		int indx = 0;
		for (++xit; xit != xViewport.end(); ++xit)
		{
			std::list<int>::iterator yit = yViewport.begin();
			for (++yit; yit != yViewport.end(); ++yit)
			{
				viewports.push_back(glm::ivec4(*std::prev(xit), *std::prev(yit), *xit - *std::prev(xit), *yit - *std::prev(yit)));
				drawInfo.push_back(new DrawInfo(indx, 0, 1, 0, (indx < 1) | blackClear | depthTest | scissorTest | stencilTest | blend));
				indx++;
			}
		}
	}

}

void Renderer::Draw(int infoIndx)
{
	DrawInfo info = *drawInfo[infoIndx];


	buffers[info.buffer]->Bind();
	glViewport(viewports[info.viewportIndx].x, viewports[info.viewportIndx].y, viewports[info.viewportIndx].z, viewports[info.viewportIndx].w);
	if (info.flags & scissorTest)
		glEnable(GL_SCISSOR_TEST);
	else
		glDisable(GL_SCISSOR_TEST);

	if (info.flags & stencilTest)
		glEnable(GL_STENCIL_TEST);
	else
		glDisable(GL_STENCIL_TEST);

	if (info.flags & depthTest)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);

	if (info.flags & blend)
		glEnable(GL_BLEND);
	else
		glDisable(GL_BLEND);

	glm::mat4 MVP = cameras[info.cameraIndx]->GetViewProjection() * glm::inverse(cameras[info.cameraIndx]->MakeTrans());

	if (info.flags & toClear)
	{
		if (info.flags & blackClear)
			Clear(0, 0, 0, 0);
		else
			Clear(1, 1, 1, 1);
	}
	scn->Draw(info.shaderIndx, MVP, info.viewportIndx, cameras[info.cameraIndx], debugMode);

}

void Renderer::DrawAll()
{
	for (int i = 0; i < drawInfo.size(); i++) {
		if (!(drawInfo[i]->flags & inAction))
			Draw(i);
	}
}

bool Renderer::Picking(int x, int y)
{
	//picking from camera 0 and using shader 0
	// ActionDraw();
	for (int i = 0; i < drawInfo.size(); i++)
	{
		if (drawInfo[i]->flags & inAction){
			Draw(i);
			GLint viewport[4];
			glGetIntegerv(GL_VIEWPORT, viewport); //reading viewport parameters
			int xPos = x * (vpx / 840.0);
			int yPos = y * (vpy / 840.0);
			int bs = i == 0 ? 1 : 40, cp = 0, size = 4 * bs * bs;
			unsigned char *data = new unsigned char[size];
			glReadPixels(xPos, viewport[3] - yPos, bs, bs, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glReadPixels(xPos, viewport[3] - yPos, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
			for (int j = 0; j < size; j+=4){
				if((data + j)[3] > 3){ cp = j; break; }
			}
			bool p = scn->Picking(data + cp);
			delete[] data;
			if(p) return true;
		}
	}
	return false;
	//return depth;

}

void Renderer::MouseProccessing(int button)
{
	scn->MouseProccessing(button, xrel, yrel);
	// cameras[0]->MyRotate(xrel / 2.0f, glm::vec3(0, 1, 0), 0);
	// cameras[0]->MyRotate(-yrel / 2.0f, glm::vec3(1, 0, 0), 1);
}

void Renderer::AddCamera(const glm::vec3& pos, float fov, float relationWH, float zNear, float zFar, int infoIndx)
{
	if (infoIndx >= 0 && infoIndx < drawInfo.size())
		drawInfo[infoIndx]->SetCamera(cameras.size());
	cameras.push_back(new Camera(fov, relationWH, zNear, zFar));
	cameras.back()->MyTranslate(pos, 0);
}

void Renderer::AddViewport(int left, int bottom, int width, int height)
{
	viewports.push_back(glm::ivec4(left, bottom, width, height));
	glViewport(left, bottom, width, height);

}

unsigned int Renderer::AddBuffer(int infoIndx, bool stencil)
{
	CopyDraw(infoIndx, buffers.size());

	DrawInfo* info = drawInfo.back();
	info->SetFlags(stencilTest | toClear | blackClear);
	//info->ClearFlags(depthTest);
	int width = viewports[info->viewportIndx].z, height = viewports[info->viewportIndx].w;

	unsigned int texId;

	texId = scn->AddTexture(width, height, 0, COLOR);
	if (stencil)
		scn->AddTexture(width, height, 0, STENCIL);
	else
		scn->AddTexture(width, height, 0, DEPTH);
	//scn->BindTexture(texId, info->buffer - 1);
	buffers.push_back(new DrawBuffer(width, height, stencil, texId + 1));

	return texId;
}

int Renderer::Create2Dmaterial(int texsNum)
{
	std::vector<unsigned int> texIds;
	std::vector<unsigned int> slots;
	for (size_t i = 0; i < texsNum; i++)
	{
		unsigned int texId = AddBuffer(1, true);
		texIds.push_back(texId);
		slots.push_back(i);
	}

	materialIndx2D = scn->AddMaterial((unsigned int*)& texIds[0], (unsigned int*)& slots[0], texsNum);
	return materialIndx2D;
}

void Renderer::AddDraw(int view, int camera, int shader, int buff, unsigned int flags)
{
	drawInfo.push_back(new DrawInfo(view, camera, shader, buff, flags));
}

void Renderer::CopyDraw(int infoIndx, int buff)
{
	DrawInfo* info = drawInfo[infoIndx];
	drawInfo.push_back(new DrawInfo(info->viewportIndx, info->cameraIndx, info->shaderIndx, buff, info->flags));
}

void Renderer::SetViewport(int left, int bottom, int width, int height, int indx)
{
	viewports[indx] = glm::ivec4(left, bottom, width, height);
	glViewport(left, bottom, width, height);
}

void Renderer::UpdatePosition(float xpos, float ypos)
{
	xrel = xold - xpos;
	yrel = yold - ypos;
	xold = xpos;
	yold = ypos;
}

void Renderer::Resize(int width, int height)
{
	//not working properly
	cameras[0]->SetProjection(cameras[0]->GetAngle(), (float)width / height);
	//glViewport(0, 0, width, height);
	viewports[0].z = width;
	viewports[0].w = height;
	//std::cout << cameras[0]->GetRelationWH() << std::endl;
}

void Renderer::MoveCamera(int cameraIndx, int type, float amt)
{
	switch (type)
	{
	case xTranslate:
		cameras[cameraIndx]->MyTranslate(glm::vec3(amt, 0, 0), 0);
		break;
	case yTranslate:
		cameras[cameraIndx]->MyTranslate(glm::vec3(0, amt, 0), 0);
		break;
	case zTranslate:
		cameras[cameraIndx]->MyTranslate(glm::vec3(0, 0, amt), 0);
		break;
	case xRotate:
		cameras[cameraIndx]->MyRotate(amt, glm::vec3(1, 0, 0), 0);
		break;
	case yRotate:
		cameras[cameraIndx]->MyRotate(amt, glm::vec3(0, 1, 0), 0);
		break;
	case zRotate:
		cameras[cameraIndx]->MyRotate(amt, glm::vec3(0, 0, 1), 0);
		break;
	default:
		break;
	}
}

bool Renderer::checkViewport(int x, int y, int viewportIndx)
{
	return (viewports[viewportIndx].x < x&& viewports[viewportIndx].y < y&& viewports[viewportIndx][2] + viewports[viewportIndx][0] > x&& viewports[viewportIndx][3] + viewports[viewportIndx][1] > y);
}

Renderer::~Renderer()
{
	for (Camera* cam : cameras)
	{
		delete cam;
	}

	for (DrawInfo* info : drawInfo)
	{
		delete info;
	}

	
}

void Renderer::Clear(float r, float g, float b, float a)
{
	glClearColor(r, g, b, a);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void Renderer::ActionDraw()
{
	for (int i = 0; i < drawInfo.size(); i++)
	{
		if (drawInfo[i]->flags & inAction)
			Draw(i);
	}

}


void Renderer::SetScene(Scene* scene)
{
	scn = scene;
}




