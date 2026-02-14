#include "vk_base.h"

#include <GLFW/glfw3.h>
#include <iostream>
#include <assert.h>

#include "BackEnd.h"

#include "vk_instance_manager.h"
#include "vk_device_manager.h"
#include "vk_swapchain_manager.h"
#include "vk_pipeline_manager.h"
#include "vk_command_manager.h"
#include "vk_sync_manager.h"

#include "../../Config/Config.h"

bool VulkanBase::Init() {
	if (!VulkanInstanceManager::Init())			return false;	// Instance, dbgmsg, surface
	if (!VulkanDeviceManager::Init())			return false;   // pick physical device, create logical device
	if (!VulkanSwapchainManager::Init())		return false;   // swapchain, img views
	if (!VulkanPipelineManager::Init())			return false;   // render pass, graphic pipeline, framebuffers
	if (!VulkanCommandManager::Init())			return false;   // command pool, command buffers
	if (!VulkanSyncManager::Init())				return false;	// sync objects

	return true;
}

void VulkanBase::DrawFrame() {
	VkDevice device = VulkanDeviceManager::GetDevice();
	VkSwapchainKHR swapChain = VulkanSwapchainManager::GetSwapchain();
	VkQueue graphicsQueue = VulkanDeviceManager::GetGraphicQueue();
	VkQueue presentQueue = VulkanDeviceManager::GetPresentQueue();
	const auto& imageAvailableSemaphores = VulkanSyncManager::GetImageSemaphore();
	const auto& renderFinishedSemaphores = VulkanSyncManager::GetRenderSemaphore();
	const auto& inFlightFences = VulkanSyncManager::GetInFlightFences();
	const auto& cmdBuffers = VulkanCommandManager::GetCommandBuffers();

	vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		VulkanSwapchainManager::RecreateSwapchain();
		return;
	} 
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("Failed to acquire swap chain image!");
	}
	vkResetFences(device, 1, &inFlightFences[currentFrame]);

	vkResetCommandBuffer(cmdBuffers[currentFrame], 0);
	VulkanCommandManager::RecordCommandBuffer(cmdBuffers[currentFrame], imageIndex);

	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
	VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
	VkSwapchainKHR swapChains[] = { swapChain };

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &cmdBuffers[currentFrame];
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
		throw std::runtime_error("Failed to submit draw command buffer!\n");
	}

	VkPresentInfoKHR presentInfoKHR{};
	presentInfoKHR.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfoKHR.waitSemaphoreCount = 1;
	presentInfoKHR.pWaitSemaphores = signalSemaphores;
	presentInfoKHR.swapchainCount = 1;
	presentInfoKHR.pSwapchains = swapChains;
	presentInfoKHR.pImageIndices = &imageIndex;

	result = vkQueuePresentKHR(presentQueue, &presentInfoKHR);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
		framebufferResized = false;
		VulkanSwapchainManager::RecreateSwapchain();
	}

	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void VulkanBase::CleanUp() {
	vkDeviceWaitIdle(VulkanDeviceManager::GetDevice());

	VulkanSyncManager::CleanUp();
	VulkanCommandManager::CleanUp();
	VulkanPipelineManager::CleanUp();
	VulkanSwapchainManager::CleanUp();
	VulkanDeviceManager::CleanUp();
	VulkanInstanceManager::CleanUp();

	glfwDestroyWindow(BackEnd::GetWindowPointer());
	glfwTerminate();
}
