#include "vk_pipeline_manager.h"

#include <string>
#include <fstream>

#include "vk_device_manager.h"
#include "vk_swapchain_manager.h"

static std::vector<char> ReadFile(const std::string& filename) {
	std::ifstream file(filename, std::ios::binary | std::ios::ate);

	if (!file.is_open()) {
		throw std::runtime_error("Failed to open file!\n");
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;
}

namespace VulkanPipelineManager {
	VkRenderPass g_renderPass = VK_NULL_HANDLE;
	VkPipeline g_pipeline = VK_NULL_HANDLE;
	VkPipelineLayout g_pipelineLayout = VK_NULL_HANDLE;
	std::vector<VkFramebuffer> g_framebuffers;

	// TODO: make this shit pretty
	bool Init() {
		InitRenderPass();
		InitPipeline();
		InitFramebuffers();
		return true;
	}

	void InitRenderPass() {
		VkDevice device = VulkanDeviceManager::GetDevice();
		VkFormat swapChainImageFormat = VulkanSwapchainManager::GetSwapchainFormat();

		VkAttachmentDescription colorAttachmentDescription{};
		colorAttachmentDescription.format = swapChainImageFormat;
		colorAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentReference{};
		colorAttachmentReference.attachment = 0;
		colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpassDescription{};
		subpassDescription.colorAttachmentCount = 1;
		subpassDescription.pColorAttachments = &colorAttachmentReference;
		subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

		VkSubpassDependency subpassDependency{};
		subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		subpassDependency.dstSubpass = 0;
		subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		subpassDependency.srcAccessMask = 0;
		subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo renderPassCreateInfo{};
		renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassCreateInfo.attachmentCount = 1;
		renderPassCreateInfo.pAttachments = &colorAttachmentDescription;
		renderPassCreateInfo.subpassCount = 1;
		renderPassCreateInfo.pSubpasses = &subpassDescription;
		renderPassCreateInfo.dependencyCount = 1;
		renderPassCreateInfo.pDependencies = &subpassDependency;

		if (vkCreateRenderPass(device, &renderPassCreateInfo, nullptr, &g_renderPass) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create render pass!\n");
		}
	}

	void InitPipeline() {
		VkDevice device = VulkanDeviceManager::GetDevice();

		std::vector<char> vertShaderCode = ReadFile("runtime_data/shaders/baseVert.spv");
		std::vector<char> fragShaderCode = ReadFile("runtime_data/shaders/baseFrag.spv");
		VkShaderModule vertShaderModule = CreateShaderModule(vertShaderCode);
		VkShaderModule fragShaderModule = CreateShaderModule(fragShaderCode);

		VkPipelineShaderStageCreateInfo vertPipelineShaderStageCreateInfo{};
		vertPipelineShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertPipelineShaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertPipelineShaderStageCreateInfo.module = vertShaderModule;
		vertPipelineShaderStageCreateInfo.pName = "main";

		VkPipelineShaderStageCreateInfo fragPipelineShaderStageCreateInfo{};
		fragPipelineShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragPipelineShaderStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragPipelineShaderStageCreateInfo.module = fragShaderModule;
		fragPipelineShaderStageCreateInfo.pName = "main";

		VkPipelineShaderStageCreateInfo shaderStages[] = { vertPipelineShaderStageCreateInfo, fragPipelineShaderStageCreateInfo };

		VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo{};
		pipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		pipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = 0;
		pipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = 0;

		VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo{};
		pipelineInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		pipelineInputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		pipelineInputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

		VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo{};
		pipelineViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		pipelineViewportStateCreateInfo.viewportCount = 1;
		pipelineViewportStateCreateInfo.scissorCount = 1;

		VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo{};
		pipelineRasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		pipelineRasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
		pipelineRasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
		pipelineRasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
		pipelineRasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
		pipelineRasterizationStateCreateInfo.lineWidth = 1.0f;
		pipelineRasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
		pipelineRasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;

		VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo{};
		pipelineMultisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		pipelineMultisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		pipelineMultisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;

		VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentState{};
		pipelineColorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		pipelineColorBlendAttachmentState.blendEnable = VK_FALSE;

		VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo{};
		pipelineColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		pipelineColorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
		pipelineColorBlendStateCreateInfo.attachmentCount = 1;
		pipelineColorBlendStateCreateInfo.pAttachments = &pipelineColorBlendAttachmentState;

		std::vector<VkDynamicState> dynamicStates = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR,
		};

		VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo{};
		pipelineDynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		pipelineDynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		pipelineDynamicStateCreateInfo.pDynamicStates = dynamicStates.data();

		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
		pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCreateInfo.setLayoutCount = 0;
		pipelineLayoutCreateInfo.pushConstantRangeCount = 0;

		if (vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &g_pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create pipeline layout!\n");
		}

		VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo{};
		graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		graphicsPipelineCreateInfo.stageCount = 2;
		graphicsPipelineCreateInfo.pStages = shaderStages;
		graphicsPipelineCreateInfo.pVertexInputState = &pipelineVertexInputStateCreateInfo;
		graphicsPipelineCreateInfo.pInputAssemblyState = &pipelineInputAssemblyStateCreateInfo;
		graphicsPipelineCreateInfo.pViewportState = &pipelineViewportStateCreateInfo;
		graphicsPipelineCreateInfo.pRasterizationState = &pipelineRasterizationStateCreateInfo;
		graphicsPipelineCreateInfo.pMultisampleState = &pipelineMultisampleStateCreateInfo;
		graphicsPipelineCreateInfo.pColorBlendState = &pipelineColorBlendStateCreateInfo;
		graphicsPipelineCreateInfo.pDynamicState = &pipelineDynamicStateCreateInfo;
		graphicsPipelineCreateInfo.layout = g_pipelineLayout;
		graphicsPipelineCreateInfo.renderPass = g_renderPass;
		graphicsPipelineCreateInfo.subpass = 0;
		graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;

		if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, nullptr, &g_pipeline) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create graphic pipeline!\n");
		}

		vkDestroyShaderModule(device, vertShaderModule, nullptr);
		vkDestroyShaderModule(device, fragShaderModule, nullptr);
	}

	void InitFramebuffers() {
		VkDevice device = VulkanDeviceManager::GetDevice();
		VkExtent2D swapChainExtent = VulkanSwapchainManager::GetExtent();
		const auto& swapChainImageViews = VulkanSwapchainManager::GetSwapchainImageViews();

		g_framebuffers.resize(swapChainImageViews.size());
		for (size_t i = 0; i < swapChainImageViews.size(); i++) {
			VkImageView attachments[] = { swapChainImageViews[i] };
			VkFramebufferCreateInfo framebufferCreateInfo{};
			framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferCreateInfo.renderPass = g_renderPass;
			framebufferCreateInfo.attachmentCount = 1;
			framebufferCreateInfo.pAttachments = attachments;
			framebufferCreateInfo.width = swapChainExtent.width;
			framebufferCreateInfo.height = swapChainExtent.height;
			framebufferCreateInfo.layers = 1;

			if (vkCreateFramebuffer(device, &framebufferCreateInfo, nullptr, &g_framebuffers[i]) != VK_SUCCESS) {
				throw std::runtime_error("Failed to create frame buffer!\n");
			}
		}
	}

	void CleanUp() {
		// TODO: 
		for (VkFramebuffer framebuffer : g_framebuffers) {
			vkDestroyFramebuffer(VulkanDeviceManager::GetDevice(), framebuffer, nullptr);
		}

		if (g_pipeline != VK_NULL_HANDLE) {
			vkDestroyPipeline(VulkanDeviceManager::GetDevice(), g_pipeline, nullptr);
			g_pipeline = VK_NULL_HANDLE;
		}

		if (g_pipelineLayout != VK_NULL_HANDLE) {
			vkDestroyPipelineLayout(VulkanDeviceManager::GetDevice(), g_pipelineLayout, nullptr);
			g_pipelineLayout = VK_NULL_HANDLE;
		}

		if (g_renderPass != VK_NULL_HANDLE) {
			vkDestroyRenderPass(VulkanDeviceManager::GetDevice(), g_renderPass, nullptr);
			g_renderPass = VK_NULL_HANDLE;
		}
	}

	VkShaderModule CreateShaderModule(const std::vector<char>& code) {
		VkShaderModule shaderModule;

		VkShaderModuleCreateInfo smInfo{};

		smInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		smInfo.codeSize = code.size();
		smInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		if (vkCreateShaderModule(VulkanDeviceManager::GetDevice(), &smInfo, nullptr, &shaderModule) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create shader module!\n");
		}

		return shaderModule;
	}

	VkPipeline GetPipeline() { return g_pipeline; }
	VkRenderPass GetRenderPass() { return g_renderPass; }
	std::vector<VkFramebuffer>& GetFramebuffers() { return g_framebuffers; }
}