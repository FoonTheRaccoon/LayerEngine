#include "ThirdParty.h"
#include "VkTypes.h"
#include "MemoryTracker.h"
#include "LayerContainers.h"
#include "Broadcaster.h"

#ifndef IMGUI_MANAGER_H
#define IMGUI_MANAGER_H

namespace ImGuiManager
{
	inline Broadcaster<void()> imguiDrawBroadcaster;
	inline T_string imguiConfigFilePath = {};

	void SetupImgui(const VkRef& vkRef);
	void StartImguiFrame();
	void SubmitImGuiVulkanCommands(VkCommandBuffer cmdBuffer, u32 frameIndex);
	void EndImguiFrame();
	void ShutdownImgui(const VkRef& vkRef);
	void CreateImGuiFrameBuffer(const VkRef& vkRef, const T_vector<SwapChainImage, MT_GRAPHICS> swapChainImages, VkExtent2D swapChainExtent);

	// DockSpace and top menu bar manager
	void _DockSpaceManager(ImGuiIO& io);

	// Temp
	inline bool _bShowDemoWindow = true;
	inline glm::vec4 _clearColor = { 0.301f, 0.341f, 0.411f, 1.0f };
	inline ImVec4 _textColor = { 1.0f, 1.0f, 1.0f, 1.0f };

	// ImGui Vulkan members
	inline VkRenderPass _imguiRenderPass = {};
	inline T_vector<VkFramebuffer, MT_GRAPHICS> _imguiFrameBuffer = {};
	inline VkExtent2D _swapChainExtentRef;
	inline VkPipelineCache _imguiPipelineCache = {};
	inline VkDescriptorPool _imguiDescriptorPool = {};
	inline VkClearValue _imguiClearValue = {};

	// DockSpace members
	inline bool _bDockSpaceOpen = true;
	inline f32 _imguiWindowOpacity = 0.6f;
};

#ifdef LAYER_EDITOR
// Give a context object ('this' for members or 'nullptr' for globals that you don't plan to unregister) and a function that contains ImGui Begin/End to draw in the editor.
#define REGISTER_EDITOR_UI(contextObj, func) ImGuiManager::imguiDrawBroadcaster.Register(contextObj, func);
#else
#define REGISTER_EDITOR_UI(contextObj, func)
#endif

#endif // IMGUI_MANAGER_H

