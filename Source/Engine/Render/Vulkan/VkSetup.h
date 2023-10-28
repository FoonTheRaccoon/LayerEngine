#pragma once

// Forward Declares
struct VkRef;

namespace VkSetup
{
	void CreateInstance(const char* appName, VkRef& vkRef);
	void CreateSurface(VkRef& vkRef);
	void CapturePhysicalDevice(VkRef& vkRef);
	void CreateLogicalDevice(VkRef& vkRef);
	void CreateVmaAllocator(VkRef& vkRef);
	void CreateCommandPools(VkRef& vkRef);
	void AllocateCommandBuffers(VkRef& vkRef);
}


