#pragma once //maybe should be static class
#include "display.h"
#include "renderer.h"
#include "raytracing.h"
#include <iostream>

void mouse_callback(GLFWwindow *window, int button, int action, int mods)
{
	Renderer *rndr = (Renderer *)glfwGetWindowUserPointer(window);
	Raytracing *scn = (Raytracing *)rndr->GetScene();
	if (action == GLFW_PRESS)
	{
		double x2, y2;
		glfwGetCursorPos(window, &x2, &y2);
	}
	//rndr->ClearDrawFlag(1, 1);
}

double cursor_x = 0.0;
double cursor_y = 0.0;
float zoom = 0.0;
int picked = -1;
bool pressed = false;

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
	Renderer *rndr = (Renderer *)glfwGetWindowUserPointer(window);
	Raytracing *scn = (Raytracing *)rndr->GetScene();
	scn->moveEye(0, 0, 0.015 * yoffset);
	zoom += 0.015 * yoffset;
}

void cursor_position_callback(GLFWwindow *window, double xpos, double ypos)
{
	Renderer *rndr = (Renderer *)glfwGetWindowUserPointer(window);
	Raytracing *scn = (Raytracing *)rndr->GetScene();
	rndr->UpdatePosition((float)xpos, (float)ypos);
	float xrel = xpos - cursor_x;
	float yrel = ypos - cursor_y;
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS){
		float xs = (xpos / 420.0) - 1;
		float ys = (ypos / 420.0) - 1;
		if(!pressed && picked == -1)
			picked = scn->pickSphere(xs, -ys);
		scn->moveSphere(picked, xrel, -yrel);
		if(!pressed) pressed = true;
	} else {
		//if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		//	scn->moveEye(-0.015 * xrel, 0.015 * yrel, 0);
		picked = -1; // stop moving object once not clicking
		pressed = false;
	}	
	cursor_x = xpos;
	cursor_y = ypos;
}

void window_size_callback(GLFWwindow *window, int width, int height)
{
	Renderer *rndr = (Renderer *)glfwGetWindowUserPointer(window);
	Raytracing *scn = (Raytracing *)rndr->GetScene();
	//scn->updateScreen(width, height);

	rndr->Resize(width, height);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	Renderer *rndr = (Renderer *)glfwGetWindowUserPointer(window);
	Raytracing *scn = (Raytracing *)rndr->GetScene();

	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		switch (key)
		{
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, GLFW_TRUE);
			break;
		case GLFW_KEY_SPACE:
			if (scn->IsActive())
				scn->Deactivate();
			else
				scn->Activate();
			break;

		case GLFW_KEY_UP:
			// rndr->MoveCamera(0, scn->yTranslate, 0.4f);
			break;
		case GLFW_KEY_DOWN:
			//scn->shapeTransformation(scn->xGlobalRotate,-5.f);
			// rndr->MoveCamera(0, scn->yTranslate, -0.4f);
			break;
		case GLFW_KEY_LEFT:
			//scn->lowerColors();
			break;
		case GLFW_KEY_RIGHT:
			//scn->raiseColors();
			break;

		default:
			break;
		}
	}
}

void Init(Display &display)
{
	display.AddKeyCallBack(key_callback);
	display.AddMouseCallBacks(mouse_callback, scroll_callback, cursor_position_callback);
	display.AddResizeCallBack(window_size_callback);
}
