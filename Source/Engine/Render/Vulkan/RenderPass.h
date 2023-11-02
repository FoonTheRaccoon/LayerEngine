#pragma once
#include "ThirdParty.h"
#include "LayerContainers.h"
#include "GpuMemoryTracker.h"

// Forward Declares
struct VkRef;
class SwapChain;

class RenderPass
{
public:
	RenderPass() = default;
	~RenderPass() = default;

	void CreateRenderPass(const VkRef& vkRef, const SwapChain& swapChain);
	void DestroyRenderPass(const VkRef& vkRef);

	[[nodiscard]] VkRenderPass GetRenderPass() const { return m_RenderPass; }
	[[nodiscard]] VkFramebuffer GetFrameBuffer(u32 index) const;

private:
	void CreateAttachmentImageBuffers(const VkRef& vkRef, const SwapChain& swapChain);
	void CreateFrameBuffers(const VkRef& vkRef, const SwapChain& swapChain);

	void DestroyAttachmentImageBuffers(const VkRef& vkRef);
	void DestroyFrameBuffers(const VkRef& vkRef);

private:
	VkRenderPass m_RenderPass = {};
 
	T_vector<GpuImage, MT_GRAPHICS> m_ColorImages;

	T_vector<GpuImage, MT_GRAPHICS> m_DepthImages;

	T_vector<VkFramebuffer, MT_GRAPHICS> m_FrameBuffers;
};

