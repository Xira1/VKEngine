#pragma once

#include "vk_device_manager.h"
#include "vk_swapchain_manager.h"

namespace VulkanSyncManager {
	bool Init();

	void CleanUp();

	const std::vector<VkSemaphore>& GetImageSemaphore();
	const std::vector<VkSemaphore>& GetRenderSemaphore();
	const std::vector<VkFence>& GetInFlightFences();
}