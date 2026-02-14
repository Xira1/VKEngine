#pragma once

#include "GLFWBase.h"

namespace BackEnd {
	bool Init();

	void ForceWindowClose();
	bool WindowIsOpen();

	void ToggleFullscreen();
	void SetWindowedMode(const WindowedMode& windowedMode);

	float GetCurrentWindowWidth();
	float GetCurrentWindowHeight();

	GLFWwindow* GetWindowPointer();
}