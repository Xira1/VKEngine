#pragma once

#include <GLFW/glfw3.h>

namespace GLFWBase {
	bool Init();

	void ForceWindowClose();
	bool WindowIsOpen();
	
	float GetCurrentWindowWidth();
	float GetCurrentWindowHeight();

	GLFWwindow* GetWindowPointer();
}