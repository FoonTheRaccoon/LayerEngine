#include "RenderManager.h"
#include "VkSetup.h"
#include "Viewport.h"
#include "RenderPass.h"
#include "SwapChain.h"
#include "Logger.h"
#include "ImGuiManager.h"
#include "VkTypes.h"
#include "LoggingCallbacks.h"
#include "LayerContainers.h"

namespace RenderManager
{
	Viewport _Viewport = {};
	VkRef _VkRef = {};

	u32 _CurrentFrame = 0;

	SwapChain _SwapChain = {};
	bool _bSwapChainNeedsRebuild = false;

	RenderPass m_RenderPass = {};

	// Semaphores (GPU sync) and Fences (GPU->CPU sync)
	T_vector<VkSemaphore, MT_GRAPHICS> _ImageAvailable = {};
	T_vector<VkSemaphore, MT_GRAPHICS> _RenderFinished = {};
	T_vector<VkFence, MT_GRAPHICS> _DrawFence = {};

	// -- Internal Helpers --

	// Start cmd buffer, write to it, and end it
	void _RecordCommands(u32 currentImage);

	// Creates vulkan GPU sync Semaphores and GPU->CPU sync Fences
	void _CreateSemaphoresAndFences();
}


void RenderManager::Initialize(const char* appName, u32 winWidth, u32 winHeight)
{
	LOG_DEBUG("Initializing Render Manager...")

	_Viewport.CreateViewport(appName, winWidth, winHeight);

	_VkRef.pWindow = _Viewport.GetWindow();

	VkSetup::CreateInstance(appName, _VkRef);
	ValidationLayers::SetupDebugMessenger(_VkRef.instance);
	VkSetup::CreateSurface(_VkRef);
	VkSetup::CapturePhysicalDevice(_VkRef);
	VkSetup::CreateLogicalDevice(_VkRef);
	VkSetup::CreateVmaAllocator(_VkRef);
	VkSetup::CreateCommandPools(_VkRef);
	VkSetup::AllocateCommandBuffers(_VkRef);
 
	ImGuiManager::SetupImgui(_VkRef);
    
    
    _SwapChain.CreateInitialSwapChain(_VkRef);
	m_RenderPass.CreateRenderPass(_VkRef, _SwapChain);

	_CreateSemaphoresAndFences();

	LOG_INFO("Render Manager Initialized")
}

void RenderManager::Shutdown()
{
	LOG_DEBUG("Shutting Down Render Manager...")

	// Wait till all GPU processes are done
	LOG_VKRESULT(vkDeviceWaitIdle(_VkRef.logDevice))

	// Clean up in reverse order of initialization 
	for (u32 i = 0; i < _VkRef.phyDevice.numInFlightFrames; i++)
	{
		vkDestroyFence(_VkRef.logDevice, _DrawFence[i], &_VkRef.hostAllocator);
		vkDestroySemaphore(_VkRef.logDevice, _RenderFinished[i], &_VkRef.hostAllocator);
		vkDestroySemaphore(_VkRef.logDevice, _ImageAvailable[i], &_VkRef.hostAllocator);
	}

	ImGuiManager::ShutdownImgui(_VkRef);

	m_RenderPass.DestroyRenderPass(_VkRef);
	_SwapChain.DestroySwapChain(_VkRef);

	if (_VkRef.bHasTransferCommandBuffer)
	{
		vkFreeCommandBuffers(_VkRef.logDevice, _VkRef.transferCommandPool, (u32)_VkRef.transferCommandBuffers.size(), _VkRef.transferCommandBuffers.data());
		vkDestroyCommandPool(_VkRef.logDevice, _VkRef.transferCommandPool, &_VkRef.hostAllocator);
	}
	vkFreeCommandBuffers(_VkRef.logDevice, _VkRef.computeCommandPool, (u32)_VkRef.computeCommandBuffers.size(), _VkRef.computeCommandBuffers.data());
	vkDestroyCommandPool(_VkRef.logDevice, _VkRef.computeCommandPool, &_VkRef.hostAllocator);
	vkFreeCommandBuffers(_VkRef.logDevice, _VkRef.graphicsCommandPool, (u32)_VkRef.graphicsCommandBuffers.size(), _VkRef.graphicsCommandBuffers.data());
	vkDestroyCommandPool(_VkRef.logDevice, _VkRef.graphicsCommandPool, &_VkRef.hostAllocator);

	vmaDestroyAllocator(_VkRef.vmaAllocator);
	vkDestroyDevice(_VkRef.logDevice, &_VkRef.hostAllocator);
	vkDestroySurfaceKHR(_VkRef.instance, _VkRef.surface, &_VkRef.hostAllocator);
	ValidationLayers::DestroyDebugUtilsMessengerEXT(_VkRef.instance, &_VkRef.hostAllocator);
	vkDestroyInstance(_VkRef.instance, &_VkRef.hostAllocator);

	_Viewport.DestroyViewport();

	LOG_INFO("Render Manager Shut Down")
}

bool RenderManager::WindowsShouldClose()
{
    #if LAYER_PLATFORM_ANDROID
		// TODO: Android implementation
		return false;
    #else // GLFW
		return glfwWindowShouldClose(_Viewport.GetWindow());
    #endif
}

void RenderManager::DrawFrame()
{
	// Rebuild swap chain if needed.
	if (_bSwapChainNeedsRebuild)
	{
		LOG_VKRESULT(vkDeviceWaitIdle(_VkRef.logDevice))
		_SwapChain.CreateSwapChain(_VkRef);
		_bSwapChainNeedsRebuild = false;
	}

	// Start imgui frame and call all the UI related functions
	ImGuiManager::StartImguiFrame();

	// Check if the window has be minimized, then check if it has been un-minimized and let imgui finish up.
	if (_SwapChain.WindowIsMinimized())
	{
		_SwapChain.CheckForUnMinimize(_VkRef);
		ImGuiManager::EndImguiFrame();
		return;
	}

	// Wait for given fence to signal (open) from last draw before continuing. 
	vkWaitForFences(_VkRef.logDevice, 1, &_DrawFence[_CurrentFrame], VK_TRUE, U64_MAX);
	// TODO: Add CPU synchronize code here to to run while waiting for GPU.
	// Manually reset (close) fences.
	vkResetFences(_VkRef.logDevice, 1, &_DrawFence[_CurrentFrame]);

	// Get index to the next image that mSwapChainImages, mSwapChainFramebuffers, and mCommandBuffers can all sync with.
	// This also tells the semaphore we provide when the next image is ready to be drawn to.
	u32 nextImage;
	VkResult result = vkAcquireNextImageKHR(_VkRef.logDevice, _SwapChain.GetHandle(), U64_MAX, _ImageAvailable[_CurrentFrame], VK_NULL_HANDLE, &nextImage);
	// If the window has been resized then vkAcquireNextImageKHR will return VK_ERROR_OUT_OF_DATE_KHR or VK_SUBOPTIMAL_KHR in which case the swap chain needs to be rebuilt.
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
	{
		_bSwapChainNeedsRebuild = true;
		ImGuiManager::EndImguiFrame();
		return;
	}
	else
	{
		LOG_VKRESULT(result)
	}


	_RecordCommands(nextImage);
	//UpdateUniformBuffers(nextImage);		//Dynamic Buffer

	// Submit command buffer to render. We specify what semaphores the render pass should wait on and where, and what semaphores should be signaled when finished.
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount = 1;											// Number of semaphores to wait on
	submitInfo.pWaitSemaphores = &_ImageAvailable[_CurrentFrame];				// List of semaphores to wait on
	VkPipelineStageFlags waitStages[] = {										// List of stages (points in the render pass) where the render pass can freely run up to
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT							// before waiting on the provided wait semaphores to be changed.
	};																			// To clarify: Render pass starts without next image being ready -> Stops at bit provided
	submitInfo.pWaitDstStageMask = waitStages;									// in 'waitStages' -> Waits till wait semaphore in the same index changes -> repeats with next wait stage(s)/semaphore(s) until all are finished.
	submitInfo.commandBufferCount = 1;											// Number of command buffers
	submitInfo.pCommandBuffers = &_VkRef.graphicsCommandBuffers[nextImage];	// List of command buffers to submit
	submitInfo.signalSemaphoreCount = 1;										// Number of semaphores to signal when command buffer finishes.
	submitInfo.pSignalSemaphores = &_RenderFinished[_CurrentFrame];			// List of semaphores to signal when command buffer finishes.

	LOG_VKRESULT(vkQueueSubmit(_VkRef.queues.graphics, 1, &submitInfo, _DrawFence[_CurrentFrame]))

	// End imgui frame updating all the windows
	ImGuiManager::EndImguiFrame();

	//Present rendered image to screen
	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;									// Number of semaphores to wait on before presenting image to swap chain/surface
	presentInfo.pWaitSemaphores = &_RenderFinished[_CurrentFrame];	// List of semaphores to wait on before presenting image to swap chain/surface
	presentInfo.swapchainCount = 1;										// Number of swap chains/surfaces to present to 
	presentInfo.pSwapchains = _SwapChain.GetPtr();						// List of swap chain(s)/surface(s) to present to
	presentInfo.pImageIndices = &nextImage;								// Index(s) of image(s) in swap chain to present to surface

	result = vkQueuePresentKHR(_VkRef.queues.graphics, &presentInfo);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
	{
		_bSwapChainNeedsRebuild = true;
		return;
	}
	else
	{
		LOG_VKRESULT(result)
	}

	// Get next frame (use % numInFlightFrames to keep value below numInFlightFrames)
	_CurrentFrame = (_CurrentFrame + 1) % _VkRef.phyDevice.numInFlightFrames;
}

void RenderManager::_RecordCommands(u32 currentImage)
{
	// Reset The command pool
	// vkResetCommandPool(_VkRef.logDevice, _VkRef.graphicsCommandPool, 0);

	// Info about how to begin each command buffer
	VkCommandBufferBeginInfo commandBufferBeginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
	commandBufferBeginInfo.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	// Start recording commands to command buffer
	vkBeginCommandBuffer(_VkRef.graphicsCommandBuffers[currentImage], &commandBufferBeginInfo);

	// Allow imgui to submit any commands it needs to
	ImGuiManager::SubmitImGuiVulkanCommands(_VkRef.graphicsCommandBuffers[currentImage], currentImage);

	// Stop recording commands to command buffer
	vkEndCommandBuffer(_VkRef.graphicsCommandBuffers[currentImage]);
}

void RenderManager::_CreateSemaphoresAndFences()
{
	LOG_DEBUG("Creating Semaphores And Fences...")

	//Allocate semaphore and fence vector sizes
	_ImageAvailable.resize(_VkRef.phyDevice.numInFlightFrames);
	_RenderFinished.resize(_VkRef.phyDevice.numInFlightFrames);
	_DrawFence.resize(_VkRef.phyDevice.numInFlightFrames);

	// Semaphore Creation info
	VkSemaphoreCreateInfo semaphoreCreateInfo = {};
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	// Fence Creation info
	VkFenceCreateInfo fenceCreateInfo = {};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;			// Default fence to be opened to avoid start of app hang.

	for (u32 i = 0; i < _VkRef.phyDevice.numInFlightFrames; i++)
	{
		LOG_VKRESULT(vkCreateSemaphore(_VkRef.logDevice, &semaphoreCreateInfo, &_VkRef.hostAllocator, &_ImageAvailable[i]))
		LOG_VKRESULT(vkCreateSemaphore(_VkRef.logDevice, &semaphoreCreateInfo, &_VkRef.hostAllocator, &_RenderFinished[i]))
		LOG_VKRESULT(vkCreateFence(_VkRef.logDevice, &fenceCreateInfo, &_VkRef.hostAllocator, &_DrawFence[i]))
	}

	LOG_INFO("Semaphores And Fences Created")
}


