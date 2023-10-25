#pragma once
#include "ThirdParty.h"
#include "LayerContainers.h"


#ifndef GPU_MEMORY_TRACKER_H
#define GPU_MEMORY_TRACKER_H


enum GpuMemoryUsageTag
{
	GPU_USAGE_UNKNOWN,					// Default Case
	GPU_USAGE_UNIFORM_TEXEL_BUFFER,		// Small Shader Read Only Texel (1D Texture) Buffer
	GPU_USAGE_STORAGE_TEXEL_BUFFER,		// Larger/Slower Shader Read/Write Texel (1D Texture) Buffer
	GPU_USAGE_UNIFORM_BUFFER,			// Small Shader Read Only General Buffer a.k.a. UBO (Uniform Buffer Object)
	GPU_USAGE_STORAGE_BUFFER,			// Larger/Slower Shader Read/Write General Buffer a.k.a. SSBO (Shader Storage Buffer Object)
	GPU_USAGE_VERTEX_BUFFER,			// Mesh Vertices Buffer
	GPU_USAGE_INDEX_BUFFER,				// Mesh Indices Buffer
	GPU_USAGE_SAMPLED_IMAGE,			// Read Only Shader Sampled Texture (0.0~1.0 Range)
	GPU_USAGE_STORAGE_IMAGE,			// Read/Write Shader Sampled Texture (Pixel Coords i.e. (30,64))
	GPU_USAGE_ATTACHMENT_IMAGE,			// Framebuffer Image Used By Render Pass (Access to only local fragment)
	GPU_USAGE_MAX
};

// Helper struct to hold info for a GPU image allocation so we can track the memory used
struct GpuImage
{
	VkImage image = {};
	VkImageView imageView = {};
	VmaAllocation vmaAllocation = {};
	GpuMemoryUsageTag usageTag = GPU_USAGE_UNKNOWN;
};

namespace GpuMemoryTracker
{
	// Initialize GPU memory tracker and setup everything it needs
	void InitilizeGpuMemoryTracker();
	// Called anytime GPU memory is allocated so we can update the relevant tracking info.
	void AllocatedGpuMemory(GpuMemoryUsageTag tag, u64 sizeOfAlloc);
	// Called anytime GPU memory is deallocated so we can update the relevant tracking info.
	void DeallocatedGpuMemory(GpuMemoryUsageTag tag, u64 sizeOfAlloc);
	// Add the current GPU memory usage to the log file
	void LogGpuMemoryUsage();
}



#endif //GPU_MEMORY_TRACKER_H
