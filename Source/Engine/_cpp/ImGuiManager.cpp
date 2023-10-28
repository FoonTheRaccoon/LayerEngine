#include "ImGuiManager.h"
#include "Logger.h"
#include "LoggingCallbacks.h"
#include "FileHelper.h"

void ImGuiManager::SetupImgui(const VkRef& vkRef)
{
	LOG_DEBUG("Setting Up ImGui...");

	// Create Render Pass For Just ImGui
	VkAttachmentDescription attachment = {};
	attachment.format = vkRef.phyDevice.preferredSurfaceFormat.format;
	attachment.samples = VK_SAMPLE_COUNT_1_BIT;
	attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	VkAttachmentReference color_attachment = {};
	color_attachment.attachment = 0;
	color_attachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &color_attachment;
	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	VkRenderPassCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	info.attachmentCount = 1;
	info.pAttachments = &attachment;
	info.subpassCount = 1;
	info.pSubpasses = &subpass;
	info.dependencyCount = 1;
	info.pDependencies = &dependency;
	LOG_VKRESULT(vkCreateRenderPass(vkRef.logDevice, &info, &vkRef.hostAllocator, &_imguiRenderPass));

	// _imguiFrameBuffer will be set up by the main swap chain object

	// Create Descriptor Pool
	std::array<VkDescriptorPoolSize, 1> poolSizes =
	{
		VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1 }
	};
	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	poolInfo.maxSets = 1;
	poolInfo.poolSizeCount = static_cast<u32>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	LOG_VKRESULT(vkCreateDescriptorPool(vkRef.logDevice, &poolInfo, &vkRef.hostAllocator, &ImGuiManager::_imguiDescriptorPool));

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION(); 
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
	//io.ConfigViewportsNoAutoMerge = true;
	//io.ConfigViewportsNoTaskBarIcon = true;

	// Set config location
	if (imguiConfigFilePath[0] == '\0')
	{
		LOG_ERROR("ImGui config file hasn't be set!");
	}
	else
	{
		FileHelper::CreateFolderIfAbsent(imguiConfigFilePath.c_str(), false);
		imguiConfigFilePath.append("GuiLayout.ini");
		io.IniFilename = imguiConfigFilePath.c_str();
	}


	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.FrameRounding = 2.0f;
		style.GrabRounding = 2.0f;
		style.Colors[ImGuiCol_WindowBg].w = _imguiWindowOpacity;
	}

	// Setup Platform/Renderer back ends
	ImGui_ImplGlfw_InitForVulkan(vkRef.pWindow, true);
	ImGui_ImplVulkan_InitInfo initInfo = {};
	initInfo.Instance = vkRef.instance;
	initInfo.PhysicalDevice = vkRef.phyDevice.handle;
	initInfo.Device = vkRef.logDevice;
	initInfo.QueueFamily = vkRef.phyDevice.graphicsQueueIndex;
	initInfo.Queue = vkRef.queues.graphics;
	initInfo.PipelineCache = _imguiPipelineCache;
	initInfo.DescriptorPool = _imguiDescriptorPool;
	initInfo.Subpass = 0;
	initInfo.MinImageCount = vkRef.phyDevice.swapChainBufferCount;
	initInfo.ImageCount = vkRef.phyDevice.swapChainBufferCount;
	initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
	initInfo.Allocator = &vkRef.hostAllocator;
	initInfo.CheckVkResultFn = LoggingCallbacks::ImguiCheckVkResult;
	ImGui_ImplVulkan_Init(&initInfo, _imguiRenderPass);

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	// - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
	// - Read 'docs/FONTS.md' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
	//IM_ASSERT(font != nullptr);

	// TODO: Upload custom fonts
	// Upload Fonts
	{
		// Use any command queue

		LOG_VKRESULT(vkResetCommandPool(vkRef.logDevice, vkRef.graphicsCommandPool, 0));
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		LOG_VKRESULT(vkBeginCommandBuffer(vkRef.graphicsCommandBuffers[0], &beginInfo));

		ImGui_ImplVulkan_CreateFontsTexture(vkRef.graphicsCommandBuffers[0]);

		VkSubmitInfo endInfo = {};
		endInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		endInfo.commandBufferCount = 1;
		endInfo.pCommandBuffers = &vkRef.graphicsCommandBuffers[0];
		LOG_VKRESULT(vkEndCommandBuffer(vkRef.graphicsCommandBuffers[0]));
		LOG_VKRESULT(vkQueueSubmit(vkRef.queues.graphics, 1, &endInfo, VK_NULL_HANDLE));

		LOG_VKRESULT(vkDeviceWaitIdle(vkRef.logDevice));
		ImGui_ImplVulkan_DestroyFontUploadObjects();
	}


	LOG_INFO("ImGui Setup Finished");
}

void ImGuiManager::StartImguiFrame()
{
	// Start the Dear ImGui frame
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();


	ImGuiIO& io = ImGui::GetIO();
	ImGuiStyle& style = ImGui::GetStyle();
	{
		_DockSpaceManager(io);

		imguiDrawBroadcaster.Broadcast();

		if (_bShowDemoWindow)
			ImGui::ShowDemoWindow(&_bShowDemoWindow);

		ImGui::Begin("Hello, world!");
		ImGui::Checkbox("Open Demo Window", &_bShowDemoWindow);
		ImGui::ColorEdit3("Clear Color", (float*)&_clearColor);
		ImGui::ColorEdit3("Text Color", (float*)&_textColor);
		ImGui::End();


	}

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.Colors[ImGuiCol_WindowBg].w = _imguiWindowOpacity;
		style.Colors[ImGuiCol_Text] = _textColor;
	}

	// Rendering
	ImGui::Render();
}

void ImGuiManager::SubmitImGuiVulkanCommands(VkCommandBuffer cmdBuffer, u32 frameIndex)
{
	_imguiClearValue.color = { {_clearColor.x, _clearColor.y, _clearColor.z, 1.0f} };

	// Command Buffer must be started 
	VkRenderPassBeginInfo imguiRenderPassBeginInfo = {};
	imguiRenderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	imguiRenderPassBeginInfo.renderPass = _imguiRenderPass;
	imguiRenderPassBeginInfo.framebuffer = _imguiFrameBuffer[frameIndex];
	imguiRenderPassBeginInfo.renderArea.extent.width = _swapChainExtentRef.width;
	imguiRenderPassBeginInfo.renderArea.extent.height = _swapChainExtentRef.height;
	imguiRenderPassBeginInfo.clearValueCount = 1;
	imguiRenderPassBeginInfo.pClearValues = &_imguiClearValue;
	vkCmdBeginRenderPass(cmdBuffer, &imguiRenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

	// Record dear imgui primitives into command buffer
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmdBuffer);

	// Submit command buffer
	vkCmdEndRenderPass(cmdBuffer);
}

void ImGuiManager::EndImguiFrame()
{
	// Update and Render additional Platform Windows
	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}

void ImGuiManager::ShutdownImgui(const VkRef& vkRef)
{
	LOG_DEBUG("Shutting Down ImGui...");

	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	for (u32 i = 0; i < _imguiFrameBuffer.size(); i++)
	{
		vkDestroyFramebuffer(vkRef.logDevice, _imguiFrameBuffer[i], &vkRef.hostAllocator);
	}
	vkDestroyRenderPass(vkRef.logDevice, ImGuiManager::_imguiRenderPass, &vkRef.hostAllocator);
	vkDestroyDescriptorPool(vkRef.logDevice, ImGuiManager::_imguiDescriptorPool, &vkRef.hostAllocator);

	LOG_INFO("Shutdown ImGui");
}

void ImGuiManager::CreateImGuiFrameBuffer(const VkRef& vkRef, const T_vector<SwapChainImage, MT_GRAPHICS> swapChainImages, VkExtent2D swapChainExtent)
{
	for (u32 i = 0; i < _imguiFrameBuffer.size(); i++)
	{
		vkDestroyFramebuffer(vkRef.logDevice, ImGuiManager::_imguiFrameBuffer[i], &vkRef.hostAllocator);
	}

	// Preallocate size
	_imguiFrameBuffer.resize(vkRef.phyDevice.swapChainBufferCount);

	_swapChainExtentRef = swapChainExtent;

	for (size_t i = 0; i < _imguiFrameBuffer.size(); i++)
	{
		// Add attachments made in the render pass in the same order
		std::array<VkImageView, 1> attachments = {
			swapChainImages[i].imageView
		};

		VkFramebufferCreateInfo framebufferCreateInfo = {};
		framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCreateInfo.renderPass = _imguiRenderPass;							// Render pass layout the framebuffer will be used with
		framebufferCreateInfo.attachmentCount = static_cast<u32>(attachments.size());	// Number of attachments you made in the render pass
		framebufferCreateInfo.pAttachments = attachments.data();						// List of attachments you made in the render pass (1:1)
		framebufferCreateInfo.width = _swapChainExtentRef.width;						// Framebuffer width
		framebufferCreateInfo.height = _swapChainExtentRef.height;						// Framebuffer height
		framebufferCreateInfo.layers = 1;												// Framebuffer layers (number designated in swap chain creation)

		LOG_VKRESULT(vkCreateFramebuffer(vkRef.logDevice, &framebufferCreateInfo, &vkRef.hostAllocator, &_imguiFrameBuffer[i]));
	}
}

void ImGuiManager::_DockSpaceManager(ImGuiIO& io)
{
	constexpr ImGuiWindowFlags windowFlags =  ImGuiWindowFlags_MenuBar			| ImGuiWindowFlags_NoTitleBar	
											| ImGuiWindowFlags_NoDocking		| ImGuiWindowFlags_NoCollapse
											| ImGuiWindowFlags_NoBackground		| ImGuiWindowFlags_NoResize
											| ImGuiWindowFlags_NoMove			| ImGuiWindowFlags_NoBringToFrontOnFocus
											| ImGuiWindowFlags_NoNavFocus;
	
	constexpr ImGuiDockNodeFlags dockspaceFlags =	ImGuiDockNodeFlags_PassthruCentralNode 
												  | ImGuiDockNodeFlags_NoDockingOverCentralNode;

	
	// Update DockSpace size, position, and style.
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace Demo", &_bDockSpaceOpen, windowFlags);
	ImGui::PopStyleVar(3);

	// Submit the DockSpace
	const ImGuiID dockspaceID = ImGui::GetID("LayerDockSpace");
	ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), dockspaceFlags);
	
	// TODO: Flush out these menu options
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			ImGui::MenuItem("Open", "Ctrl + O");
			ImGui::MenuItem("Save", "Ctrl + S");
			ImGui::Separator();
			ImGui::MenuItem("Exit", "Alt + F4");

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Settings"))
		{
			ImGui::SliderFloat("Window Opacity", &_imguiWindowOpacity, 0.0f, 1.0f);

			ImGui::EndMenu();
		}

		// Print the frame rate data in the middle of the bar
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2 - ImGui::CalcTextSize("XXXX ms/frame | XXXX FPS").x / 2);
		ImGui::Text(" %.3f ms/frame | %.1f FPS", 1000.0f / io.Framerate, io.Framerate);

		ImGui::EndMenuBar();
	}

	ImGui::End();
}

