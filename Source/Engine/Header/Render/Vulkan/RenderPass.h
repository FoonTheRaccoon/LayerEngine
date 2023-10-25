#pragma once
#include "EngUtils.h"
#include "SwapChain.h"
#include "VkBuffersAndImages.h"

class RenderPass
{
public:
	void CreateRenderPass(const VkRef& vkRef, const SwapChain& swapChain);

	void DestroyRenderPass(const VkRef& vkRef);

	const VkRenderPass GetRenderPass() const { return m_RenderPass; }
	const VkFramebuffer GetFrameBuffer(u32 index) const;

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

