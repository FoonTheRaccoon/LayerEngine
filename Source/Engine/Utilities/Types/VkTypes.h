#pragma once
#include "ThirdParty.h"
#include "GlobalConstants.h"
#include "MemoryTracker.h"
#include "LayerContainers.h"


struct SwapChainImage
{
	SwapChainImage() = default;
	~SwapChainImage() = default;

	VkImage image;
	VkImageView imageView;
};

struct PhysicalDevice
{
	PhysicalDevice() = default;
	~PhysicalDevice() = default;

	// Info
	VkPhysicalDevice handle = VK_NULL_HANDLE;
	VkPhysicalDeviceProperties properties = {};
	VkPhysicalDeviceFeatures features = {};

	// Queues
	i32 presentQueueIndex = -1;
	i32 graphicsQueueIndex = -1;
	i32 computeQueueIndex = -1;
	i32 transferQueueIndex = -1;
	bool bHasTransferQueue = false;

	// Swap Chain compatibility details
	VkSurfaceCapabilitiesKHR surfaceCapabilities = {};				// Surface properties, e.g. image size/extent.
	T_vector<VkSurfaceFormatKHR, MT_GRAPHICS> surfaceFormats = {};	// Surface compatible image formats and color space
	T_vector<VkPresentModeKHR, MT_GRAPHICS> presentationModes = {};	// The mode(s) that determine how images are swapped on the buffer/screen (Mailbox and FIFO for no screen tearing, FIFO can lag)

	// Preferred Surface format, present mode, and buffer count that this device is compatible with
	VkSurfaceFormatKHR preferredSurfaceFormat = {};
	VkPresentModeKHR preferredPresentMode = VK_PRESENT_MODE_FIFO_KHR;
	u32 swapChainBufferCount = 2;
	u32 numInFlightFrames = 1;
	bool bSupportsMailboxMode = false;

	// Attachment Formats
	VkFormat preferred32BitPackColorAttachmentFormat = {};
	VkFormat preferredDepthStencilAttachmentFormat = {};

	bool bSupports16BitPackColorAttachment = false;
	VkFormat preferred16BitPackColorAttachmentFormat = VK_FORMAT_UNDEFINED;
	bool bSupports10BitColorAttachment = false;
	VkFormat preferred10BitColorAttachmentFormat = VK_FORMAT_UNDEFINED;


	// Misc Data brought to the front
	u64 minUniformBufferOffset = 256;	// Used for dynamic Buffers
};

struct DeviceQueues
{
	DeviceQueues() = default;
	~DeviceQueues() = default;

	VkQueue present = {};
	VkQueue graphics = {};
	VkQueue compute = {};
	VkQueue transfer = VK_NULL_HANDLE;
	bool bHasTransferQueue = false;
};

struct VkRef
{
	VkRef() = default;
	~VkRef() = default;

	Window* pWindow = nullptr;

	VkInstance instance = {};
	VkAllocationCallbacks hostAllocator = {};
	VmaAllocator vmaAllocator = {};
	VkSurfaceKHR surface = {};

	PhysicalDevice phyDevice = {};
	VkDevice logDevice = {};

	DeviceQueues queues = {};

	VkCommandPool graphicsCommandPool = {};
	T_vector<VkCommandBuffer, MT_GRAPHICS> graphicsCommandBuffers = {};

	VkCommandPool computeCommandPool = {};
	T_vector<VkCommandBuffer, MT_GRAPHICS> computeCommandBuffers = {};

	bool bHasTransferCommandBuffer = false;
	VkCommandPool transferCommandPool = {};
	T_vector<VkCommandBuffer, MT_GRAPHICS> transferCommandBuffers = {};
};
