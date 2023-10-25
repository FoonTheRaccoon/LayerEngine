#pragma once
 
#include "EngUtils.h"
#include "Viewport.h"
#include "Vulkan/RenderPass.h"
#include "Vulkan/SwapChain.h"

class RenderManager
{
public:
	RenderManager();
	void Initilize(const char* appName, u32 winWidth, u32 winHeight);
	void Shutdown();
	bool WindowsShouldClose();
	void DrawFrame();

private:
	void RecordCommands(u32 currentImage);	
	void CreateSemaphoresAndFences();

private:
	Viewport m_Viewport = {};
	VkRef m_VkRef = {};

	u32 m_CurrentFrame = 0;

	SwapChain m_SwapChain = {};
	bool m_bSwapChainNeedsRebuild = false;

	RenderPass m_RenderPass = {};

	// Semaphores and Fences
	T_vector<VkSemaphore, MT_GRAPHICS> m_ImageAvailable = {};
	T_vector<VkSemaphore, MT_GRAPHICS> m_RenderFinished = {};
	T_vector<VkFence, MT_GRAPHICS> m_DrawFence = {};
};

