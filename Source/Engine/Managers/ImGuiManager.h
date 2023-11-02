#pragma once
#include "ThirdParty.h"
#include "Broadcaster.h"

// Forward Declares
struct VkRef;
struct SwapChainImage;

namespace ImGuiManager
{
    // Broadcaster to register drawUI functions to be drawn during ImGuiManager::StartImguiFrame()
	inline Broadcaster<void()> imguiDrawBroadcaster;
    
    // Path to imgui config directory. Set by Editor/Game FileManager
	inline T_string imguiConfigDirPath = {};
    // Path to imgui config file (Path/to/imguiConfig.ini). Set by Editor/Game FileManager
    inline T_string imguiConfigFilePath = {};

	void SetupImgui(const VkRef& vkRef);
	void StartImguiFrame();
	void SubmitImGuiVulkanCommands(VkCommandBuffer cmdBuffer, u32 frameIndex);
	void EndImguiFrame();
	void ShutdownImgui(const VkRef& vkRef);
	void CreateImGuiFrameBuffer(const VkRef& vkRef, const T_vector<SwapChainImage, MT_GRAPHICS>& swapChainImages, VkExtent2D swapChainExtent);
 
 
	// Temp TODO: DELETE ME
	inline bool _bShowDemoWindow = true;
	inline glm::vec4 _clearColor = { 0.301f, 0.341f, 0.411f, 1.0f };
	inline ImVec4 _textColor = { 1.0f, 1.0f, 1.0f, 1.0f };
 
} // namespace ImGuiManager


#ifdef LAYER_EDITOR
    // Give a context object ('this' for members or 'nullptr' for globals that you don't plan to unregister) and a function that contains ImGui Begin/End to draw in the editor. This will get stripped out in game exe.
    #define REGISTER_EDITOR_UI_WINDOW(contextObj, func) ImGuiManager::imguiDrawBroadcaster.Register(contextObj, func);
#else // LAYER_EDITOR
    // Only used in Editor
    #define REGISTER_EDITOR_UI_WINDOW(contextObj, func)
#endif // LAYER_EDITOR




