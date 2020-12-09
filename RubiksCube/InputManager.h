#pragma once //maybe should be static class
#include "display.h"
#include "renderer.h"
#include "RubiksCube.h"
#include <random>

void mouse_callback(GLFWwindow *window, int button, int action, int mods) {
	if (action == GLFW_PRESS) {
		Renderer *rndr = (Renderer *)glfwGetWindowUserPointer(window);
		RubiksCube *scn = (RubiksCube *)rndr->GetScene();
		double x2, y2;
		glfwGetCursorPos(window, &x2, &y2);
		if (rndr->Picking((int)x2, (int)y2))
			rndr->UpdatePosition(x2, y2);
	}
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
	Renderer *rndr = (Renderer *)glfwGetWindowUserPointer(window);
	RubiksCube *scn = (RubiksCube *)rndr->GetScene();
	scn->MyTranslate(glm::vec3(0, 0, xoffset), 0);
}

void cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {
	Renderer *rndr = (Renderer *)glfwGetWindowUserPointer(window);
	RubiksCube *scn = (RubiksCube *)rndr->GetScene();

	rndr->UpdatePosition((float)xpos, (float)ypos);

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
		rndr->MouseProccessing(GLFW_MOUSE_BUTTON_RIGHT);
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		rndr->MouseProccessing(GLFW_MOUSE_BUTTON_LEFT);
	}
}

void window_size_callback(GLFWwindow *window, int width, int height) {
	Renderer *rndr = (Renderer *)glfwGetWindowUserPointer(window);
	rndr->Resize(width, height);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
	Renderer *rndr = (Renderer *)glfwGetWindowUserPointer(window);
	RubiksCube *scn = (RubiksCube *)rndr->GetScene();

	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		switch (key) {
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, GLFW_TRUE);
			break;
		case GLFW_KEY_SPACE:
			scn->addTask(RubiksCube::tasks::SWITCH_DIRECTION);
			break;
		case GLFW_KEY_UP:
			rndr->MoveCamera(0, scn->zTranslate, 0.4f);
			break;
		case GLFW_KEY_DOWN:
			//scn->shapeTransformation(scn->xGlobalRotate,-5.f);
			//cout<< "down: "<<endl;
			rndr->MoveCamera(0, scn->zTranslate, -0.4f);
			break;
		case GLFW_KEY_L:
			scn->addTask(RubiksCube::tasks::ROTATE_LEFT);
			break;
		case GLFW_KEY_R:
			scn->addTask(RubiksCube::tasks::ROTATE_RIGHT);
			break;
		case GLFW_KEY_U:
			scn->addTask(RubiksCube::tasks::ROTATE_UP);
			break;
		case GLFW_KEY_D:
			scn->addTask(RubiksCube::tasks::ROTATE_BOTTOM);
			break;
		case GLFW_KEY_F:
			scn->addTask(RubiksCube::tasks::ROTATE_FRONT);
			break;
		case GLFW_KEY_B:
			scn->addTask(RubiksCube::tasks::ROTATE_BACK);
			break;
		case GLFW_KEY_Z:
			scn->addTask(RubiksCube::tasks::ROTATE_FASTER);
			break;
		case GLFW_KEY_A:
			scn->addTask(RubiksCube::tasks::ROTATE_SLOWER);
			break;
		case GLFW_KEY_M:
			for (int i = 0; i < 10; i++)
				scn->addTask(std::rand() % 5);
			break;

		default:
			break;
		}
	}
}

void Init(Display &display) {
	display.AddKeyCallBack(key_callback);
	display.AddMouseCallBacks(mouse_callback, scroll_callback, cursor_position_callback);
	display.AddResizeCallBack(window_size_callback);
}
