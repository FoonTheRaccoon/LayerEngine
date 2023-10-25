#pragma once
#include "EngUtils.h"


#ifndef VK_HELPER_H
#define VK_HELPER_H


namespace VkSetup
{
	// Array of available physical devices and their properties. Gets populated during VkSetup::CapturePhysicalDevice
	inline T_vector<PhysicalDevice, MT_GRAPHICS> availablePhysicalDevices = {};

	void CreateInstance(const char* appName, VkRef& vkRef);
	void CreateSurface(VkRef& vkRef);
	void CapturePhysicalDevice(VkRef& vkRef);
	void CreateLogicalDevice(VkRef& vkRef);
	void CreateVmaAllocator(VkRef& vkRef);
	void CreateCommandPools(VkRef& vkRef);
	void AllocateCommandBuffers(VkRef& vkRef);
}

#endif // VK_HELPER_H

