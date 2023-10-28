#pragma once
#include "ThirdParty.h"


namespace RenderManager
{
	void Initilize(const char* appName, u32 winWidth, u32 winHeight);
	void Shutdown();
	bool WindowsShouldClose();
	void DrawFrame();
}

