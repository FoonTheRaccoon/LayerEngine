#include "Editor.h"

Editor::Editor()
{
}

Editor::~Editor()
{
	Logger::shutdownBroadcaster.Unregister(this);
}

void Editor::StartUp()
{
	TIMER_LOG("Editor StartUp()");

	m_Engine.StartUp("Layer Editor", 1280, 720);

	// Add Shutdown function to call list if fatal error occurs somewhere
	Logger::shutdownBroadcaster.Register(this, std::bind(&Editor::Shutdown, this));

	Logger::AddToLogFile("Memory Usage After Engine Start Up:");
	MemoryTracker::LogMemoryUsage();
	GpuMemoryTracker::LogGpuMemoryUsage();
}

void Editor::RunGameLoop()
{
	m_Engine.Run();
	
}

void Editor::Shutdown()
{
	m_Engine.Shutdown();

	Logger::AddToLogFile("Memory Usage After Engine Shutdown:");
	MemoryTracker::LogMemoryUsage();
	GpuMemoryTracker::LogGpuMemoryUsage();
}
