#pragma once
#include "ThirdParty.h"

// Forward Declares
struct VkRef;
struct GpuImage;
enum GpuMemoryUsageTag : u32;

namespace VkImageHelpers
{
	// Returns a VkImageView for a given VkImage based on given VkFormat and VkImageAspectFlags
	VkImageView CreateImageView(const VkRef& vkRef, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);

	// Creates a VkImage and allocates the memory for it on the GPU
	GpuImage Create2DImage(const VkRef& vkRef, VkExtent2D extent, VkFormat format, VkImageTiling tiling, VkImageUsageFlags useFlags, VkImageAspectFlags aspectFlags, VkMemoryPropertyFlags propFlags, GpuMemoryUsageTag gpuMemUsage);

	// Destroys a VkImage and deallocates the memory for it on the GPU
	void DestroyImage(const VkRef& vkRef, GpuImage& gpuImage);
}

namespace VkBufferHelpers
{

}

