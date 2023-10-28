#include "SwapChain.h"
#include "VkTypes.h"
#include "VkBuffersAndImages.h"
#include "Logger.h"
#include "ImGuiManager.h"


void SwapChain::CreateIntialSwapChain(VkRef& vkRef)
{
	LOG_DEBUG("Creating Vulkan Swap Chain...");

	// Make sure the chosen buffer count is acceptable.
	const u32 minImageCount = vkRef.phyDevice.surfaceCapabilities.minImageCount;
	const u32 maxImageCount = vkRef.phyDevice.surfaceCapabilities.maxImageCount;
	const u32 prefferedImageCount = vkRef.phyDevice.swapChainBufferCount;
	ASSERT_TRUE(prefferedImageCount > minImageCount && prefferedImageCount < maxImageCount);

	CreateSwapChain(vkRef);

	LOG_INFO("Created Vulkan Swap Chain");
}


void SwapChain::CreateSwapChain(VkRef& vkRef)
{
	// Set the new extent based on the window
	m_SwapChainExtent = ChooseImageExtent(vkRef);
	//LOG_DEBUG(T_string("New SwapChain Extent- X: ", std::to_string(m_SwapChainExtent.width), " | Y: ", std::to_string(m_SwapChainExtent.height)));

	// Check if window is minimized
	if (m_SwapChainExtent.width <= 0 || m_SwapChainExtent.height <= 0)
	{
		m_bWindowMinimized = true;
		return;
	}
	else
	{
		m_bWindowMinimized = false;
	}

	VkSwapchainKHR oldSwapchain = m_SwapChain;

	// Set the Swap Chain Create Info
	VkSwapchainCreateInfoKHR swapChainCreateInfo = {};
	swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;				// Boiler
	swapChainCreateInfo.surface = vkRef.surface;											// Ref to current surface.
	swapChainCreateInfo.minImageCount = vkRef.phyDevice.swapChainBufferCount;				// Minimum images in the swapchain
	swapChainCreateInfo.imageFormat = vkRef.phyDevice.preferredSurfaceFormat.format;		// Swapchain format
	swapChainCreateInfo.imageColorSpace = vkRef.phyDevice.preferredSurfaceFormat.colorSpace;// Swapchain color space
	swapChainCreateInfo.imageExtent = m_SwapChainExtent;									// Swapchain image extents
	swapChainCreateInfo.imageArrayLayers = 1;												// Number of layers for each image in chain.
	swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;					// What attachment images will be used as VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT most common for images to be presented to the screen.
	swapChainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;				// Transform to perform on swapchain images. TODO: Check if android devices need a VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR pre-transform
	swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;					// How the image should blend with other images/windows that it occludes. 
	swapChainCreateInfo.presentMode = vkRef.phyDevice.preferredPresentMode;					// Swapchain presentation mode
	swapChainCreateInfo.clipped = VK_TRUE;													// Whether to clip parts of image not in view (e.g. behind another window, off screen, etc)
	swapChainCreateInfo.oldSwapchain = oldSwapchain;										// Link to old swapchain being destroyed to hand over responsibilities (common case is screen resizing where you would destroy and recreate swapchain with every resize)


	// If graphics and presentation families are different, then swapchain must let images be shared between families.
	if (vkRef.phyDevice.graphicsQueueIndex != vkRef.phyDevice.presentQueueIndex)
	{
		// Queues to share between.
		u32 queueFamilyIndices[] = {
			(u32)vkRef.phyDevice.graphicsQueueIndex,
			(u32)vkRef.phyDevice.presentQueueIndex
		};

		swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;	// Image share handling
		swapChainCreateInfo.queueFamilyIndexCount = 2;						// Number of queues to share images between
		swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;		// Array of queues to share between
	}
	else // Default (and most likely) case if queue handles are the same.
	{
		swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapChainCreateInfo.queueFamilyIndexCount = 0;
		swapChainCreateInfo.pQueueFamilyIndices = nullptr;
	}

	LOG_VKRESULT(vkCreateSwapchainKHR(vkRef.logDevice, &swapChainCreateInfo, &vkRef.hostAllocator, &m_SwapChain));

	// Hold reference for later
	m_SwapChainFormat = vkRef.phyDevice.preferredSurfaceFormat.format;

	// Get swap chain images and copy over to our local class.
	uint32_t swapChainImageCount;
	vkGetSwapchainImagesKHR(vkRef.logDevice, m_SwapChain, &swapChainImageCount, nullptr);
	T_vector<VkImage> images(swapChainImageCount);
	vkGetSwapchainImagesKHR(vkRef.logDevice, m_SwapChain, &swapChainImageCount, images.data());

	if (oldSwapchain != VK_NULL_HANDLE)
	{
		for (auto& image : m_SwapChainImages)
			vkDestroyImageView(vkRef.logDevice, image.imageView, &vkRef.hostAllocator);
		m_SwapChainImages.clear();
	}

	for (VkImage image : images)
	{
		// Store image handle (int index)
		SwapChainImage swapChainImage = {};
		swapChainImage.image = image;

		// Create Image view and store it.
		swapChainImage.imageView = VkImageHelpers::CreateImageView(vkRef, image, m_SwapChainFormat, VK_IMAGE_ASPECT_COLOR_BIT);

		// Add to swap chain image list.
		m_SwapChainImages.emplace_back(swapChainImage);
	}

	if (oldSwapchain != VK_NULL_HANDLE)
	{
		vkDestroySwapchainKHR(vkRef.logDevice, oldSwapchain, &vkRef.hostAllocator);
	}

	// Update ImGui with new swapchain
	ImGuiManager::CreateImGuiFrameBuffer(vkRef, m_SwapChainImages, m_SwapChainExtent);
}

void SwapChain::DestroySwapChain(const VkRef& vkRef)
{
	for (auto& image : m_SwapChainImages)
		vkDestroyImageView(vkRef.logDevice, image.imageView, &vkRef.hostAllocator);

	vkDestroySwapchainKHR(vkRef.logDevice, m_SwapChain, &vkRef.hostAllocator);
}

void SwapChain::CheckForUnMinimize(VkRef& vkRef)
{
	int width, height;

	if constexpr (GlobalConstants::bOnAndroid)
	{
		// TODO: Add android implementation
	}
	else // GLFW
	{
		glfwGetFramebufferSize(vkRef.pWindow, &width, &height);
	}

	// Check if window is unminimized and create a new swap chain if it is.
	if (width > 0 || height > 0)
	{
		m_bWindowMinimized = false;
		CreateSwapChain(vkRef);
	}
}

const u64 SwapChain::Size() const { return m_SwapChainImages.size(); }

const T_vector<SwapChainImage, MT_GRAPHICS> SwapChain::GetImages() const { return m_SwapChainImages; }

VkExtent2D SwapChain::ChooseImageExtent(VkRef& vkRef)
{
	// Make it more readable
	#define surfCap vkRef.phyDevice.surfaceCapabilities

	LOG_VKRESULT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkRef.phyDevice.handle, vkRef.surface, &surfCap));

	int width, height;

	if constexpr (GlobalConstants::bOnAndroid)
	{
		// TODO: Add android implementation
	}
	else // GLFW
	{
		glfwGetFramebufferSize(vkRef.pWindow, &width, &height);
	}

	VkExtent2D newExtent = {};
	newExtent.width = static_cast<u32>(width);
	newExtent.height = static_cast<u32>(height);

	newExtent.width = std::clamp(newExtent.width, surfCap.minImageExtent.width, surfCap.maxImageExtent.width);
	newExtent.height = std::clamp(newExtent.height, surfCap.minImageExtent.height, surfCap.maxImageExtent.height);

	return newExtent;

	#undef surfCap // Clean up
}
