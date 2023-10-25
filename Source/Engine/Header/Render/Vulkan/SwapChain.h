#pragma once
#include "EngUtils.h"


class SwapChain
{
public:
	SwapChain();
	void CreateIntialSwapChain(VkRef& vkRef);
	void CreateSwapChain(VkRef& vkRef);
	void DestroySwapChain(const VkRef& vkRef);
	void CheckForUnMinimize(VkRef& vkRef);

	//Getters
	VkSwapchainKHR GetHandle() { return m_SwapChain; }
	VkSwapchainKHR* GetPtr() { return &m_SwapChain; }
	const u64 Size() const { return m_SwapChainImages.size(); } 
	const T_vector<SwapChainImage, MT_GRAPHICS> GetImages() const { return m_SwapChainImages; }
	const VkExtent2D Extent() const { return m_SwapChainExtent; }
	const bool WindowIsMinimized() const { return m_bWindowMinimized; }

private:
	VkExtent2D ChooseImageExtent(VkRef& vkRef);

private:
	VkSwapchainKHR m_SwapChain = VK_NULL_HANDLE;

	VkFormat m_SwapChainFormat = {};
	VkExtent2D m_SwapChainExtent = {};

	T_vector<SwapChainImage, MT_GRAPHICS> m_SwapChainImages = {};

	bool m_bWindowMinimized = false;
};

