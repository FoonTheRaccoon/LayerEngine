#include "RenderManager.h"
#include "VkSetup.h"


RenderManager::RenderManager()
{

}

void RenderManager::Initilize(const char* appName, u32 winWidth, u32 winHeight)
{
	LOG_DEBUG("Initilizing Render Manager...");

	m_Viewport.CreateViewport(appName, winWidth, winHeight);

	m_VkRef.pWindow = m_Viewport.GetWindow();

	VkSetup::CreateInstance(appName, m_VkRef);
	ValidationLayers::SetupDebugMessenger(m_VkRef.instance);
	VkSetup::CreateSurface(m_VkRef);
	VkSetup::CapturePhysicalDevice(m_VkRef);
	VkSetup::CreateLogicalDevice(m_VkRef);
	VkSetup::CreateVmaAllocator(m_VkRef);
	VkSetup::CreateCommandPools(m_VkRef);
	VkSetup::AllocateCommandBuffers(m_VkRef);

	ImGuiManager::SetupImgui(m_VkRef);

	m_SwapChain.CreateIntialSwapChain(m_VkRef);
	m_RenderPass.CreateRenderPass(m_VkRef, m_SwapChain);

	CreateSemaphoresAndFences();

	LOG_INFO("Render Manager Initilized");
}

void RenderManager::Shutdown()
{
	LOG_DEBUG("Shuting Down Render Manager...");

	// Wait till all GPU processes are done
	LOG_VKRESULT(vkDeviceWaitIdle(m_VkRef.logDevice));

	// Clean up in reverse order of initialization 
	for (u32 i = 0; i < m_VkRef.phyDevice.numInFlightFrames; i++)
	{
		vkDestroyFence(m_VkRef.logDevice, m_DrawFence[i], &m_VkRef.hostAllocator);
		vkDestroySemaphore(m_VkRef.logDevice, m_RenderFinished[i], &m_VkRef.hostAllocator);
		vkDestroySemaphore(m_VkRef.logDevice, m_ImageAvailable[i], &m_VkRef.hostAllocator);
	}

	ImGuiManager::ShutdownImgui(m_VkRef);

	m_RenderPass.DestroyRenderPass(m_VkRef);
	m_SwapChain.DestroySwapChain(m_VkRef);

	if (m_VkRef.bHasTransferCommandBuffer)
	{
		vkFreeCommandBuffers(m_VkRef.logDevice, m_VkRef.transferCommandPool, (u32)m_VkRef.transferCommandBuffers.size(), m_VkRef.transferCommandBuffers.data());
		vkDestroyCommandPool(m_VkRef.logDevice, m_VkRef.transferCommandPool, &m_VkRef.hostAllocator);
	}
	vkFreeCommandBuffers(m_VkRef.logDevice, m_VkRef.computeCommandPool, (u32)m_VkRef.computeCommandBuffers.size(), m_VkRef.computeCommandBuffers.data());
	vkDestroyCommandPool(m_VkRef.logDevice, m_VkRef.computeCommandPool, &m_VkRef.hostAllocator);
	vkFreeCommandBuffers(m_VkRef.logDevice, m_VkRef.graphicsCommandPool, (u32)m_VkRef.graphicsCommandBuffers.size(), m_VkRef.graphicsCommandBuffers.data());
	vkDestroyCommandPool(m_VkRef.logDevice, m_VkRef.graphicsCommandPool, &m_VkRef.hostAllocator);

	vmaDestroyAllocator(m_VkRef.vmaAllocator);
	vkDestroyDevice(m_VkRef.logDevice, &m_VkRef.hostAllocator);
	vkDestroySurfaceKHR(m_VkRef.instance, m_VkRef.surface, &m_VkRef.hostAllocator);
	ValidationLayers::DestroyDebugUtilsMessengerEXT(m_VkRef.instance, &m_VkRef.hostAllocator);
	vkDestroyInstance(m_VkRef.instance, &m_VkRef.hostAllocator);

	m_Viewport.DestroyViewport();

	LOG_INFO("Render Manager Shut Down");
}

bool RenderManager::WindowsShouldClose()
{
	if constexpr (GlobalConstants::bOnAndroid)
	{
		// TODO: Android implementation
		return false;
	}
	else // GLFW
	{
		return glfwWindowShouldClose(m_Viewport.GetWindow());
	}
}

void RenderManager::DrawFrame()
{
	// Rebuild swap chain if needed.
	if (m_bSwapChainNeedsRebuild)
	{
		LOG_VKRESULT(vkDeviceWaitIdle(m_VkRef.logDevice));
		m_SwapChain.CreateSwapChain(m_VkRef);
		m_bSwapChainNeedsRebuild = false;
	}

	// Start imgui frame and call all the UI related functions
	ImGuiManager::StartImguiFrame();

	// Check if the window has be minimized, then check if it has been unminimized and let imgui finish up.
	if (m_SwapChain.WindowIsMinimized())
	{
		m_SwapChain.CheckForUnMinimize(m_VkRef);
		ImGuiManager::EndImguiFrame();
		return;
	}

	// Wait for given fence to signal (open) from last draw before continuing. 
	vkWaitForFences(m_VkRef.logDevice, 1, &m_DrawFence[m_CurrentFrame], VK_TRUE, U64_MAX);
	// TODO: Add CPU synchronize code here to to run while waiting for GPU.
	// Manually reset (close) fences.
	vkResetFences(m_VkRef.logDevice, 1, &m_DrawFence[m_CurrentFrame]);

	// Get index to the next image that mSwapChainImages, mSwapChainFramebuffers, and mCommandBuffers can all sync with.
	// This also tells the semaphore we provide when the next image is ready to be drawn to.
	u32 nextImage;
	VkResult result = vkAcquireNextImageKHR(m_VkRef.logDevice, m_SwapChain.GetHandle(), U64_MAX, m_ImageAvailable[m_CurrentFrame], VK_NULL_HANDLE, &nextImage);
	// If the window has been resized then vkAcquireNextImageKHR will return VK_ERROR_OUT_OF_DATE_KHR or VK_SUBOPTIMAL_KHR in which case the swap chain needs to be rebuilt.
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
	{
		m_bSwapChainNeedsRebuild = true;
		ImGuiManager::EndImguiFrame();
		return;
	}
	else
	{
		LOG_VKRESULT(result);
	}


	RecordCommands(nextImage);
	//UpdateUniformBuffers(nextImage);		//Dynamic Buffer

	// Submit command buffer to render. We specify what semaphores the render pass should wait on and where, and what semaphores should be signaled when finished.
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount = 1;											// Number of semaphores to wait on
	submitInfo.pWaitSemaphores = &m_ImageAvailable[m_CurrentFrame];				// List of semaphores to wait on
	VkPipelineStageFlags waitStages[] = {										// List of stages (points in the render pass) where the render pass can freely run up to
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT							// before waiting on the provided wait semaphores to be changed.
	};																			// To clarify: Render pass starts without next image being ready -> Stops at bit provided
	submitInfo.pWaitDstStageMask = waitStages;									// in 'waitStages' -> Waits till wait semaphore in the same index changes -> repeats with next wait stage(s)/semaphore(s) until all are finished.
	submitInfo.commandBufferCount = 1;											// Number of command buffers
	submitInfo.pCommandBuffers = &m_VkRef.graphicsCommandBuffers[nextImage];	// List of command buffers to submit
	submitInfo.signalSemaphoreCount = 1;										// Number of semaphores to signal when command buffer finishes.
	submitInfo.pSignalSemaphores = &m_RenderFinished[m_CurrentFrame];			// List of semaphores to signal when command buffer finishes.

	LOG_VKRESULT(vkQueueSubmit(m_VkRef.queues.graphics, 1, &submitInfo, m_DrawFence[m_CurrentFrame]));

	// End imgui frame updating all the windows
	ImGuiManager::EndImguiFrame();

	//Present rendered image to screen
	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;									// Number of semaphores to wait on before presenting image to swap chain/surface
	presentInfo.pWaitSemaphores = &m_RenderFinished[m_CurrentFrame];	// List of semaphores to wait on before presenting image to swap chain/surface
	presentInfo.swapchainCount = 1;										// Number of swap chains/surfaces to present to 
	presentInfo.pSwapchains = m_SwapChain.GetPtr();						// List of swap chain(s)/surface(s) to present to
	presentInfo.pImageIndices = &nextImage;								// Index(s) of image(s) in swap chain to present to surface

	result = vkQueuePresentKHR(m_VkRef.queues.graphics, &presentInfo);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
	{
		m_bSwapChainNeedsRebuild = true;
		return;
	}
	else
	{
		LOG_VKRESULT(result);
	}

	// Get next frame (use % numInFlightFrames to keep value below numInFlightFrames)
	m_CurrentFrame = (m_CurrentFrame + 1) % m_VkRef.phyDevice.numInFlightFrames;
}

void RenderManager::RecordCommands(u32 currentImage)
{
	// Reset The command pool
	//vkResetCommandPool(m_VkRef.logDevice, m_VkRef.graphicsCommandPool, 0);

	// Info about how to begin each command buffer
	VkCommandBufferBeginInfo commandBufferBeginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
	commandBufferBeginInfo.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	// Start recording commands to command buffer
	vkBeginCommandBuffer(m_VkRef.graphicsCommandBuffers[currentImage], &commandBufferBeginInfo);

	// Allow imgui to submit any commands it needs to
	ImGuiManager::SubmitImGuiVulkanCommands(m_VkRef.graphicsCommandBuffers[currentImage], currentImage);

	// Stop recording commands to command buffer
	vkEndCommandBuffer(m_VkRef.graphicsCommandBuffers[currentImage]);
}

void RenderManager::CreateSemaphoresAndFences()
{
	LOG_DEBUG("Creating Semaphores And Fences...")

	//Allocate semaphore and fence vector sizes
	m_ImageAvailable.resize(m_VkRef.phyDevice.numInFlightFrames);
	m_RenderFinished.resize(m_VkRef.phyDevice.numInFlightFrames);
	m_DrawFence.resize(m_VkRef.phyDevice.numInFlightFrames);

	// Semaphore Creation info
	VkSemaphoreCreateInfo semaphoreCreateInfo = {};
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	// Fence Creation info
	VkFenceCreateInfo fenceCreateInfo = {};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;			// Default fence to be opened to avoid start of app hang.

	for (u32 i = 0; i < m_VkRef.phyDevice.numInFlightFrames; i++)
	{
		LOG_VKRESULT(vkCreateSemaphore(m_VkRef.logDevice, &semaphoreCreateInfo, &m_VkRef.hostAllocator, &m_ImageAvailable[i]));
		LOG_VKRESULT(vkCreateSemaphore(m_VkRef.logDevice, &semaphoreCreateInfo, &m_VkRef.hostAllocator, &m_RenderFinished[i]));
		LOG_VKRESULT(vkCreateFence(m_VkRef.logDevice, &fenceCreateInfo, &m_VkRef.hostAllocator, &m_DrawFence[i]));
	}

	LOG_INFO("Semaphores And Fences Created");
}


