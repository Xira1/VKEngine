#pragma once

#include <vulkan/vulkan.h>
#include <optional>
#include <vector>

//struct QueueFamilyIndex {
//	std::optional<uint32_t> graphics;
//	std::optional<uint32_t> present;
//
//	bool IsComplete() {
//		return graphics.has_value() && present.has_value();
//	}
//};
//
//struct Queues {
//	VkQueue graphics;
//	VkQueue present;
//};
//
//struct SwapChainSupportDetails {
//	VkSurfaceCapabilitiesKHR capabilities;
//	std::vector<VkSurfaceFormatKHR> formats;
//	std::vector<VkPresentModeKHR> presentModes;
//};

namespace VulkanDeviceManager {
	bool Init();

	void CleanUp();

	/*bool IsDeviceSuitable(VkPhysicalDevice device);
	bool CheckDeviceExtensionSupport(VkPhysicalDevice device);

	QueueFamilyIndex FindQueueFamilies(VkPhysicalDevice device);
	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);

	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentMods);
	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);*/

	VkPhysicalDevice GetPhysicalDevice();
	VkDevice GetDevice();

	VkQueue GetGraphicQueue();
	uint32_t GetGraphicQueueFamily();

	VkQueue GetPresentQueue();
	uint32_t GetPresentQueueFamily();
}