#pragma once
#include "ThirdParty.h"


namespace LoggingCallbacks
{
	// Callback if GLFW wants to report an error
	void glfw_error_callback(int error, const char* description);
	// Callback if ImGui wants to report a Vulkan error
	void ImguiCheckVkResult(VkResult err);
}

// Vulkan Validation Layer Callbacks
namespace ValidationLayers
{
	// List of desired validation layers
	inline std::array<const char*, 1> DesiredLayers = {
		#ifdef __ANDROID__
		// TODO: Add android validation layers
		#else // GLFW
		"VK_LAYER_KHRONOS_validation"
		#endif
	};

	VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
	void DestroyDebugUtilsMessengerEXT(VkInstance instance, const VkAllocationCallbacks* pAllocator);
	bool CheckValidationLayerSupport();
	void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	void SetupDebugMessenger(VkInstance instance);
}

namespace MemoryTackingCallbacks
{
	// Callbacks for a VkAllocationCallbacks object
	void* vkAllocateHostMemory(void* pUserData, size_t size, size_t alignment, VkSystemAllocationScope allocationScope);
	void vkFreeHostMemory(void* pUserData, void* pMemory);
	void* vkReallocateHostMemory(void* pUserData, void* pOriginal, size_t size, size_t alignment, VkSystemAllocationScope allocationScope);
	void vkInternalAllocationNotification(void* pUserData, size_t size, VkInternalAllocationType allocationType, VkSystemAllocationScope allocationScope);
	void vkInternalFreeNotification(void* pUserData, size_t size, VkInternalAllocationType allocationType, VkSystemAllocationScope allocationScope);

}
