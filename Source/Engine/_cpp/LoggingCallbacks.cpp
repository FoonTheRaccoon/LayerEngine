#include "LoggingCallbacks.h"
#include "Logger.h"
#include "MemoryTracker.h"
#include "LayerMemory.h"
#include "GlobalConstants.h"
#include "vk_enum_string_helper.h"


void LoggingCallbacks::glfw_error_callback(int error, const char* description)
{
	LOG_ERROR_MIN(T_string("GLFW Error! Error Code: ", std::to_string(error), " | Error Description: ", description));
}

void LoggingCallbacks::ImguiCheckVkResult(VkResult err)
{
	if (err != VK_SUCCESS) [[unlikely]]
		{
			LOG_ERROR_MIN(T_string("Imgui Error: VkResult == ", string_VkResult(err)));
		}
}

namespace ValidationLayers
{
	VkDebugUtilsMessengerEXT _VkDebugMessenger = {};
	T_string _VkMessageBuffer = {};
}

VKAPI_ATTR VkBool32 VKAPI_CALL ValidationLayers::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
	_VkMessageBuffer.clear();
	_VkMessageBuffer.AppendMany("Validation Layers: ", pCallbackData->pMessage);
	if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
	{
		LOG_ERROR_MIN(_VkMessageBuffer.c_str());
	}
	else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
	{
		LOG_WARNING_MIN(_VkMessageBuffer.c_str());
	}
	else
	{
		LOG_INFO(_VkMessageBuffer.c_str());
	}

	return VK_FALSE;
}

VkResult ValidationLayers::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr)
	{
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else
	{
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}

}

void ValidationLayers::DestroyDebugUtilsMessengerEXT(VkInstance instance, const VkAllocationCallbacks* pAllocator)
{
	if (!GlobalConstants::bEnableValidationLayers) return;

	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr)
	{
		func(instance, _VkDebugMessenger, pAllocator);
	}
}

bool ValidationLayers::CheckValidationLayerSupport()
{
	u32 layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	T_vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : DesiredLayers) {
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}
		if (!layerFound) { return false; }
	}
	return true;
}

void ValidationLayers::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = DebugCallback;
}

void ValidationLayers::SetupDebugMessenger(VkInstance instance)
{
	if (!GlobalConstants::bEnableValidationLayers) return;

	LOG_DEBUG("Setting Up Vulkan Validation Layers Callback...");

	VkDebugUtilsMessengerCreateInfoEXT createInfo;
	PopulateDebugMessengerCreateInfo(createInfo);

	LOG_VKRESULT(CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &_VkDebugMessenger));
	LOG_INFO("Setup Vulkan Validation Layers Callback");
}



namespace MemoryTackingCallbacks
{
	// Hash map used for tracking the various allocations VkHostMemory functions use and remembers each allocation size
	T_unordered_map<void*, size_t, MT_GRAPHICS> _vkHostMemoryAllocationTracker = {};
}

// Callback for a VkAllocationCallbacks object
void* MemoryTackingCallbacks::vkAllocateHostMemory(void* pUserData, size_t size, size_t alignment, VkSystemAllocationScope allocationScope)
{
	if (size == 0) { return nullptr; }

	void* ptr = LayerMemory::AlignedMalloc(size, alignment);
	_vkHostMemoryAllocationTracker.emplace(ptr, size);
	MemoryTracker::AllocatedHostMemory(MT_VULKAN, size);

	// LOG_DEBUG(T_string("Vk Allocating ", std::to_string(size), " bytes!"));

	return ptr;
}

void MemoryTackingCallbacks::vkFreeHostMemory(void* pUserData, void* pMemory)
{
	if (pMemory)
	{
		const size_t originalSize = _vkHostMemoryAllocationTracker.at(pMemory);
		MemoryTracker::DeallocatedHostMemory(MT_VULKAN, originalSize);
		_vkHostMemoryAllocationTracker.erase(pMemory);

		// LOG_DEBUG(T_string("Vk Freeing ", std::to_string(originalSize), " bytes!"));

		LayerMemory::AlignedFree(pMemory);
	}
}

void* MemoryTackingCallbacks::vkReallocateHostMemory(void* pUserData, void* pOriginal, size_t size, size_t alignment, VkSystemAllocationScope allocationScope)
{
	if (!pOriginal) { return vkAllocateHostMemory(pUserData, size, alignment, allocationScope); }
	if (size == 0)
	{
		vkFreeHostMemory(pUserData, pOriginal);
		return nullptr;
	}

	size_t originalSize = _vkHostMemoryAllocationTracker.at(pOriginal);
	size_t copySize = std::min(originalSize, size);
	void* pNewMemory = vkAllocateHostMemory(pUserData, size, alignment, allocationScope);
	if (pNewMemory != nullptr)
	{
		// Copy the contents from the original allocation to the new allocation
		std::memcpy(pNewMemory, pOriginal, copySize);

		// Free the original allocation
		vkFreeHostMemory(pUserData, pOriginal);
	}

	return pNewMemory;
}

void MemoryTackingCallbacks::vkInternalAllocationNotification(void* pUserData, size_t size, VkInternalAllocationType allocationType, VkSystemAllocationScope allocationScope)
{
	MemoryTracker::AllocatedHostMemory(MT_VULKAN_INTERNAL, size);
}

void MemoryTackingCallbacks::vkInternalFreeNotification(void* pUserData, size_t size, VkInternalAllocationType allocationType, VkSystemAllocationScope allocationScope)
{
	MemoryTracker::DeallocatedHostMemory(MT_VULKAN_INTERNAL, size);
}


