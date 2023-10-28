#include "RenderPass.h"
#include "GpuMemoryTracker.h"
#include "VkTypes.h"
#include "SwapChain.h"
#include "VkBuffersAndImages.h"
#include "Logger.h"


void RenderPass::CreateRenderPass(const VkRef& vkRef, const SwapChain& swapChain)
{
	LOG_DEBUG("Creating Vulkan Render Pass...");

	CreateAttachmentImageBuffers(vkRef, swapChain);

	// Amount of subpasses the render pass is using.
	std::array<VkSubpassDescription, 2> subpasses = {};

	// Render pass attachments. These describe what is attached to the pipeline during the render pass. 
	// These include Color, depth/stencil, and Input (Render target) attachments.
	// Attachments represent your data/link to the pipeline from your shaders. (e.g. a shader var at location = 0 would reference the attachmentReference[0])

	// --SUBPASS 1--
	// -Subpass 1 Attachments + References (Input attachments)

	// Color attachment (Input)
	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = vkRef.phyDevice.preferred32BitPackColorAttachmentFormat;	// Data format the attachment uses 
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;									// Number of sample to write for MSAA (TODO: Change when implementing MSAA)
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;								// Describes what to do with attachment before render pass (Clear/Load(Reuse)/DontCare(UB))
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;							// Describes what to do with attachment after render pass (Store/DontCare(UB))
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;					// Describes what to do with stencil before render pass
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;					// Describes what to do with stencil after render pass
	// Framebuffer data will be stored as an image, but images can be given different data layouts that are optimal for certain operations.
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;						// Image Data layout before render pass starts
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;			// Image Data layout after render pass (to change to)

	// Depth attachment (Input)
	VkAttachmentDescription depthAttachment = {};
	depthAttachment.format = vkRef.phyDevice.preferredDepthStencilAttachmentFormat;
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	// Attachment reference is the handle a subpass can use to refer to an attachment. The Attachment number it hold is the index of the attachment it's referencing in your framebuffer.
	// The 'attachment' value of a reference is also the location value of any variable set in your shaders.
	// Color attachment (Input) reference
	VkAttachmentReference colorAttachmentReference = {};
	colorAttachmentReference.attachment = 1;									// Index to attachment in your framebuffer that is passed to the render pass
	colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;	// Layout of the image while in the subpass.

	// Depth attachment (Input) reference
	VkAttachmentReference depthAttachmentReference = {};
	depthAttachmentReference.attachment = 2;
	depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	// Set up subpass 1
	subpasses[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;	// See subpass 2 for info
	subpasses[0].colorAttachmentCount = 1;
	subpasses[0].pColorAttachments = &colorAttachmentReference;
	subpasses[0].pDepthStencilAttachment = &depthAttachmentReference;

	// --SUBPASS 2--
	// -Subpass 2 Attachments + References (Input attachments)

	// Swap Chain Color attachment
	VkAttachmentDescription swapchainColorAttachment = {};
	swapchainColorAttachment.format = vkRef.phyDevice.preferredSurfaceFormat.format;
	swapchainColorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	swapchainColorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	swapchainColorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	swapchainColorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	swapchainColorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	swapchainColorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	swapchainColorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	// Subpass 2 attachment references
	VkAttachmentReference swapchainColorAttachmentReference = {};
	swapchainColorAttachmentReference.attachment = 0;									// 0 since the swapchain image view is in framebuffer index 0.
	swapchainColorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	// Array of attachment references for the inputs to subpass 2 from subpass 1
	std::array<VkAttachmentReference, 2> inputRefrences = {};
	// Subpass 1 output 1/Subpass 2 input 1 (Color)
	inputRefrences[0].attachment = 1;				// Index in the framebuffer when the attachment from subpass 1 was placed
	inputRefrences[0].layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	// Subpass 1 output 2/Subpass 2 input 2 (Depth)
	inputRefrences[1].attachment = 2;				// Index in the framebuffer when the attachment from subpass 1 was placed
	inputRefrences[1].layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	// Set up subpass 2
	subpasses[1].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;	// Pipeline type subpass is to be bound/linked to
	subpasses[1].colorAttachmentCount = 1;												// Number of color attachments used by subpass
	subpasses[1].pColorAttachments = &swapchainColorAttachmentReference;				// List of color attachments used by subpass
	subpasses[1].pDepthStencilAttachment = VK_NULL_HANDLE;								// Depth stencil (only 1) used by subpass in raster phase
	subpasses[1].inputAttachmentCount = static_cast<uint32_t>(inputRefrences.size());	// Number of input attachments used by subpass from previous subpasses
	subpasses[1].pInputAttachments = inputRefrences.data();								// List of input attachments used by subpass from previous subpasses
	subpasses[1].preserveAttachmentCount = 0;											// Number of attachments that you want to preserve that aren't used by this subpass (You must pass them along to later subpasses that will use them)
	subpasses[1].pPreserveAttachments = VK_NULL_HANDLE;									// List of attachments that you want to preserve that aren't used by this subpass
	subpasses[1].pResolveAttachments = VK_NULL_HANDLE;									// List of attachments that you want to resolve (defined by VkSubpassDescriptionDepthStencilResolve struct in pNext chain of VkSubpassDescription2)

	// --SUBPASS DEPENDENCIES--
	// Subpass dependencies describes when the implicit layout transition between subpasses occurs. We do not call these transitions explicitly.
	std::array<VkSubpassDependency, 3> subpassDependencies;
	// Conversion from VK_IMAGE_LAYOUT_UNDEFINED (Initial layout of color attachment) to VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL (Subpass 1 format)
	// Transition must happen after...
	subpassDependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;					// What subpass (index) this subpass is pulling from (if first(src)/last(dst) subpass in render pass then this will be VK_SUBPASS_EXTERNAL)
	subpassDependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;	// Which stage of the pipeline needs to happen before this transition takes place.
	subpassDependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;			// Stage access mask (memory access) Refer to VkAccessFlagBits web page to see which stage uses which bits.
	// But must happen before...
	subpassDependencies[0].dstSubpass = 0;
	subpassDependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpassDependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	subpassDependencies[0].dependencyFlags = 0;

	// Subpass 1 (color/depth) to Subpass 2 layout (shader read)
	// Transition must happen after...
	subpassDependencies[1].srcSubpass = 0;
	subpassDependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpassDependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	// But must happen before...
	subpassDependencies[1].dstSubpass = 1;
	subpassDependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	subpassDependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	subpassDependencies[1].dependencyFlags = 0;

	// Conversion from VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL (Subpass 1 format) to VK_IMAGE_LAYOUT_PRESENT_SRC_KHR (Attachment post subpass 2 format)
	// Transition must happen after...
	subpassDependencies[2].srcSubpass = 1;										// The dst setting of one dependency should match the src settings of the next if they are adjacent and deal with the same data.
	subpassDependencies[2].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpassDependencies[2].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	// But must happen before...
	subpassDependencies[2].dstSubpass = VK_SUBPASS_EXTERNAL;
	subpassDependencies[2].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	subpassDependencies[2].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	subpassDependencies[2].dependencyFlags = 0;

	// --CREATE RENDER PASS--
	// Create and array of all render attachments our render pass will use, and list them in the same order the show up in the framebuffer
	std::array<VkAttachmentDescription, 3> renderPassAttachments = { swapchainColorAttachment, colorAttachment, depthAttachment };

	// Render pass create info
	VkRenderPassCreateInfo renderPassCreateInfo = {};
	renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(renderPassAttachments.size());
	renderPassCreateInfo.pAttachments = renderPassAttachments.data();
	renderPassCreateInfo.subpassCount = static_cast<uint32_t>(subpasses.size());
	renderPassCreateInfo.pSubpasses = subpasses.data();
	renderPassCreateInfo.dependencyCount = static_cast<uint32_t>(subpassDependencies.size());
	renderPassCreateInfo.pDependencies = subpassDependencies.data();

	LOG_VKRESULT(vkCreateRenderPass(vkRef.logDevice, &renderPassCreateInfo, &vkRef.hostAllocator, &m_RenderPass));

	CreateFrameBuffers(vkRef, swapChain);

	LOG_INFO("Created Vulkan Render Pass");
}


void RenderPass::DestroyRenderPass(const VkRef& vkRef)
{
	LOG_DEBUG("Destroying Vulkan Render Pass...")

	DestroyAttachmentImageBuffers(vkRef);
	DestroyFrameBuffers(vkRef);
	vkDestroyRenderPass(vkRef.logDevice, m_RenderPass, &vkRef.hostAllocator);

	LOG_INFO("Destroyed Vulkan Render Pass");
}


const VkFramebuffer RenderPass::GetFrameBuffer(u32 index) const
{
	if (index <= (m_FrameBuffers.size() - 1)) [[likely]]
	{
		return m_FrameBuffers[index];
	}
	else [[unlikely]]
	{
		LOG_WARNING(T_string("Trying to access invalid frame buffer index! Given index: ", std::to_string(index)));
		return m_FrameBuffers[0];
	}
}


void RenderPass::CreateAttachmentImageBuffers(const VkRef& vkRef, const SwapChain& swapChain)
{
	// Preallocate sizes
	m_ColorImages.resize(swapChain.Size());

	m_DepthImages.resize(swapChain.Size());

	const VkExtent2D swapChainExtent = swapChain.Extent();

	for (size_t i = 0; i < m_ColorImages.size(); i++)
	{
		// Create color buffer image
		m_ColorImages[i] = VkImageHelpers::Create2DImage(
			vkRef,
			swapChainExtent, 
			vkRef.phyDevice.preferred32BitPackColorAttachmentFormat,
			VK_IMAGE_TILING_OPTIMAL, 
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT,
			VK_IMAGE_ASPECT_COLOR_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
			GPU_USAGE_ATTACHMENT_IMAGE
		);
	}

	for (size_t i = 0; i < m_DepthImages.size(); i++)
	{
		// Create color buffer image
		m_DepthImages[i] = VkImageHelpers::Create2DImage(
			vkRef,
			swapChainExtent,
			vkRef.phyDevice.preferredDepthStencilAttachmentFormat,
			VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT,
			VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			GPU_USAGE_ATTACHMENT_IMAGE
		);
	}
}


void RenderPass::CreateFrameBuffers(const VkRef& vkRef, const SwapChain& swapChain)
{
	LOG_DEBUG("Creating Vulkan Framebuffers...")

	// Preallocate size
	m_FrameBuffers.resize(swapChain.Size());
	// Create framebuffer for each swap chain image and each render pass attachment 

	const VkExtent2D swapChainExtent = swapChain.Extent();

	for (size_t i = 0; i < m_FrameBuffers.size(); i++)
	{
		// Add attachments made in the render pass in the same order
		std::array<VkImageView, 3> attachments = {
			swapChain.GetImages()[i].imageView,
			m_ColorImages[i].imageView,
			m_DepthImages[i].imageView
		};

		VkFramebufferCreateInfo framebufferCreateInfo = {};
		framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCreateInfo.renderPass = m_RenderPass;								// Render pass layout the framebuffer will be used with
		framebufferCreateInfo.attachmentCount = static_cast<u32>(attachments.size());	// Number of attachments you made in the render pass
		framebufferCreateInfo.pAttachments = attachments.data();						// List of attachments you made in the render pass (1:1)
		framebufferCreateInfo.width = swapChainExtent.width;							// Framebuffer width
		framebufferCreateInfo.height = swapChainExtent.height;							// Framebuffer height
		framebufferCreateInfo.layers = 1;												// Framebuffer layers (number designated in swap chain creation)

		LOG_VKRESULT(vkCreateFramebuffer(vkRef.logDevice, &framebufferCreateInfo, &vkRef.hostAllocator, &m_FrameBuffers[i]));
	}

	LOG_INFO("Created Vulkan Framebuffers");
}


void RenderPass::DestroyAttachmentImageBuffers(const VkRef& vkRef)
{
	for (size_t i = 0; i < m_ColorImages.size(); i++)
	{
		VkImageHelpers::DestroyImage(vkRef, m_ColorImages[i]);
	}

	for (size_t i = 0; i < m_DepthImages.size(); i++)
	{
		VkImageHelpers::DestroyImage(vkRef, m_DepthImages[i]);
	}
}


void RenderPass::DestroyFrameBuffers(const VkRef& vkRef)
{
	for (auto& frameBuffer : m_FrameBuffers)
	{
		vkDestroyFramebuffer(vkRef.logDevice, frameBuffer, &vkRef.hostAllocator);
	}
}
