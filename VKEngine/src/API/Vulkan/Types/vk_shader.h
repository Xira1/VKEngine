#pragma once

#include <vulkan/vulkan.h>

#include <string>
#include <vector>

// TODO: take it from GLSandBox
struct VulkanShaderModule {
	VulkanShaderModule() = default;
    VulkanShaderModule(VkDevice device, const std::string filename, VkShaderStageFlagBits stage);
    /*VulkanShaderModule(const VulkanShaderModule&) = delete;
    VulkanShaderModule& operator=(const VulkanShaderModule&) = delete;
    VulkanShaderModule(VulkanShaderModule&& other) noexcept;
    VulkanShaderModule& operator=(VulkanShaderModule&& other) noexcept;*/

private:
    std::string m_path;
    VkShaderModule m_module = VK_NULL_HANDLE;
    VkShaderStageFlagBits m_stages = VK_SHADER_STAGE_ALL;
};

struct VulkanShader {

};