#include "vk_device_manager.h"

#include <GLFW/glfw3.h>

#include "BackEnd.h"

#include "vk_instance_manager.h"

#include <iostream>
#include <set>

namespace VulkanDeviceManager {
	VkPhysicalDevice g_physicalDevice = VK_NULL_HANDLE;
	VkDevice g_device = VK_NULL_HANDLE;

	VkQueue g_graphicsQueue = VK_NULL_HANDLE;
	uint32_t g_graphicsQueueFamily = UINT32_MAX;

	VkQueue g_presentQueue = VK_NULL_HANDLE;
	uint32_t g_presentQueueFamily = UINT32_MAX;

	bool Init() {
		VkInstance instance = VulkanInstanceManager::GetInstance();
		VkSurfaceKHR surface = VulkanInstanceManager::GetSurface();

		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

		g_physicalDevice = devices.data()[0];

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(g_physicalDevice, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(g_physicalDevice, &queueFamilyCount, queueFamilies.data());

		g_graphicsQueueFamily = 0;
		g_presentQueueFamily = 0;

		for (size_t i = 0; i < queueFamilies.size(); i++)
		{
			VkQueueFamilyProperties queueFamily = queueFamilies[i];

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(g_physicalDevice, i, surface, &presentSupport);

			if (presentSupport) {
				g_presentQueueFamily = i;
			}

			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				g_graphicsQueueFamily = i;
				break;
			}
		}

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = {
			g_graphicsQueueFamily,
			g_presentQueueFamily
		};
		float queuePriority = 1.0f;

		for (const uint32_t queueFamily : uniqueQueueFamilies)
		{
			VkDeviceQueueCreateInfo queueCreateInfo = {};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.queueFamilyIndex = g_graphicsQueueFamily;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		VkPhysicalDeviceFeatures deviceFeautres = {};
		VkDeviceCreateInfo deviceCreateInfo = {};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

		const std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
		deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
		deviceCreateInfo.pEnabledFeatures = &deviceFeautres;

		deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();

		if (vkCreateDevice(g_physicalDevice, &deviceCreateInfo, nullptr, &g_device) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create logical device!\n");
			return false;
		}

		vkGetDeviceQueue(g_device, g_graphicsQueueFamily, 0, &g_graphicsQueue);
		vkGetDeviceQueue(g_device, g_presentQueueFamily, 0, &g_presentQueue);

		return true;
	}

	void CleanUp() {
		if (g_device != VK_NULL_HANDLE) {
			vkDestroyDevice(g_device, nullptr);
			g_device = VK_NULL_HANDLE;
			g_physicalDevice = VK_NULL_HANDLE;

			g_graphicsQueue = VK_NULL_HANDLE;
			g_presentQueue = VK_NULL_HANDLE;
			g_graphicsQueueFamily = UINT32_MAX;
			g_presentQueueFamily = UINT32_MAX;
		}
	}

	/*bool IsDeviceSuitable(VkPhysicalDevice device) {
		QueueFamilyIndex indices = FindQueueFamilies(device);

		bool extensionSupported = CheckDeviceExtensionSupport(device);

		bool swapChainAdequate = false;
		if (extensionSupported) {
			SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device);
			swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
		}

		return indices.IsComplete() && extensionSupported && swapChainAdequate;
	}*/

	/*bool CheckDeviceExtensionSupport(VkPhysicalDevice device) {
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

		for (const auto& ext : availableExtensions) {
			requiredExtensions.erase(ext.extensionName);
		}

		return requiredExtensions.empty();
	}*/

	/*QueueFamilyIndex FindQueueFamilies(VkPhysicalDevice device) {
		QueueFamilyIndex indices;

		uint32_t qCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(g_physicalDevice, &qCount, nullptr);

		std::vector<VkQueueFamilyProperties> qFamilies(qCount);
		vkGetPhysicalDeviceQueueFamilyProperties(g_physicalDevice, &qCount, qFamilies.data());

		int i = 0;
		for (const auto& qFamily : qFamilies) {
			if (indices.IsComplete()) {
				break;
			}

			if (qFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				indices.graphics = i;
			}
			i++;
		}

		return indices;
	}

	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device) {
		SwapChainSupportDetails details;
		VkSurfaceKHR surface = VulkanInstanceManager::GetSurface();

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

		if (formatCount != 0) {
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

		if (formatCount != 0) {
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
		}

		return details;
	}

	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
		for (const auto& availableFormat : availableFormats) {
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				return availableFormat;
			}
		}

		return availableFormats[0];
	}

	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentMods) {
		for (const auto& availablePresentMode : availablePresentMods) {
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				return availablePresentMode;
			}
		}

		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
		if (capabilities.currentExtent.width != UINT32_MAX) {
			return capabilities.currentExtent;
		}
		else {
			int width, height;
			glfwGetFramebufferSize(BackEnd::GetWindowPointer(), &width, &height);

			VkExtent2D actualExtent = {
				static_cast<uint32_t>(width),
				static_cast<uint32_t>(height),
			};

			actualExtent.width = std::max(capabilities.currentExtent.width, std::min(capabilities.currentExtent.height, actualExtent.width));
			actualExtent.width = std::max(capabilities.currentExtent.height, std::min(capabilities.currentExtent.width, actualExtent.height));

			return actualExtent;
		}
	}*/

	VkPhysicalDevice GetPhysicalDevice() { return g_physicalDevice; }
	VkDevice GetDevice() { return g_device; }

	VkQueue GetGraphicQueue() { return g_graphicsQueue; }
	uint32_t GetGraphicQueueFamily() { return g_graphicsQueueFamily; }

	VkQueue GetPresentQueue() { return g_presentQueue; }
	uint32_t GetPresentQueueFamily() { return g_presentQueueFamily; }
}