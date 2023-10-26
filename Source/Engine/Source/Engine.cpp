#include "Engine.h"

Engine::Engine()
{
	// Init all engine utilities
	EngineUtilities::InitilizeEngineUtilities();

}

Engine::~Engine()
{
	
}

void Engine::StartUp(const char* appName, u32 winWidth, u32 winHeight)
{
	LOG_DEBUG("Starting Engine...");

	if constexpr (GlobalConstants::bOnAndroid)
	{ 
		// TODO: Create Android implementation
	}
	else // GLFW
	{ 
		glfwSetErrorCallback(LoggingCallbacks::glfw_error_callback);
		glfwInit();
	}

	m_RenderManager.Initilize(appName, winWidth, winHeight);

	LOG_INFO("Engine Started");
}

void Engine::Run()
{
	// TODO: Make this loop multi-platform
	while (!m_RenderManager.WindowsShouldClose())
	{
		glfwPollEvents();
		m_RenderManager.DrawFrame();
	}
}

void Engine::Shutdown()
{
	LOG_DEBUG("Shutting Engine Down...");

	m_RenderManager.Shutdown();

	if constexpr (GlobalConstants::bOnAndroid)
	{
		// TODO: Create Android implementation
	}
	else // GLFW
	{
		glfwTerminate();
	}

	LOG_INFO("Engine Shut Down");

	EngineUtilities::ShutdownEngineUtilities();
}
