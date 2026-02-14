#include "vk_swapchain_manager.h"

#include "vk_device_manager.h"
#include "vk_instance_manager.h"

#include "BackEnd.h"

#include <algorithm>
#include <iostream>

namespace VulkanSwapchainManager {
	VkSwapchainKHR g_swapchain = VK_NULL_HANDLE;
	VkExtent2D g_extent;
	VkFormat g_swapchainImageFormat;
	std::vector<VkImage> g_swapchainImages;
	std::vector<VkImageView> g_swapchainImageViews;

	bool Init() {
		CreateSwapchain();
		return true;
	}

	void RecreateSwapchain() {
		vkDeviceWaitIdle(VulkanDeviceManager::GetDevice());

		// Clean up old resources
		CleanUp();

		// Recreate
		CreateSwapchain();
		std::cout << "VulkanSwapchainManager::RecreateSwapchain()\n";
	}

	void CreateSwapchain() {
		VkDevice device = VulkanDeviceManager::GetDevice();
		VkPhysicalDevice physicalDevice = VulkanDeviceManager::GetPhysicalDevice();
		VkSurfaceKHR surface = VulkanInstanceManager::GetSurface();

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);
		std::vector<VkPresentModeKHR> presentModes(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes.data());

		VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
		for (VkPresentModeKHR avalibleMode : presentModes)
		{
			if (avalibleMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				presentMode = avalibleMode;
				break;
			}
		}

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
		std::vector<VkSurfaceFormatKHR> swapChainFormats(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &presentModeCount, swapChainFormats.data());

		VkSurfaceFormatKHR surfaceFormat = swapChainFormats.data()[0];
		for (VkSurfaceFormatKHR avalibleFormat : swapChainFormats)
		{
			if (avalibleFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR && avalibleFormat.format == VK_FORMAT_B8G8R8A8_SRGB) {
				surfaceFormat = avalibleFormat;
				break;
			}
		}

		VkSurfaceCapabilitiesKHR capabilities;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities);
		VkExtent2D extent = capabilities.currentExtent;
		uint32_t imageCount = capabilities.minImageCount + 1;


		VkSwapchainCreateInfoKHR swapChainCreateInfo{};
		swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapChainCreateInfo.surface = surface;
		swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		swapChainCreateInfo.clipped = VK_TRUE;
		swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;
		swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		swapChainCreateInfo.imageArrayLayers = 1;

		uint32_t queueFamiliyIndices[] = { VulkanDeviceManager::GetGraphicQueueFamily(), VulkanDeviceManager::GetPresentQueueFamily()};
		if (VulkanDeviceManager::GetGraphicQueueFamily() != VulkanDeviceManager::GetPresentQueueFamily()) {

			swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			swapChainCreateInfo.queueFamilyIndexCount = 2;
			swapChainCreateInfo.pQueueFamilyIndices = queueFamiliyIndices;
		}
		else {
			swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		}

		swapChainCreateInfo.presentMode = presentMode;
		swapChainCreateInfo.imageExtent = extent;
		swapChainCreateInfo.minImageCount = imageCount;
		swapChainCreateInfo.preTransform = capabilities.currentTransform;
		swapChainCreateInfo.imageFormat = surfaceFormat.format;
		swapChainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;

		vkCreateSwapchainKHR(device, &swapChainCreateInfo, nullptr, &g_swapchain);

		uint32_t swapChainImageCount;
		vkGetSwapchainImagesKHR(device, g_swapchain, &swapChainImageCount, nullptr);
		g_swapchainImages.resize(swapChainImageCount);
		vkGetSwapchainImagesKHR(device, g_swapchain, &swapChainImageCount, g_swapchainImages.data());

		g_swapchainImageFormat = surfaceFormat.format;
		g_extent = extent;

		g_swapchainImageViews.resize(g_swapchainImages.size());

		for (size_t i = 0; i < g_swapchainImages.size(); i++)
		{
			VkImageViewCreateInfo imageViewCreateInfo{};
			imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			imageViewCreateInfo.image = g_swapchainImages[i];
			imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			imageViewCreateInfo.format = g_swapchainImageFormat;
			imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
			imageViewCreateInfo.subresourceRange.levelCount = 1;
			imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
			imageViewCreateInfo.subresourceRange.layerCount = 1;
			vkCreateImageView(device, &imageViewCreateInfo, nullptr, &g_swapchainImageViews[i]);
		}
	}

	void CleanUp() {
		VkDevice device = VulkanDeviceManager::GetDevice();

		for (VkImageView imageViews : g_swapchainImageViews) {
			vkDestroyImageView(device, imageViews, nullptr);
		}
		vkDestroySwapchainKHR(device, g_swapchain, nullptr);
	}

	VkSwapchainKHR GetSwapchain() { return g_swapchain; }
	VkExtent2D GetExtent() { return g_extent; }
	VkFormat GetSwapchainFormat() { return g_swapchainImageFormat; }
	const std::vector<VkImage>& GetSwapchainImages() { return g_swapchainImages; }
	const std::vector<VkImageView>& GetSwapchainImageViews() { return g_swapchainImageViews; }
}