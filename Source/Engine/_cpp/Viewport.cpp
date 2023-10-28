#include "Viewport.h"
#include "Logger.h"




void Viewport::CreateViewport(const char* appName, u32 winWidth, u32 winHeight)
{
	LOG_DEBUG("Creating Viewport...");

	m_Name = appName;
	m_Width = winWidth;
	m_Height = winHeight;

	if constexpr (GlobalConstants::bOnAndroid)
	{ // Android
		// TODO: Create Android implementation
	}
	else
	{ // GLFW
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); 
		glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
		m_pWindow = glfwCreateWindow(m_Width, m_Height, m_Name, nullptr, nullptr);
		glfwSetWindowSizeLimits(m_pWindow, minWindowWidth, minWindowHeight, initialMaxWindowWidth, initialMaxWindowHeight);

		// TODO: Create a function that can load the icon.
		// GLFWimage images = load_icon("..\\..\\Resources\\EngineTextures\\LayerIcon.png");
		// glfwSetWindowIcon(m_pWindow, 1, &images);
	}

	ASSERT_PTR(m_pWindow);

	LOG_INFO("Created Viewport");
}
void Viewport::DestroyViewport()
{
	LOG_DEBUG("Destroying Viewport...");

	if constexpr (GlobalConstants::bOnAndroid)
	{ // Android
		// TODO: Create Android implementation
	}
	else
	{ // GLFW
		glfwDestroyWindow(m_pWindow);
	}

	LOG_INFO("Destroyed Viewport");
}

