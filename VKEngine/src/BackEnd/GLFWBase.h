#pragma once

#include <GLFW/glfw3.h>
#include "../Config/Enums.h"

namespace GLFWBase {
	bool Init();

	void ForceWindowClose();
	bool WindowIsOpen();

	void ToggleFullscreen();
	void SetWindowedMode(const WindowedMode& windowedMode);
	
	float GetCurrentWindowWidth();
	float GetCurrentWindowHeight();

	GLFWwindow* GetWindowPointer();
}