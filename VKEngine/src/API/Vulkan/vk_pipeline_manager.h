#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace VulkanPipelineManager {
	bool Init();

	void InitRenderPass();
	void InitPipeline();
	void InitFramebuffers();
	
	void CleanUp();

	VkPipeline GetPipeline();
	VkRenderPass GetRenderPass();

	VkShaderModule CreateShaderModule(const std::vector<char>& code);

	std::vector<VkFramebuffer>& GetFramebuffers();
}