#include "BackEnd.h"

namespace BackEnd {
	bool Init() {
		if (!GLFWBase::Init()) return false;

		return true;
	}

	void ForceWindowClose() {
		GLFWBase::ForceWindowClose();
	}

	bool WindowIsOpen() {
		return GLFWBase::WindowIsOpen();
	}

	void ToggleFullscreen() {
		GLFWBase::ToggleFullscreen();
	}

	void SetWindowedMode(const WindowedMode& windowedMode) {
		GLFWBase::SetWindowedMode(windowedMode);
	}

	float GetCurrentWindowWidth() {
		return GLFWBase::GetCurrentWindowWidth();
	}

	float GetCurrentWindowHeight() {
		return GLFWBase::GetCurrentWindowHeight();
	}

	GLFWwindow* GetWindowPointer() {
		return GLFWBase::GetWindowPointer();
	}
}