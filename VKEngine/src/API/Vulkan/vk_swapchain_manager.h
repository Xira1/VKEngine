#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace VulkanSwapchainManager {
	bool Init();

	void RecreateSwapchain();
	void CreateSwapchain();

	void CleanUp();

	VkSwapchainKHR GetSwapchain();
	VkExtent2D GetExtent();
	VkFormat GetSwapchainFormat();
	const std::vector<VkImage>& GetSwapchainImages();
	const std::vector<VkImageView>& GetSwapchainImageViews();
}