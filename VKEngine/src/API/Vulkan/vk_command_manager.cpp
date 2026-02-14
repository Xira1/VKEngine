#include "vk_command_manager.h"

#include "vk_device_manager.h"
#include "vk_swapchain_manager.h"
#include "vk_pipeline_manager.h"

#include "../../Config/Config.h"
#include <iostream>

namespace VulkanCommandManager {
	VkCommandPool g_commandPool = VK_NULL_HANDLE;
	std::vector<VkCommandBuffer> g_commandBuffers;

	bool Init() {
		VkDevice device = VulkanDeviceManager::GetDevice();
		uint32_t graphicsQueueFamilyIndex = VulkanDeviceManager::GetGraphicQueueFamily();

		// Pool
		VkCommandPoolCreateInfo commandPoolCreateInfo{};
		commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		commandPoolCreateInfo.queueFamilyIndex = graphicsQueueFamilyIndex;

		if (vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &g_commandPool) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create command pool!\n");
			return false;
		}
		
		// Buffers
		g_commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo commandBufferInfo{};
		commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferInfo.commandBufferCount = (uint32_t)g_commandBuffers.size();
		commandBufferInfo.commandPool = g_commandPool;
		commandBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

		if (vkAllocateCommandBuffers(device, &commandBufferInfo, g_commandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("Failed to allocate command buffers!\n");
			return false;
		}

		return true;
	}

	void RecordCommandBuffer(VkCommandBuffer buffer, uint32_t imgIdx) {
		VkExtent2D swapChainExtent = VulkanSwapchainManager::GetExtent();
		std::vector<VkFramebuffer> swapChainFramebuffers = VulkanPipelineManager::GetFramebuffers();

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		vkBeginCommandBuffer(buffer, &beginInfo);

		VkClearValue clearValue = { { 0.0f, 0.0f, 0.0f, 1.0f } };

		VkRenderPassBeginInfo renderPassBeginInfo{};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = VulkanPipelineManager::GetRenderPass();
		renderPassBeginInfo.framebuffer = swapChainFramebuffers[imgIdx];
		renderPassBeginInfo.renderArea.offset = { 0,0 };
		renderPassBeginInfo.renderArea.extent = swapChainExtent;
		renderPassBeginInfo.clearValueCount = 1;
		renderPassBeginInfo.pClearValues = &clearValue;

		vkCmdBeginRenderPass(buffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, VulkanPipelineManager::GetPipeline());

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)swapChainExtent.width;
		viewport.height = (float)swapChainExtent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		vkCmdSetViewport(buffer, 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0,0 };
		scissor.extent = swapChainExtent;
		vkCmdSetScissor(buffer, 0, 1, &scissor);

		vkCmdDraw(buffer, 3, 1, 0, 0);
		vkCmdEndRenderPass(buffer);
		vkEndCommandBuffer(buffer);
	}

	void CleanUp() {
		if (g_commandPool != VK_NULL_HANDLE) {
			vkDestroyCommandPool(VulkanDeviceManager::GetDevice(), g_commandPool, nullptr);
			g_commandPool = VK_NULL_HANDLE;
		}
	}

	const std::vector<VkCommandBuffer>& GetCommandBuffers() { return g_commandBuffers; }
}