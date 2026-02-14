#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace VulkanCommandManager {
	bool Init();

	void RecordCommandBuffer(VkCommandBuffer buffer, uint32_t imgIdx);

	void CleanUp();

	const std::vector<VkCommandBuffer>& GetCommandBuffers();
}