#include "vk_instance_manager.h"

#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

#include "BackEnd.h"

namespace VulkanInstanceManager {
	VkInstance g_instance = VK_NULL_HANDLE;
	VkDebugUtilsMessengerEXT g_debugMessenger = VK_NULL_HANDLE;
	VkSurfaceKHR g_surface = VK_NULL_HANDLE;
	bool g_validationEnabled = true;

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData) {

		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

		return VK_FALSE;
	}

	bool Init() {
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensios = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> requiredExtensions;
		for (uint32_t i = 0; i < glfwExtensionCount; i++) {
			requiredExtensions.emplace_back(glfwExtensios[i]);
		}

		if (g_validationEnabled) {
			requiredExtensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		requiredExtensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

		std::vector<const char*> validationLayers;
		if (g_validationEnabled) {
			uint32_t layerCount;
			vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
			std::vector<VkLayerProperties> availableLayers(layerCount);
			vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

			bool found = false;
			for (const auto& layer : availableLayers) {
				if (strcmp(layer.layerName, "VK_LAYER_KHRONOS_validation") == 0) {
					validationLayers.push_back("VK_LAYER_KHRONOS_validation");
					found = true;
					break;
				}
			}

			if (!found) {
				std::cerr << "Validation layers requested but not available\n";
				g_validationEnabled = false;
			}
		}

		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Engine";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = nullptr;
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_2;

		VkDebugUtilsMessengerCreateInfoEXT debugInfo{};
		if (g_validationEnabled) {
			debugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			debugInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
			debugInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
			debugInfo.pfnUserCallback = debugCallback;
		}
		
		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
		createInfo.ppEnabledExtensionNames = requiredExtensions.data();
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
		createInfo.pNext = g_validationEnabled ? &debugInfo : nullptr;

		if (vkCreateInstance(&createInfo, nullptr, &g_instance) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create instance!\n");
			return false;
		}

		if (g_validationEnabled) {
			auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(g_instance, "vkCreateDebugUtilsMessengerEXT");
			if (func) {
				if (func(g_instance, &debugInfo, nullptr, &g_debugMessenger) != VK_SUCCESS) {
					std::cerr << "Failed to set up debug messenger!\n";
				}
			}
		}

		GLFWwindow* window = static_cast<GLFWwindow*>(BackEnd::GetWindowPointer());
		if (glfwCreateWindowSurface(g_instance, window, nullptr, &g_surface) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create window surface!\n");
			return false;
		}

		return true;
	}

	void CleanUp() {
		if (g_surface != VK_NULL_HANDLE) {
			vkDestroySurfaceKHR(g_instance, g_surface, nullptr);
			g_surface = VK_NULL_HANDLE;
		}

		if (g_debugMessenger != VK_NULL_HANDLE) {
			auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(g_instance, "vkDestroyDebugUtilsMessengerEXT");
			if (func != nullptr) {
				func(g_instance, g_debugMessenger, nullptr);
			}
			g_debugMessenger = VK_NULL_HANDLE;
		}

		if (g_instance != VK_NULL_HANDLE) {
			vkDestroyInstance(g_instance, nullptr);
			g_instance = VK_NULL_HANDLE;
		}
	}

	VkInstance GetInstance() { return g_instance; }
	VkDebugUtilsMessengerEXT GetDebugMessenger() { return g_debugMessenger; }
	VkSurfaceKHR GetSurface() { return g_surface; }
	bool ValidationEnabled() { return g_validationEnabled; }
}