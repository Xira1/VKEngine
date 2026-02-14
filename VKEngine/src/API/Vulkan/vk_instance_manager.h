#pragma once

#include <vulkan/vulkan.h>

namespace VulkanInstanceManager {
	bool Init();

	void CleanUp();

	VkInstance GetInstance();
	VkDebugUtilsMessengerEXT GetDebugMessenger();
	VkSurfaceKHR GetSurface();
	bool ValidationEnabled();
}