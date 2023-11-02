 #include "VkSetup.h"
#include "LayerContainers.h"
#include "VkConfig.h"
#include "Viewport.h"
#include "VkTypes.h"
#include "Logger.h"
#include "LoggingCallbacks.h"
#include "vk_enum_string_helper.h"


namespace VkSetup
{
	// Array of available physical devices and their properties. Gets populated during VkSetup::CapturePhysicalDevice
	T_vector<PhysicalDevice, MT_GRAPHICS> _availablePhysicalDevices = {};

	// INITIALIZE HELPERS

	// -CreateInstance Helpers
	T_vector<const char*> _GetRequiredInstanceExtensions();
	bool _CheckInstanceExtensionSupport(const T_vector<const char*>& checkExtensions);

	// -CapturePhysicalDevice Helpers
	bool _CheckPhysicalDeviceIsSuitableAndBuildReference(VkPhysicalDevice phyDevice, VkSurfaceKHR surface, PhysicalDevice& phyDeviceReference);
	bool _CheckPhysicalDeviceSupportsDesiredFeatures(PhysicalDevice& phyDeviceReference);
	bool _CheckPhysicalDeviceSupportsDesiredExtensions(PhysicalDevice& phyDeviceReference);
	bool _CheckQueueFamiliesAreSuitableAndSetRef(PhysicalDevice& phyDeviceReference, VkSurfaceKHR surface);
	bool _CheckAndSetSwapChainDetails(PhysicalDevice& phyDeviceReference, VkSurfaceKHR surface);
	bool _CheckAndSetAttachmentFormats(PhysicalDevice& phyDeviceReference);
 
	template<size_t S>
	VkFormat _ChooseSupportedAttachmentFormat([[maybe_unused]] const PhysicalDevice& phyDeviceReference, [[maybe_unused]] const std::array<VkFormat, S>& formats, [[maybe_unused]] VkImageTiling tiling, [[maybe_unused]] VkFormatFeatureFlags featureFlags);

	// Returns string for VkPhysicalDeviceFeatures struct member
	const char* _GetVkPhysicalDeviceFeaturesName(u32 index);
}


void VkSetup::CreateInstance(const char* appName, VkRef& vkRef)
{
	LOG_DEBUG("Creating Vulkan Instance...")

	// Set up the VkAllocationCallbacks
	vkRef.hostAllocator.pfnAllocation =				MemoryTackingCallbacks::vkAllocateHostMemory;
	vkRef.hostAllocator.pfnFree =					MemoryTackingCallbacks::vkFreeHostMemory;
	vkRef.hostAllocator.pfnReallocation =			MemoryTackingCallbacks::vkReallocateHostMemory;
	vkRef.hostAllocator.pfnInternalAllocation =		MemoryTackingCallbacks::vkInternalAllocationNotification;
	vkRef.hostAllocator.pfnInternalFree =			MemoryTackingCallbacks::vkInternalFreeNotification;
    
    
    #if LAYER_USE_VALIDATION_LAYERS
        // Early check to see if validation layers should be used and if they are supported
        LOG_ERROR_IF(!ValidationLayers::CheckValidationLayerSupport(),
                     "Required Vulkan validation layers are not supported!")
    #endif

	// Create information about the application
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = appName;								// Name of the App
	appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);	// Version of the App
	appInfo.pEngineName = "Layer Engine";							// Name of the Engine
	appInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);		// Version of the Engine
	appInfo.apiVersion = VK_API_VERSION_1_1;						// Vulkan API to be used

	// Set the info for the Instance to be created
	VkInstanceCreateInfo instanceCreateInfo = {};
	instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceCreateInfo.pApplicationInfo = &appInfo;

	T_vector<const char*> extensions = _GetRequiredInstanceExtensions();

	LOG_FATAL_IF(!_CheckInstanceExtensionSupport(extensions),
                 "Required Vulkan Instance Extensions Are Not Available!")

	instanceCreateInfo.enabledExtensionCount = static_cast<u32>(extensions.size());
	instanceCreateInfo.ppEnabledExtensionNames = extensions.data(); // Sets extensions

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    #if LAYER_USE_VALIDATION_LAYERS
		instanceCreateInfo.enabledLayerCount = static_cast<u32>(ValidationLayers::DesiredLayers.size());
		instanceCreateInfo.ppEnabledLayerNames = ValidationLayers::DesiredLayers.data();

		ValidationLayers::PopulateDebugMessengerCreateInfo(debugCreateInfo);
		instanceCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
		LOG_DEBUG("Loading And Checking Validation Layers...")
    #else
		instanceCreateInfo.enabledLayerCount = 0;
		instanceCreateInfo.pNext = nullptr;
    #endif // LAYER_USE_VALIDATION_LAYERS

	// Create the Instance
	LOG_VKRESULT(vkCreateInstance(&instanceCreateInfo, &vkRef.hostAllocator, &vkRef.instance))
	LOG_INFO("Created Vulkan Instance And Setup Vulkan Host Memory Allocation Callbacks")
}

void VkSetup::CreateSurface(VkRef& vkRef)
{
	LOG_DEBUG("Creating Vulkan Surface...")
    
    #if LAYER_PLATFORM_ANDROID
		// TODO: Create Android implementation
    #else // GLFW
		LOG_VKRESULT(glfwCreateWindowSurface(vkRef.instance, vkRef.pWindow, &vkRef.hostAllocator, &vkRef.surface))
    #endif
    
	LOG_INFO("Created Vulkan Surface")
}

void VkSetup::CapturePhysicalDevice(VkRef& vkRef)
{
	LOG_DEBUG("Capturing Vulkan Physical Device...")

	// Enumerate Physical devices that the instance can access.
	u32 physicalDeviceCount = 0;
	LOG_VKRESULT(vkEnumeratePhysicalDevices(vkRef.instance, &physicalDeviceCount, nullptr))

	// Log Fatal if there are no available devices
	LOG_FATAL_IF(physicalDeviceCount == 0,
		"No Vulkan Compatible Physical Device Found!")

	// Create an Array with the correct size then populate it with the available devices.
	T_vector<VkPhysicalDevice> physicalDevicesAvailable(physicalDeviceCount);
	LOG_VKRESULT(vkEnumeratePhysicalDevices(vkRef.instance, &physicalDeviceCount, physicalDevicesAvailable.data()))

	// Capture Suitable Devices and create references to them
	for (const auto& physicalDevice : physicalDevicesAvailable)
	{
		PhysicalDevice physicalDeviceRef;

		if (_CheckPhysicalDeviceIsSuitableAndBuildReference(physicalDevice, vkRef.surface, physicalDeviceRef))
		{
			LOG_INFO(T_string("Suitable Vulkan Physical Device Available: ", physicalDeviceRef.properties.deviceName))
			_availablePhysicalDevices.emplace_back(physicalDeviceRef);
		}
	}

	if (_availablePhysicalDevices.empty()) { LOG_FATAL("No Suitable Vulkan Physical Device Found For Given Requirements!") }

	// Check Suitable Devices and choose one (discrete GPU preferred)
	u32 suitableDevice = 0;
	for (u32 i = 0; i < _availablePhysicalDevices.size(); i++)
	{
		if (_availablePhysicalDevices[i].properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		{
			suitableDevice = i;
			break;
		}
		suitableDevice = i;
	}

	vkRef.phyDevice = _availablePhysicalDevices[suitableDevice];
    
    #if LAYER_PLATFORM_ANDROID
        // TODO: Create Android implementation
    #else // GLFW
        // Set window limits based off what the hardware can support
        const int maxWidth = static_cast<int>(vkRef.phyDevice.properties.limits.maxFramebufferWidth);
        const int maxHeight = static_cast<int>(vkRef.phyDevice.properties.limits.maxFramebufferHeight);
        glfwSetWindowSizeLimits(vkRef.pWindow, Viewport::minWindowWidth, Viewport::minWindowHeight, maxWidth, maxHeight);
    #endif

	LOG_INFO(T_string("Captured Vulkan Physical Device: ", vkRef.phyDevice.properties.deviceName))
}

void VkSetup::CreateLogicalDevice(VkRef& vkRef)
{
	LOG_DEBUG("Creating Vulkan Logical Device And Queues...")

	// Create a set that will filter out any overlapping queue families and a vector for them to be conditionally pushed into.
	std::set<i32> queueFamilyIndices = {
		vkRef.phyDevice.graphicsQueueIndex,
		vkRef.phyDevice.presentQueueIndex,
		vkRef.phyDevice.computeQueueIndex }; // Set will only allow one of each index number, filters out duplicates.

	if (vkRef.phyDevice.bHasTransferQueue)
	{
		queueFamilyIndices.emplace(vkRef.phyDevice.transferQueueIndex);
	}
	T_vector<VkDeviceQueueCreateInfo> queueCreateInfos;

	// List of queues the logical device needs to create and the info to do so, pushed into a vector to be used by deviceCreateInfo
	for (i32 queueFamilyIndex : queueFamilyIndices)
	{
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamilyIndex;				// The index of the family to create a queue from
		queueCreateInfo.queueCount = 1;										// Number of queues to create
		f32 priority = 1.0f;												// Priority of the queue 0.0~1.0 compared to other queues (1 == highest priority)
		queueCreateInfo.pQueuePriorities = &priority;

		queueCreateInfos.emplace_back(queueCreateInfo);
	}

	// Info to create logical device (also called "device")
	VkDeviceCreateInfo deviceCreateInfo = {};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.queueCreateInfoCount = static_cast<u32>(queueCreateInfos.size());						// Number of Queue create infos
	deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();											// List of queue create infos
	deviceCreateInfo.enabledExtensionCount = static_cast<u32>(VkConfig::desiredDeviceExtensions.size());	// Number of logical device extensions (different from Instance extensions)
	deviceCreateInfo.ppEnabledExtensionNames = VkConfig::desiredDeviceExtensions.data();					// List of enabled logical device extensions (if any)
	deviceCreateInfo.pEnabledFeatures = &VkConfig::desiredDeviceFeatures;									// Features That Should Be Enabled

	// Create the logical device for the given physical device
	LOG_VKRESULT(vkCreateDevice(vkRef.phyDevice.handle, &deviceCreateInfo, &vkRef.hostAllocator, &vkRef.logDevice))

	// Get handle to the queue(s) that was created the same time as the device.
	// From given logical device, of given Queue Family, of given Queue Index (0 since only 1 queue), place reference in given VkQueue
	// We try to grab different queue if they are separate, but if they aren't separate the both handles will point to the same place.
	vkGetDeviceQueue(vkRef.logDevice, vkRef.phyDevice.graphicsQueueIndex, 0, &vkRef.queues.graphics);
	vkGetDeviceQueue(vkRef.logDevice, vkRef.phyDevice.presentQueueIndex, 0, &vkRef.queues.present);
	vkGetDeviceQueue(vkRef.logDevice, vkRef.phyDevice.computeQueueIndex, 0, &vkRef.queues.compute);
	if (vkRef.phyDevice.bHasTransferQueue)
	{
		vkGetDeviceQueue(vkRef.logDevice, vkRef.phyDevice.transferQueueIndex, 0, &vkRef.queues.transfer);
		vkRef.queues.bHasTransferQueue = true;
	}

	LOG_INFO("Created Vulkan Logical Device And Queues")
}


void VkSetup::CreateVmaAllocator(VkRef& vkRef)
{
	LOG_DEBUG("Creating VMA Allocator...")
	
	VkAllocationCallbacks hostCallbacks = {};
	hostCallbacks.pfnAllocation =			MemoryTackingCallbacks::vkAllocateHostMemory;
	hostCallbacks.pfnFree =					MemoryTackingCallbacks::vkFreeHostMemory;
	hostCallbacks.pfnReallocation =			MemoryTackingCallbacks::vkReallocateHostMemory;
	hostCallbacks.pfnInternalAllocation =	MemoryTackingCallbacks::vkInternalAllocationNotification;
	hostCallbacks.pfnInternalFree =			MemoryTackingCallbacks::vkInternalFreeNotification;

	VmaAllocatorCreateInfo allocatorCreateInfo = {};
	allocatorCreateInfo.vulkanApiVersion = VK_API_VERSION_1_1;
	allocatorCreateInfo.physicalDevice = vkRef.phyDevice.handle;
	allocatorCreateInfo.device = vkRef.logDevice;
	allocatorCreateInfo.instance = vkRef.instance;
	allocatorCreateInfo.pAllocationCallbacks = &hostCallbacks;

	LOG_VKRESULT(vmaCreateAllocator(&allocatorCreateInfo, &vkRef.vmaAllocator))

	LOG_INFO("Created VMA Allocator")
}

void VkSetup::CreateCommandPools(VkRef& vkRef)
{
	LOG_DEBUG("Creating Command Pools...")

	// Graphics Command Pool
	VkCommandPoolCreateInfo graphicsCommandPoolCreateInfo = {};
	graphicsCommandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	graphicsCommandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;	// Sets command buffers to implicitly reset whenever vkBeginCommandBuffer is called on that buffer
	graphicsCommandPoolCreateInfo.queueFamilyIndex = vkRef.phyDevice.graphicsQueueIndex;

	LOG_VKRESULT(vkCreateCommandPool(vkRef.logDevice, &graphicsCommandPoolCreateInfo, &vkRef.hostAllocator, &vkRef.graphicsCommandPool))

	// Compute Command Pool
	VkCommandPoolCreateInfo computeCommandPoolCreateInfo = {};
	computeCommandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	computeCommandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;	// Sets command buffers to implicitly reset whenever vkBeginCommandBuffer is called on that buffer
	computeCommandPoolCreateInfo.queueFamilyIndex = vkRef.phyDevice.computeQueueIndex;

	LOG_VKRESULT(vkCreateCommandPool(vkRef.logDevice, &computeCommandPoolCreateInfo, &vkRef.hostAllocator, &vkRef.computeCommandPool))

	// Transfer Command Pool
	if (vkRef.phyDevice.bHasTransferQueue)
	{
		VkCommandPoolCreateInfo transferCommandPoolCreateInfo = {};
		transferCommandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		transferCommandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;	// Sets command buffers to implicitly reset whenever vkBeginCommandBuffer is called on that buffer
		transferCommandPoolCreateInfo.queueFamilyIndex = vkRef.phyDevice.transferQueueIndex;

		vkRef.bHasTransferCommandBuffer = true;

		LOG_VKRESULT(vkCreateCommandPool(vkRef.logDevice, &transferCommandPoolCreateInfo, &vkRef.hostAllocator, &vkRef.transferCommandPool))
	}

	LOG_INFO("Command Pools Created")
}

void VkSetup::AllocateCommandBuffers(VkRef& vkRef)
{
	LOG_DEBUG("Allocating Command Buffers...")

	// Set sizes
	vkRef.graphicsCommandBuffers.resize(vkRef.phyDevice.swapChainBufferCount);
	vkRef.computeCommandBuffers.resize(vkRef.phyDevice.swapChainBufferCount);
	if(vkRef.bHasTransferCommandBuffer)
		vkRef.transferCommandBuffers.resize(vkRef.phyDevice.swapChainBufferCount);

	// Graphics Command Buffers
	VkCommandBufferAllocateInfo graphicsCommandBufferAllocateInfo = {};
	graphicsCommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	graphicsCommandBufferAllocateInfo.commandPool = vkRef.graphicsCommandPool;
	graphicsCommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;	// VK_COMMAND_BUFFER_LEVEL_PRIMARY	:	Buffer you submit directly from the queue. Can't be called by other buffers.
																				// VK_COMMAND_BUFFER_LEVEL_SECONDARY	:	Buffer can't be called directly. Can be called from other buffers via "vkCmdExecuteCommands" when recording commands in primary buffer.
	graphicsCommandBufferAllocateInfo.commandBufferCount = static_cast<u32>(vkRef.graphicsCommandBuffers.size());

	LOG_VKRESULT(vkAllocateCommandBuffers(vkRef.logDevice, &graphicsCommandBufferAllocateInfo, vkRef.graphicsCommandBuffers.data()))

	// Compute Command Buffers
	VkCommandBufferAllocateInfo computeCommandBufferAllocateInfo = {};
	computeCommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	computeCommandBufferAllocateInfo.commandPool = vkRef.computeCommandPool;
	computeCommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	computeCommandBufferAllocateInfo.commandBufferCount = static_cast<u32>(vkRef.computeCommandBuffers.size());

	LOG_VKRESULT(vkAllocateCommandBuffers(vkRef.logDevice, &computeCommandBufferAllocateInfo, vkRef.computeCommandBuffers.data()))

	// Transfer Command Buffers
	if (vkRef.bHasTransferCommandBuffer)
	{
		VkCommandBufferAllocateInfo transferCommandBufferAllocateInfo = {};
		transferCommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		transferCommandBufferAllocateInfo.commandPool = vkRef.transferCommandPool;
		transferCommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		transferCommandBufferAllocateInfo.commandBufferCount = static_cast<u32>(vkRef.transferCommandBuffers.size());

		LOG_VKRESULT(vkAllocateCommandBuffers(vkRef.logDevice, &transferCommandBufferAllocateInfo, vkRef.transferCommandBuffers.data()))
	}

	LOG_INFO("Command Buffers Allocated")
}

T_vector<const char*> VkSetup::_GetRequiredInstanceExtensions()
{
	u32 extensionCount = 0;
	const char** extensions;
    
    #if LAYER_PLATFORM_ANDROID
		// TODO: Add a way to get required android extensions
    #else // GLFW
		extensions = glfwGetRequiredInstanceExtensions(&extensionCount);
    #endif

	T_vector<const char*> requiredExtensions(extensions, extensions + extensionCount);
    
    
    #if LAYER_USE_VALIDATION_LAYERS
		requiredExtensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    #endif

	return requiredExtensions;
}

bool VkSetup::_CheckInstanceExtensionSupport(const T_vector<const char*>& checkExtensions)
{
	u32 extensionCount = 0;
	LOG_VKRESULT(vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr))

	T_vector<VkExtensionProperties> extensionsAvailable(extensionCount);
	LOG_VKRESULT(vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensionsAvailable.data()))

	for (const char* extension : checkExtensions) LOG_INFO(T_string("Required Instance Extension: ", extension))
	for (VkExtensionProperties& extension : extensionsAvailable) LOG_INFO(T_string("Available Instance Extension: ", extension.extensionName))

	// Check that every extension is available.
	bool bAllExtensionsSupported = true;
	for (const auto& extensionToCheck : checkExtensions)
	{
		bool bHasExtension = false;
		for (const auto& extensionAvailable : extensionsAvailable)
		{
			if (strcmp(extensionToCheck, extensionAvailable.extensionName) == 0)
			{
				bHasExtension = true;
				break;
			}
		}
		if (!bHasExtension) [[unlikely]]
		{
			bAllExtensionsSupported = false;
			LOG_WARNING_MIN(T_string("Required Instance Extension ", extensionToCheck, " Not Supported By Vulkan Instance"))
		}
	}

	return bAllExtensionsSupported;
}

bool VkSetup::_CheckPhysicalDeviceIsSuitableAndBuildReference(VkPhysicalDevice phyDevice, VkSurfaceKHR surface, PhysicalDevice& phyDeviceReference)
{
	// Set physical device handle and get info for it
	phyDeviceReference.handle = phyDevice;
	vkGetPhysicalDeviceProperties(phyDevice, &phyDeviceReference.properties);
	vkGetPhysicalDeviceFeatures(phyDevice, &phyDeviceReference.features);

	LOG_DEBUG(T_string("Checking If Device '", phyDeviceReference.properties.deviceName, "' Is Suitable..."))

	// Pass info along to get set and to check if it meets the given requirements
	if (!_CheckPhysicalDeviceSupportsDesiredFeatures(phyDeviceReference))		return false;
	if (!_CheckPhysicalDeviceSupportsDesiredExtensions(phyDeviceReference))		return false;
	if (!_CheckQueueFamiliesAreSuitableAndSetRef(phyDeviceReference, surface))	return false;
	if (!_CheckAndSetSwapChainDetails(phyDeviceReference, surface))				return false;
	if (!_CheckAndSetAttachmentFormats(phyDeviceReference))						return false;



	// Set any misc data
	phyDeviceReference.minUniformBufferOffset = phyDeviceReference.properties.limits.minUniformBufferOffsetAlignment; // Used for dynamic Buffers

	// If it passes all the check then we say it's suitable
	return true;
}

bool VkSetup::_CheckPhysicalDeviceSupportsDesiredFeatures(PhysicalDevice& phyDeviceReference)
{
	// Treat VkPhysicalDeviceFeatures objects like a VkBool32 array, so we can iterate through it and compared values
	const auto* desired = reinterpret_cast<const VkBool32*>(&VkConfig::desiredDeviceFeatures);
	const auto* available = reinterpret_cast<const VkBool32*>(&phyDeviceReference.features);
	const u32 numOfMembers = sizeof(VkPhysicalDeviceFeatures) / sizeof(VkBool32);

	// Iterate through every member of the desired features list and check to see if the physical device supports it.
	for (u32 i = 0; i < numOfMembers; i++)
	{
		if (desired[i] == VK_TRUE && available[i] == VK_FALSE) [[unlikely]]
		{
				LOG_WARNING_MIN(T_string("Desired Device Feature ", _GetVkPhysicalDeviceFeaturesName(i),
				" Not Supported By Device: ", phyDeviceReference.properties.deviceName))
			return false;
		}
	}

	LOG_INFO(T_string(phyDeviceReference.properties.deviceName, " Supports Desired Features"))
	return true;
}

bool VkSetup::_CheckPhysicalDeviceSupportsDesiredExtensions(PhysicalDevice& phyDeviceReference)
{
	u32 extensionAvailableCount = 0;
	LOG_VKRESULT(vkEnumerateDeviceExtensionProperties(phyDeviceReference.handle, nullptr, &extensionAvailableCount, nullptr))

	if (extensionAvailableCount == 0)
	{
		LOG_WARNING_MIN(T_string("No Vulkan Extensions Found For Device: ", phyDeviceReference.properties.deviceName))
		return false;
	}

	// Create an Array with available extensions.
	T_vector<VkExtensionProperties> extensionsAvailable(extensionAvailableCount);
	LOG_VKRESULT(vkEnumerateDeviceExtensionProperties(phyDeviceReference.handle, nullptr, &extensionAvailableCount, extensionsAvailable.data()))

	// Check to see if your wanted extensions list ('deviceExtensions' in Utils.h) is in the extensions available list.
	bool bAllExtensionsSupported = true;
	for (const auto& wantedExtension : VkConfig::desiredDeviceExtensions)
	{
		bool bExtensionSupported = false;
		for (const auto& extensionAvailable : extensionsAvailable)
		{
			if (strcmp(wantedExtension, extensionAvailable.extensionName) == 0)
			{
				bExtensionSupported = true;
				break;
			}
		}
		if (!bExtensionSupported)
		{
			bAllExtensionsSupported = false;
			LOG_WARNING_MIN(T_string("Desired Vulkan Extension ", wantedExtension, " Not Supported By Device: ", phyDeviceReference.properties.deviceName))
		}
	}

	LOG_INFO_IF(bAllExtensionsSupported, T_string(phyDeviceReference.properties.deviceName, " Supports Desired Extensions"))

	return bAllExtensionsSupported;
}

bool VkSetup::_CheckQueueFamiliesAreSuitableAndSetRef(PhysicalDevice& phyDeviceReference, VkSurfaceKHR surface)
{
	// Get a list of the devices queue families
	u32 queueFamilyPropertiesCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(phyDeviceReference.handle, &queueFamilyPropertiesCount, nullptr);

	if (queueFamilyPropertiesCount == 0)
	{
		LOG_WARNING_MIN(T_string("No Vulkan Queue Families Found For Device: ", phyDeviceReference.properties.deviceName))
		return false;
	}

	T_vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyPropertiesCount);
	vkGetPhysicalDeviceQueueFamilyProperties(phyDeviceReference.handle, &queueFamilyPropertiesCount, queueFamilyProperties.data());

	// Go through each Queue Family and check if it has at least 1 of the required types of queue.
	// -GRAPHICS QUEUE-
	for (i32 i = 0; i < queueFamilyPropertiesCount; ++i)
	{
		// Per Vulkan Spec only one queue family will have VK_QUEUE_GRAPHICS_BIT,
		if (queueFamilyProperties[i].queueCount > 0 && queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			phyDeviceReference.graphicsQueueIndex = i;
			break;
		}
	}

	if (phyDeviceReference.graphicsQueueIndex < 0)
	{
		LOG_WARNING_MIN(T_string("No Vulkan Graphics Queue Families Found For Device: ", phyDeviceReference.properties.deviceName))
		return false;
	}

	// -PRESENT QUEUE-
	for (i32 i = 0; i < queueFamilyPropertiesCount; ++i)
	{
		// Check to see if queue family supports presentation queues, set first valid queue
		VkBool32 presentationSupport = false;
		LOG_VKRESULT(vkGetPhysicalDeviceSurfaceSupportKHR(phyDeviceReference.handle, i, surface, &presentationSupport))
		if (queueFamilyProperties[i].queueCount > 0 && presentationSupport)
		{
			phyDeviceReference.presentQueueIndex = i;
			break;
		}
	}

	if (phyDeviceReference.graphicsQueueIndex < 0)
	{
		LOG_WARNING_MIN(T_string("No Vulkan Queue Family That Supports Presenting Found For Device: ", phyDeviceReference.properties.deviceName))
		return false;
	}

	// -COMPUTE QUEUE-
	bool bHasDedicatedComputeQueue = false;
	T_vector<std::pair<i32, VkQueueFamilyProperties>> computeQueues;
	for (i32 i = 0; i < queueFamilyPropertiesCount; ++i)
	{
		// Grab all the queue families that have the VK_QUEUE_COMPUTE_BIT flag
		if (queueFamilyProperties[i].queueCount > 0 && queueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
		{
			// See if compute queue is a dedicated compute queue (Usually supports everything except graphics, or it's only compute)
			if (!(queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT))
			{
				// If dedicated compute queue found then set it in the ref and break out early
				bHasDedicatedComputeQueue = true;
				phyDeviceReference.computeQueueIndex = i;
				break;
			}

			// Else add to the list of compute queues to evaluate later
			computeQueues.emplace_back(std::pair(i, queueFamilyProperties[i] ));
		}
	}

	if (computeQueues.empty() && !bHasDedicatedComputeQueue)
	{
		LOG_WARNING_MIN(T_string("No Vulkan Compute Queue Family Found For Device: ", phyDeviceReference.properties.deviceName))
		return false;
	}

	if (!bHasDedicatedComputeQueue)
	{
		u32 highestQueueCount = 0;
		i32 bestQueue = 0;
		for (auto& computeQueue : computeQueues)
		{
			// Compare to see if this queue has the highest count and set best queue to this queue if it's higher
			if (highestQueueCount < computeQueue.second.queueCount)
			{
				bestQueue = computeQueue.first;
				highestQueueCount = computeQueue.second.queueCount;
			}
		}

		phyDeviceReference.computeQueueIndex = bestQueue;
	}

	// -TRANSFER QUEUE-
	bool bHasDedicatedTransferQueue = false;
	T_vector<std::pair<i32, VkQueueFamilyProperties>> transferQueues;
	T_vector<std::pair<i32, VkQueueFamilyProperties>> dedicatedTransferQueues;
	for (i32 i = 0; i < queueFamilyPropertiesCount; ++i)
	{
		// Grab all the queue families that have the VK_QUEUE_TRANSFER_BIT flag
		if (queueFamilyProperties[i].queueCount > 0 && queueFamilyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
		{
			// See if transfer queue is a dedicated transfer queue (Usually won't have VK_QUEUE_GRAPHICS_BIT or VK_QUEUE_COMPUTE_BIT)
			if (!(queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) && !(queueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT))
			{
				// If dedicated transfer queue found then add it to list to evaluate later
				bHasDedicatedTransferQueue = true;
				dedicatedTransferQueues.emplace_back(std::pair(i, queueFamilyProperties[i]));
			}

			// Else add to the list of transfer queue to evaluate later
			transferQueues.emplace_back(std::pair(i, queueFamilyProperties[i]));
		}
	}

	// If there are dedicated transfer queues then set the ref to the one with the highest count
	if (bHasDedicatedTransferQueue && !dedicatedTransferQueues.empty())
	{
		u32 highestQueueCount = 0;
		i32 bestQueue = 0;
		for (auto& dedicatedTransferQueue : dedicatedTransferQueues)
		{
			// Compare to see if this queue has the highest count and set best queue to this queue if it's higher
			if (highestQueueCount < dedicatedTransferQueue.second.queueCount)
			{
				bestQueue = dedicatedTransferQueue.first;
				highestQueueCount = dedicatedTransferQueue.second.queueCount;
			}
		}

		phyDeviceReference.transferQueueIndex = bestQueue;
		phyDeviceReference.bHasTransferQueue = true;
	}
	else if (!bHasDedicatedTransferQueue && !transferQueues.empty())
	{ // Else set the ref transfer queue to the one with the highest count
		u32 highestQueueCount = 0;
		i32 bestQueue = 0;
		for (auto& transferQueue : transferQueues)
		{
			// Compare to see if this queue has the highest count and set best queue to this queue if it's higher
			if (highestQueueCount < transferQueue.second.queueCount)
			{
				bestQueue = transferQueue.first;
				highestQueueCount = transferQueue.second.queueCount;
			}
		}

		phyDeviceReference.transferQueueIndex = bestQueue;
		phyDeviceReference.bHasTransferQueue = true;
	}

	LOG_INFO(T_string(phyDeviceReference.properties.deviceName, " Has All Required Queue Families | (Graphics/Present/Compute) == (",
		std::to_string(phyDeviceReference.graphicsQueueIndex), "/",
		std::to_string(phyDeviceReference.presentQueueIndex), "/",
		std::to_string(phyDeviceReference.computeQueueIndex), ")"))

	LOG_INFO_IF(bHasDedicatedTransferQueue, T_string(phyDeviceReference.properties.deviceName,
		" Has Dedicated Transfer Queue Family == ", std::to_string(phyDeviceReference.transferQueueIndex)))
	LOG_INFO_IF(!bHasDedicatedTransferQueue && phyDeviceReference.bHasTransferQueue, T_string(phyDeviceReference.properties.deviceName,
		" Transfer Queue Family == ", std::to_string(phyDeviceReference.transferQueueIndex)))

	return true;
}

bool VkSetup::_CheckAndSetSwapChainDetails(PhysicalDevice& phyDeviceReference, VkSurfaceKHR surface)
{
	// -- Capabilities --
	// Get the surface capabilities for the given surface on the given device.
	LOG_VKRESULT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(phyDeviceReference.handle, surface, &phyDeviceReference.surfaceCapabilities))

	// -- Formats --
	u32 surfaceFormatCount = 0;
	LOG_VKRESULT(vkGetPhysicalDeviceSurfaceFormatsKHR(phyDeviceReference.handle, surface, &surfaceFormatCount, nullptr))

	if (surfaceFormatCount == 0)
	{
		LOG_WARNING_MIN(T_string("No Vulkan Surface Formats Found For Device: ", phyDeviceReference.properties.deviceName))
		return false;
	}
	
	phyDeviceReference.surfaceFormats.resize(surfaceFormatCount);
	LOG_VKRESULT(vkGetPhysicalDeviceSurfaceFormatsKHR(phyDeviceReference.handle, surface, &surfaceFormatCount, phyDeviceReference.surfaceFormats.data()))

	bool bFoundSuitableSurfaceFormat = false;
	for (const VkFormat& desiredFormat : VkConfig::desiredSurfaceFormats)
	{
		for (const VkSurfaceFormatKHR& availableSurfaceFormat : phyDeviceReference.surfaceFormats)
		{
			if (availableSurfaceFormat.format == desiredFormat && availableSurfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				phyDeviceReference.preferredSurfaceFormat = availableSurfaceFormat;
				bFoundSuitableSurfaceFormat = true;
				goto endloop;
			}
		}
	}

endloop:
	if (!bFoundSuitableSurfaceFormat)
	{
		LOG_WARNING_MIN(T_string("No Compatible Vulkan Surface Format Found For Device: ", phyDeviceReference.properties.deviceName))
		return false;
	}

	// -- Presentation Modes --
	u32 presentModeCount = 0;
	LOG_VKRESULT(vkGetPhysicalDeviceSurfacePresentModesKHR(phyDeviceReference.handle, surface, &presentModeCount, nullptr))

	if (presentModeCount == 0)
	{
		LOG_WARNING_MIN(T_string("No Vulkan Surface Present Modes Found For Device: ", phyDeviceReference.properties.deviceName))
		return false;
	}

	phyDeviceReference.presentationModes.resize(presentModeCount);
	LOG_VKRESULT(vkGetPhysicalDeviceSurfacePresentModesKHR(phyDeviceReference.handle, surface, &presentModeCount, phyDeviceReference.presentationModes.data()))

	// Set preferred present mode to mailbox if available and set swapChainBufferCount to 3 for triple buffering
	for (const auto& mode : phyDeviceReference.presentationModes)
	{
		if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			phyDeviceReference.preferredPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
			phyDeviceReference.swapChainBufferCount = 3;	// Triple Buffer
			phyDeviceReference.numInFlightFrames = 2;		// Should be 1 less than total swap chain count
			phyDeviceReference.bSupportsMailboxMode = true;
			break;
		}
	}
	// If Mailbox mode isn't available the just use FIFO (Which is guaranteed per Vulkan spec) and set swapChainBufferCount to 2 for double buffering
	if (!phyDeviceReference.bSupportsMailboxMode)
	{
		phyDeviceReference.preferredPresentMode = VK_PRESENT_MODE_FIFO_KHR;
		phyDeviceReference.swapChainBufferCount = 2;	// Double Buffer
		phyDeviceReference.numInFlightFrames = 1;		// Should be 1 less than total swap chain count
	}

	LOG_INFO(T_string(phyDeviceReference.properties.deviceName, " Preferred Surface Format: ",
		string_VkFormat(phyDeviceReference.preferredSurfaceFormat.format),
		" / ", string_VkColorSpaceKHR(phyDeviceReference.preferredSurfaceFormat.colorSpace)))

	LOG_INFO(T_string(phyDeviceReference.properties.deviceName, " Preferred Present Mode: ",
		string_VkPresentModeKHR(phyDeviceReference.preferredPresentMode),
		" | Set Buffer Size: ", std::to_string(phyDeviceReference.swapChainBufferCount)))

	return true;
}

bool VkSetup::_CheckAndSetAttachmentFormats(PhysicalDevice& phyDeviceReference)
{
	// Basic 32bit Pack Color Attachment Check And Set
	const VkFormat format32BitPack = _ChooseSupportedAttachmentFormat(phyDeviceReference,
		VkConfig::desiredColorAttachment32BitPackFormats,
		VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT);
	if (format32BitPack != VK_FORMAT_UNDEFINED)
	{
		phyDeviceReference.preferred32BitPackColorAttachmentFormat = format32BitPack;
		LOG_INFO(T_string(phyDeviceReference.properties.deviceName, " Preferred 32 Bit Pack Format: ", string_VkFormat(format32BitPack)))
	}
	else
	{
		LOG_WARNING_MIN(T_string("No Suitable Vulkan 32 Bit Pack Color Attachment Format Found For Device: ", phyDeviceReference.properties.deviceName))
		return false;
	}

	// Depth Stencil Attachment Check And Set
	const VkFormat formatDepthStencil = _ChooseSupportedAttachmentFormat(phyDeviceReference,
		VkConfig::desiredDepthStencilAttachmentFormats,
		VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
	if (formatDepthStencil != VK_FORMAT_UNDEFINED)
	{
		phyDeviceReference.preferredDepthStencilAttachmentFormat = formatDepthStencil;
		LOG_INFO(T_string(phyDeviceReference.properties.deviceName, " Preferred Depth Stencil Format: ", string_VkFormat(formatDepthStencil)))
	}
	else
	{
		LOG_WARNING_MIN(T_string("No Suitable Vulkan Depth Stencil Attachment Format Found For Device: ", phyDeviceReference.properties.deviceName))
		return false;
	}

	// 16bit Pack Color Attachment Check And Set (Optional)
	const VkFormat format16BitPack = _ChooseSupportedAttachmentFormat(phyDeviceReference,
		VkConfig::desiredColorAttachment16BitPackFormats,
		VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT);
	if (format16BitPack != VK_FORMAT_UNDEFINED)
	{
		phyDeviceReference.preferred16BitPackColorAttachmentFormat = format16BitPack;
		phyDeviceReference.bSupports16BitPackColorAttachment = true;
		LOG_INFO(T_string(phyDeviceReference.properties.deviceName, " Preferred 16 Bit Pack Format: ", string_VkFormat(format16BitPack)))
	}
	else
	{
		LOG_WARNING_MIN(T_string("No Suitable Vulkan 16 Bit Pack Color Attachment Format Found For Device: ", phyDeviceReference.properties.deviceName))
	}

	// 10bit Color Attachment Check And Set (Optional)
	const VkFormat format10BitColor = _ChooseSupportedAttachmentFormat(phyDeviceReference,
		VkConfig::desiredColorAttachment10BitColorFormats,
		VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT);
	if (format10BitColor != VK_FORMAT_UNDEFINED)
	{
		phyDeviceReference.preferred10BitColorAttachmentFormat = format10BitColor;
		phyDeviceReference.bSupports10BitColorAttachment = true;
		LOG_INFO(T_string(phyDeviceReference.properties.deviceName, " Preferred 10 Bit Color Format: ", string_VkFormat(format10BitColor)))
	}
	else
	{
		LOG_WARNING_MIN(T_string("No Suitable Vulkan 10 Bit Color Attachment Format Found For Device: ", phyDeviceReference.properties.deviceName))
	}

	return true;
}

template<size_t S>
VkFormat VkSetup::_ChooseSupportedAttachmentFormat([[maybe_unused]] const PhysicalDevice& phyDeviceReference,
                                                   [[maybe_unused]] const std::array<VkFormat, S>& formats,
                                                   [[maybe_unused]] VkImageTiling tiling,
                                                   [[maybe_unused]] VkFormatFeatureFlags featureFlags)
{
	// Return first supported format that meet criteria or return VK_FORMAT_UNDEFINED if none found.
	for (const VkFormat& format : formats)
	{
		// Get properties for a given property on this device
		VkFormatProperties properties = {};
		vkGetPhysicalDeviceFormatProperties(phyDeviceReference.handle, format, &properties);

		// Depending on tiling choice, need to check for different bit flag
		if (tiling == VK_IMAGE_TILING_LINEAR && (properties.linearTilingFeatures & featureFlags) == featureFlags)
		{
			return format;
		}
		if (tiling == VK_IMAGE_TILING_OPTIMAL && (properties.optimalTilingFeatures & featureFlags) == featureFlags)
		{
			return format;
		}
	}
 
	return VK_FORMAT_UNDEFINED;
}

const char* VkSetup::_GetVkPhysicalDeviceFeaturesName(u32 index)
{
	constexpr const char* featureNames[] = {
		"robustBufferAccess",
		"fullDrawIndexUint32",
		"imageCubeArray",
		"independentBlend",
		"geometryShader",
		"tessellationShader",
		"sampleRateShading",
		"dualSrcBlend",
		"logicOp",
		"multiDrawIndirect",
		"drawIndirectFirstInstance",
		"depthClamp",
		"depthBiasClamp",
		"fillModeNonSolid",
		"depthBounds",
		"wideLines",
		"largePoints",
		"alphaToOne",
		"multiViewport",
		"samplerAnisotropy",
		"textureCompressionETC2",
		"textureCompressionASTC_LDR",
		"textureCompressionBC",
		"occlusionQueryPrecise",
		"pipelineStatisticsQuery",
		"vertexPipelineStoresAndAtomics",
		"fragmentStoresAndAtomics",
		"shaderTessellationAndGeometryPointSize",
		"shaderImageGatherExtended",
		"shaderStorageImageExtendedFormats",
		"shaderStorageImageMultisample",
		"shaderStorageImageReadWithoutFormat",
		"shaderStorageImageWriteWithoutFormat",
		"shaderUniformBufferArrayDynamicIndexing",
		"shaderSampledImageArrayDynamicIndexing",
		"shaderStorageBufferArrayDynamicIndexing",
		"shaderStorageImageArrayDynamicIndexing",
		"shaderClipDistance",
		"shaderCullDistance",
		"shaderFloat64",
		"shaderInt64",
		"shaderInt16",
		"shaderResourceResidency",
		"shaderResourceMinLod",
		"sparseBinding",
		"sparseResidencyBuffer",
		"sparseResidencyImage2D",
		"sparseResidencyImage3D",
		"sparseResidency2Samples",
		"sparseResidency4Samples",
		"sparseResidency8Samples",
		"sparseResidency16Samples",
		"sparseResidencyAliased",
		"variableMultisampleRate",
		"inheritedQueries"
	};

	if (index >= 55) [[unlikely]]
    {
        return "!VkPhysicalDeviceFeatures Name Outside Of Range!";
    }
	else [[likely]]
    {
        return featureNames[index];
    }
} // VkSetup::_GetVkPhysicalDeviceFeaturesName



