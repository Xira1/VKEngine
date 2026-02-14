#include "vk_sync_manager.h"

#include "../../Config/Config.h"

#include <iostream>
#include <vector>

namespace VulkanSyncManager {
	std::vector<VkSemaphore> g_imageAvailableSemaphores;
	std::vector<VkSemaphore> g_renderFinishedSemaphores;
	std::vector<VkFence> g_inFlightFences;

	bool Init() {
		VkDevice device = VulkanDeviceManager::GetDevice();

		g_imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		g_renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		g_inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

		VkSemaphoreCreateInfo semaphoreCreateInfo{};
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceCreateInfo{};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			if (vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &g_imageAvailableSemaphores[i]) != VK_SUCCESS) {
				throw std::runtime_error("Failed to create semaphore!\n");
			}
			if (vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &g_renderFinishedSemaphores[i]) != VK_SUCCESS) {
				throw std::runtime_error("Failed to create semaphore!\n");
			}
			if (vkCreateFence(device, &fenceCreateInfo, nullptr, &g_inFlightFences[i]) != VK_SUCCESS) {
				throw std::runtime_error("Failed to create fence!\n");
			}
		}

		return true;
	}

	void CleanUp() {
		VkDevice device = VulkanDeviceManager::GetDevice();

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			vkDestroySemaphore(device, g_imageAvailableSemaphores[i], nullptr);
			vkDestroySemaphore(device, g_renderFinishedSemaphores[i], nullptr);
			vkDestroyFence(device, g_inFlightFences[i], nullptr);
		}
	}

	const std::vector<VkSemaphore>& GetImageSemaphore() { return g_imageAvailableSemaphores; }
	const std::vector<VkSemaphore>& GetRenderSemaphore() { return g_renderFinishedSemaphores; }
	const std::vector<VkFence>& GetInFlightFences() { return g_inFlightFences; }
}