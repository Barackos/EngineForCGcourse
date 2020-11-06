#pragma once //maybe should be static class
#include "display.h"
#include "renderer.h"
#include "mandelbrot.h"
#include <iostream>

void mouse_callback(GLFWwindow *window, int button, int action, int mods)
{
	Renderer *rndr = (Renderer *)glfwGetWindowUserPointer(window);
	Mandelbrot *scn = (Mandelbrot *)rndr->GetScene();
	if (action == GLFW_PRESS)
	{
		double x2, y2;
		glfwGetCursorPos(window, &x2, &y2);
	}
	//rndr->ClearDrawFlag(1, 1);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
	Renderer *rndr = (Renderer *)glfwGetWindowUserPointer(window);
	Mandelbrot *scn = (Mandelbrot *)rndr->GetScene();

	//scn->MyTranslate(glm::vec3(0, 0, yoffset), 0);
	scn->updateZoom(yoffset * -0.01f);
	//rndr->MoveCamera(0, scn->zTranslate, yoffset);

}

double cursor_x = 0.0;
double cursor_y = 0.0;

void cursor_position_callback(GLFWwindow *window, double xpos, double ypos)
{
	Renderer *rndr = (Renderer *)glfwGetWindowUserPointer(window);
	Mandelbrot *scn = (Mandelbrot *)rndr->GetScene();
	rndr->UpdatePosition((float)xpos, (float)ypos);

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		float xrel = xpos - cursor_x;
		float yrel = ypos - cursor_y;

		Mandelbrot *scn = (Mandelbrot *)rndr->GetScene();
		scn->updateOffsets(xrel * -0.005f, yrel * 0.005f);
		// rndr->MoveCamera(0, scn->xTranslate, xrel * -0.01f);
		// rndr->MoveCamera(0, scn->yTranslate, yrel * 0.01f);
		//rndr->MouseProccessing(GLFW_MOUSE_BUTTON_LEFT);
	}
	cursor_x = xpos;
	cursor_y = ypos;
}

void window_size_callback(GLFWwindow *window, int width, int height)
{
	Renderer *rndr = (Renderer *)glfwGetWindowUserPointer(window);
	Mandelbrot *scn = (Mandelbrot *)rndr->GetScene();
	//scn->updateScreen(width, height);

	rndr->Resize(width, height);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	Renderer *rndr = (Renderer *)glfwGetWindowUserPointer(window);
	Mandelbrot *scn = (Mandelbrot *)rndr->GetScene();

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
			scn->increaseP();
			break;
		case GLFW_KEY_DOWN:
			//scn->shapeTransformation(scn->xGlobalRotate,-5.f);
			// rndr->MoveCamera(0, scn->yTranslate, -0.4f);
			scn->decreaseP();
			break;
		case GLFW_KEY_LEFT:
			scn->lowerColors();
			break;
		case GLFW_KEY_RIGHT:
			scn->raiseColors();
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
