#pragma once
#include "ThirdParty.h"
 
namespace VkConfig
{
	// -PHYSICAL DEVICE CONFIG-
	constexpr VkPhysicalDeviceFeatures desiredDeviceFeatures = {
		.robustBufferAccess							= VK_FALSE,
		.fullDrawIndexUint32						= VK_FALSE,
		.imageCubeArray								= VK_FALSE,
		.independentBlend							= VK_FALSE,
		.geometryShader								= VK_FALSE,
		.tessellationShader							= VK_FALSE,
		.sampleRateShading								= VK_TRUE,
		.dualSrcBlend								= VK_FALSE,
		.logicOp									= VK_FALSE,
		.multiDrawIndirect							= VK_FALSE,
		.drawIndirectFirstInstance					= VK_FALSE,
		.depthClamp									= VK_FALSE,
		.depthBiasClamp								= VK_FALSE,
		.fillModeNonSolid							= VK_FALSE,
		.depthBounds								= VK_FALSE,
		.wideLines									= VK_FALSE,
		.largePoints								= VK_FALSE,
		.alphaToOne									= VK_FALSE,
		.multiViewport								= VK_FALSE,
		.samplerAnisotropy								= VK_TRUE,
		.textureCompressionETC2						= VK_FALSE,
		.textureCompressionASTC_LDR					= VK_FALSE,
		.textureCompressionBC						= VK_FALSE,
		.occlusionQueryPrecise						= VK_FALSE,
		.pipelineStatisticsQuery					= VK_FALSE,
		.vertexPipelineStoresAndAtomics				= VK_FALSE,
		.fragmentStoresAndAtomics					= VK_FALSE,
		.shaderTessellationAndGeometryPointSize		= VK_FALSE,
		.shaderImageGatherExtended					= VK_FALSE,
		.shaderStorageImageExtendedFormats			= VK_FALSE,
		.shaderStorageImageMultisample				= VK_FALSE,
		.shaderStorageImageReadWithoutFormat		= VK_FALSE,
		.shaderStorageImageWriteWithoutFormat		= VK_FALSE,
		.shaderUniformBufferArrayDynamicIndexing	= VK_FALSE,
		.shaderSampledImageArrayDynamicIndexing		= VK_FALSE,
		.shaderStorageBufferArrayDynamicIndexing	= VK_FALSE,
		.shaderStorageImageArrayDynamicIndexing		= VK_FALSE,
		.shaderClipDistance							= VK_FALSE,
		.shaderCullDistance							= VK_FALSE,
		.shaderFloat64								= VK_FALSE,
		.shaderInt64								= VK_FALSE,
		.shaderInt16								= VK_FALSE,
		.shaderResourceResidency					= VK_FALSE,
		.shaderResourceMinLod						= VK_FALSE,
		.sparseBinding								= VK_FALSE,
		.sparseResidencyBuffer						= VK_FALSE,
		.sparseResidencyImage2D						= VK_FALSE,
		.sparseResidencyImage3D						= VK_FALSE,
		.sparseResidency2Samples					= VK_FALSE,
		.sparseResidency4Samples					= VK_FALSE,
		.sparseResidency8Samples					= VK_FALSE,
		.sparseResidency16Samples					= VK_FALSE,
		.sparseResidencyAliased						= VK_FALSE,
		.variableMultisampleRate					= VK_FALSE,
		.inheritedQueries							= VK_FALSE
	};

	constexpr std::array<const char*, 2> desiredDeviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME
	};

	// -SURFACE FORMATS-
	constexpr std::array<VkFormat, 4> desiredSurfaceFormats = {
		VK_FORMAT_R8G8B8A8_UNORM,
		VK_FORMAT_B8G8R8A8_UNORM,
		VK_FORMAT_R8G8B8A8_SRGB,
		VK_FORMAT_B8G8R8A8_SRGB
	};

	constexpr std::array<VkFormat, 2> desiredSurface10bitFormats = {
		VK_FORMAT_A2B10G10R10_UNORM_PACK32,
		VK_FORMAT_A2R10G10B10_UNORM_PACK32
	};

	// -RENDER-PASS/SWAPCHAIN COLOR FORMATS-
	constexpr std::array<VkFormat, 5> desiredColorAttachment32BitPackFormats = {							
		VK_FORMAT_R8G8B8A8_UNORM,																// Arc Not Supported
		VK_FORMAT_B8G8R8A8_UNORM,			// No STORAGE_IMAGE |								// Arc Not Supported
		VK_FORMAT_A8B8G8R8_UNORM_PACK32,														// Arc Not Supported
		// Arc
		VK_FORMAT_B8G8R8A8_SRGB,			
		VK_FORMAT_A8B8G8R8_SRGB_PACK32		// No STORAGE_IMAGE
	};

	constexpr std::array<VkFormat, 3> desiredColorAttachment16BitPackFormats = {
		VK_FORMAT_R5G5B5A1_UNORM_PACK16,	// No STORAGE_IMAGE
		VK_FORMAT_B5G5R5A1_UNORM_PACK16,	// No STORAGE_IMAGE | No COLOR BLEND (Intel int)	
		VK_FORMAT_A1R5G5B5_UNORM_PACK16		// No STORAGE_IMAGE									// Arc Not Supported
	};

	constexpr std::array<VkFormat, 2> desiredColorAttachment10BitColorFormats = {
		VK_FORMAT_A2R10G10B10_UNORM_PACK32,	// No STORAGE_IMAGE									// Arc Not Supported
		VK_FORMAT_A2B10G10R10_UNORM_PACK32														// Arc Not Supported
	};


	// -RENDER-PASS DEPTH STENCIL FORMATS-
	constexpr std::array<VkFormat, 2> desiredDepthStencilAttachmentFormats = {
		VK_FORMAT_D24_UNORM_S8_UINT,
		VK_FORMAT_D32_SFLOAT_S8_UINT
	};

}
