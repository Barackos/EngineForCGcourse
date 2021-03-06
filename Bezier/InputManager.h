#pragma once   //maybe should be static class
#include "display.h"
#include "renderer.h"
#include "Bezier.h"
#include <iostream>

// bool up = false;

	void mouse_callback(GLFWwindow* window,int button, int action, int mods)
	{	
		Renderer* rndr = (Renderer*)glfwGetWindowUserPointer(window);
		Bezier* scn = (Bezier*)rndr->GetScene();
		if (action == GLFW_PRESS)
		{
			double x2, y2;
			glfwGetCursorPos(window, &x2, &y2);
			if (rndr->Picking((int)x2, (int)y2))
			{
				// scn->UpdatePosition(x2, y2);
				rndr->UpdatePosition(x2, y2);
			}
			//scn->ResetCounter();
		} else if (action == GLFW_RELEASE){
			scn->stopPicking();
		}
		// else
		// 	scn->SetCounter();
		//std::cout << "yes" << std::endl;
		//rndr->ClearDrawFlag(1,1);
	}
	
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
	{
		Renderer* rndr = (Renderer*)glfwGetWindowUserPointer(window);
		Bezier* scn = (Bezier*)rndr->GetScene();
		scn->scrollCB(xoffset);
	}
	
	void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
	{
		Renderer* rndr = (Renderer*)glfwGetWindowUserPointer(window);
		Bezier* scn = (Bezier*)rndr->GetScene();
		
		rndr->UpdatePosition((float)xpos,(float)ypos);
		scn->UpdatePosition((float)xpos, (float)ypos);

			if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
			{
				rndr->MouseProccessing(GLFW_MOUSE_BUTTON_RIGHT);
			}
			if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
			{
				rndr->MouseProccessing(GLFW_MOUSE_BUTTON_LEFT);
			}
	}

	void window_size_callback(GLFWwindow* window, int width, int height)
	{
		Renderer* rndr = (Renderer*)glfwGetWindowUserPointer(window);
		
		rndr->Resize(width,height);
		
	}
	
	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		Renderer* rndr = (Renderer*)glfwGetWindowUserPointer(window);
		Bezier* scn = (Bezier*)rndr->GetScene();
				glm::mat4 mat;

		if (action == GLFW_PRESS || action == GLFW_REPEAT)
		{
			switch (key)
			{
			case GLFW_KEY_ESCAPE:
				glfwSetWindowShouldClose(window, GLFW_TRUE);
				break;
			case GLFW_KEY_SPACE:
				// if (scn->IsActive())
				// 	scn->Deactivate();
				// else
				// 	scn->Activate();
				scn->createShape();
				break;
			case GLFW_KEY_UP:
				rndr->MoveCamera(0, scn->yRotate, 0.4f);
				scn->movePlane(scn->yRotate, 0.4f);
				break;
			case GLFW_KEY_DOWN:
				//scn->shapeTransformation(scn->xGlobalRotate,-5.f);
				//cout<< "down: "<<endl;
				rndr->MoveCamera(0, scn->yRotate, -0.4f);
				scn->movePlane(scn->yRotate, -0.4f);
				break;
			case GLFW_KEY_LEFT:
				rndr->MoveCamera(0, scn->xRotate, -0.4f);
				scn->movePlane(scn->xRotate, -0.4f);
				break;
			case GLFW_KEY_RIGHT:
				rndr->MoveCamera(0, scn->xRotate, 0.4f);
				scn->movePlane(scn->xRotate, 0.4f);
				break;
			case GLFW_KEY_U:
				rndr->MoveCamera(0, scn->yTranslate, 0.4f);
				scn->movePlane(scn->yTranslate, 0.4f);
				break;
			case GLFW_KEY_D:
				//scn->shapeTransformation(scn->xGlobalRotate,-5.f);
				//cout<< "down: "<<endl;
				rndr->MoveCamera(0, scn->yTranslate, -0.4f);
				scn->movePlane(scn->yTranslate, -0.4f);
				break;
			case GLFW_KEY_L:
				rndr->MoveCamera(0, scn->xTranslate, -0.4f);
				scn->movePlane(scn->xTranslate, -0.4f);
				break;
			case GLFW_KEY_R:
				rndr->MoveCamera(0, scn->xTranslate, 0.4f);
				scn->movePlane(scn->xTranslate, 0.4f);
				break;
			case GLFW_KEY_B:
				rndr->MoveCamera(0, scn->zTranslate, 0.4f);
				scn->movePlane(scn->zTranslate, 0.4f);
				break;
			case GLFW_KEY_F:
				rndr->MoveCamera(0, scn->zTranslate, -0.4f);
				scn->movePlane(scn->zTranslate, -0.4f);
				break;
			case GLFW_KEY_2:
			case GLFW_KEY_3:
			case GLFW_KEY_4:
			case GLFW_KEY_5:
			case GLFW_KEY_6:
				scn->startPosition(key - 48);
				break;
			case GLFW_KEY_C:
				scn->setContinuity();
				break;
			default:
				break;
			}
		}
	}

	void Init(Display &display)
	{
		display.AddKeyCallBack(key_callback);
		display.AddMouseCallBacks(mouse_callback,scroll_callback,cursor_position_callback);
		display.AddResizeCallBack(window_size_callback);
	}
