#include <iostream>

#include "API/Vulkan/vk_base.h"
#include "BackEnd/BackEnd.h"
#include "Input/Input.h"

int main() {
	BackEnd::Init();
	VulkanBase::GetInstance()->Init();

	Input::Init(BackEnd::GetWindowPointer());

	while (BackEnd::WindowIsOpen()) {
		Input::Update();

		if (Input::KeyPressed(GLFW_KEY_ESCAPE)) {
			BackEnd::ForceWindowClose();
		}

		glfwPollEvents();
		VulkanBase::GetInstance()->DrawFrame();
	}

	VulkanBase::GetInstance()->CleanUp();

	return 0;
}