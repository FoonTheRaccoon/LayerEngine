#include "VkBuffersAndImages.h"
#include "VkTypes.h"
#include "Logger.h"
#include "GpuMemoryTracker.h"

VkImageView VkImageHelpers::CreateImageView(const VkRef& vkRef, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
{
	VkImageViewCreateInfo imageViewCreateInfo = {};
	imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageViewCreateInfo.image = image;										// Image to create view for.
	imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;					// Type of image (1D, 2D, 3D, Array, Cube, etc)
	imageViewCreateInfo.format = format;									// Format of the image (e.g. VK_FORMAT_R8G8B8A8_UNORM or other)

	imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;		// Allows remapping of rgba components to other RGBA values or 1/0 
	imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

	// Subresources allow the image view to view only a part of an image.
	imageViewCreateInfo.subresourceRange.aspectMask = aspectFlags;			// Which aspect of image to view (e.g. COLOR_BIT for viewing color)
	imageViewCreateInfo.subresourceRange.baseMipLevel = 0;					// Start mipmap level to view from
	imageViewCreateInfo.subresourceRange.levelCount = 1;					// Number of mipmap levels to view
	imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;				// Start of array level to view from
	imageViewCreateInfo.subresourceRange.layerCount = 1;					// Number of array levels to view

	// Create image view and return it.
	VkImageView imageView;
	LOG_VKRESULT(vkCreateImageView(vkRef.logDevice, &imageViewCreateInfo, &vkRef.hostAllocator, &imageView));

	return imageView;
}

GpuImage VkImageHelpers::Create2DImage(const VkRef& vkRef, VkExtent2D extent, VkFormat format, VkImageTiling tiling, VkImageUsageFlags useFlags, VkImageAspectFlags aspectFlags, VkMemoryPropertyFlags propFlags, GpuMemoryUsageTag gpuMemUsage)
{
	GpuImage gpuImage = {};
	gpuImage.usageTag = gpuMemUsage;

	// Image create info
	VkImageCreateInfo imageCreateInfo = {};
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;					// Type of image (1D, 2D, or 3D)
	imageCreateInfo.format = format;								// Format of image (VK_FORMAT_R8G8B8A8_UNORM/etc)
	imageCreateInfo.extent.width = extent.width;					// Width of image extent
	imageCreateInfo.extent.height = extent.height;					// Height of image extent
	imageCreateInfo.extent.depth = 1;								// Depth of image (just 1, no 3D aspect)
	imageCreateInfo.mipLevels = 1;									// Number of mip map levels
	imageCreateInfo.arrayLayers = 1;								// Number of layers in image array
	imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;				// Number of samples for multi-sampling TODO: Change when implementing MSAA
	imageCreateInfo.tiling = tiling;								// How image should be 'tiled' (arranged for optimal reading)
	imageCreateInfo.usage = useFlags;								// Bit flags defining what image will be used for
	imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;		// Whether image can be shared between queues
	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;		// Layout of image on creation

	VmaAllocationCreateInfo allocationCreateInfo = {};
	allocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;

	// Ref to know how much memory was allocated
	VmaAllocationInfo allocationInfo = {};

	LOG_VKRESULT(vmaCreateImage(vkRef.vmaAllocator, &imageCreateInfo, &allocationCreateInfo, &gpuImage.image, &gpuImage.vmaAllocation, &allocationInfo));

	// Report to GpuMemoryTracker for accurate GPU memory usage
	GpuMemoryTracker::AllocatedGpuMemory(gpuImage.usageTag, allocationInfo.size);

	gpuImage.imageView = CreateImageView(vkRef, gpuImage.image, format, aspectFlags);

	return gpuImage;
}

void VkImageHelpers::DestroyImage(const VkRef& vkRef, GpuImage& gpuImage)
{
	vkDestroyImageView(vkRef.logDevice, gpuImage.imageView, &vkRef.hostAllocator);

	// Report to GpuMemoryTracker for accurate GPU memory usage
	VmaAllocationInfo allocationInfo = {};
	vmaGetAllocationInfo(vkRef.vmaAllocator, gpuImage.vmaAllocation, &allocationInfo);
	GpuMemoryTracker::DeallocatedGpuMemory(gpuImage.usageTag, allocationInfo.size);

	vmaDestroyImage(vkRef.vmaAllocator, gpuImage.image, gpuImage.vmaAllocation);
}