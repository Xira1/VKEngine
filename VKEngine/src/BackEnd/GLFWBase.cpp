#include "GLFWBase.h"

#include <iostream>

namespace GLFWBase {
	GLFWwindow* g_window;
	GLFWmonitor* g_monitor;
	const GLFWvidmode* g_mode;
	bool g_forceWindowClose = false;
	GLuint g_currentWindowWidth = 0;
	GLuint g_currentWindowHeight = 0;
	GLuint g_windowedWidth = 0;
	GLuint g_windowedHeight = 0;
	GLuint g_fullscreenWidth;
	GLuint g_fullscreenHeight;

	bool Init() {
		std::cout << "Pepechill\n";

		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		g_monitor = glfwGetPrimaryMonitor();
		g_mode = glfwGetVideoMode(g_monitor);
		glfwWindowHint(GLFW_RED_BITS, g_mode->redBits);
		glfwWindowHint(GLFW_GREEN_BITS, g_mode->greenBits);
		glfwWindowHint(GLFW_BLUE_BITS, g_mode->blueBits);
		glfwWindowHint(GLFW_REFRESH_RATE, g_mode->refreshRate);
		g_fullscreenWidth = g_mode->width;
		g_fullscreenHeight = g_mode->height;
		g_windowedWidth = g_fullscreenWidth * 0.75f;
		g_windowedHeight = g_fullscreenHeight * 0.75f;
		g_window = glfwCreateWindow(g_windowedWidth, g_windowedHeight, "Engine", NULL, NULL);
		if (g_window == NULL) {
			std::cout << "GLFW window create failed\n";
			glfwTerminate();
			return false;
		}
		glfwSetWindowPos(g_window, 0, 0);
		glfwSetInputMode(g_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		return true;
	}

	void ForceWindowClose() {
		g_forceWindowClose = true;
	}

	bool WindowIsOpen() {
		return !(glfwWindowShouldClose(g_window) || g_forceWindowClose);
	}

	float GetCurrentWindowWidth() {
		return g_currentWindowWidth;
	}

	float GetCurrentWindowHeight() {
		return g_currentWindowHeight;
	}

	GLFWwindow* GetWindowPointer() {
		return g_window;
	}
}