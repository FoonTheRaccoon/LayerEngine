#include "Engine.h"
#include "EngUtils.h"
#include "RenderManager.h"
#include "Logger.h"
#include "LoggingCallbacks.h"


void Engine::StartUp(const char* appName, u32 winWidth, u32 winHeight)
{
	LOG_DEBUG("Starting Engine...")
    
    EngineUtilities::InitializeEngineUtilities();
    
    #if LAYER_PLATFORM_ANDROID
		// TODO: Create Android implementation
    #else // GLFW
		glfwSetErrorCallback(LoggingCallbacks::glfw_error_callback);
		glfwInit();
    #endif
    
    RenderManager::Initialize(appName, winWidth, winHeight);

	LOG_INFO("Engine Started")
}

void Engine::Run()
{
	// TODO: Make this loop multi-platform
	while (!RenderManager::WindowsShouldClose())
	{
		glfwPollEvents();
		RenderManager::DrawFrame();
	}
}

void Engine::Shutdown()
{
	LOG_DEBUG("Shutting Engine Down...")

	RenderManager::Shutdown();
    
    #if LAYER_PLATFORM_ANDROID
        // TODO: Create Android implementation
    #else // GLFW
        glfwTerminate();
    #endif

	LOG_INFO("Engine Shut Down")

	EngineUtilities::ShutdownEngineUtilities();
}
