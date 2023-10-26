#pragma once
#include "Engine.h"

class Editor
{
public:
	Editor();
	~Editor();
	void StartUp();
	void RunGameLoop();
	void Shutdown();

private:
	Engine m_Engine;
};

