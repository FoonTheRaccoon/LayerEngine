#pragma once
 
#include "EngUtils.h"
#include "RenderManager.h"


class Engine
{
public:
	Engine();
	~Engine();
	void StartUp(const char* appName, u32 winWidth, u32 winHeight);
	void Run();
	void Shutdown();
private:
	RenderManager m_RenderManager;
};