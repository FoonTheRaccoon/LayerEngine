#pragma once
#include "ThirdParty.h"
#include "LayerContainers.h"

// Forward Declares
struct VkRef;
struct SwapChainImage;

class SwapChain
{
public:
	SwapChain() = default;
	~SwapChain() = default;

	void CreateIntialSwapChain(VkRef& vkRef);
	void CreateSwapChain(VkRef& vkRef);
	void DestroySwapChain(const VkRef& vkRef);
	void CheckForUnMinimize(VkRef& vkRef);

	//Getters
	const u64 Size() const;
	const T_vector<SwapChainImage, MT_GRAPHICS> GetImages() const;
	VkSwapchainKHR GetHandle() { return m_SwapChain; }
	VkSwapchainKHR* GetPtr() { return &m_SwapChain; }
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

