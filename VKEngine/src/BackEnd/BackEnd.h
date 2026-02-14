#pragma once

#include "GLFWBase.h"

namespace BackEnd {
	bool Init();

	void ForceWindowClose();
	bool WindowIsOpen();

	float GetCurrentWindowWidth();
	float GetCurrentWindowHeight();

	GLFWwindow* GetWindowPointer();
}