#include "Editor.h"
#include "Engine.h"
#include "Timer.h"
#include "Logger.h"
#include "MemoryTracker.h"
#include "GpuMemoryTracker.h"


void Editor::StartUp()
{
	TIMER_LOG("Editor StartUp()");

	Engine::StartUp("Layer Editor", 1280, 720);

	// Add Shutdown function to call list if fatal error occurs somewhere
	Logger::fatalShutdownBroadcaster.Register(nullptr, &Editor::Shutdown);

	Logger::AddToSessionLogFile("Memory Usage After Engine Start Up:");
	MemoryTracker::LogMemoryUsage();
	GpuMemoryTracker::LogGpuMemoryUsage();
}

void Editor::RunGameLoop()
{
	Engine::Run();
}

void Editor::Shutdown()
{
	Engine::Shutdown();

	Logger::AddToSessionLogFile("Memory Usage After Engine Shutdown:");
	MemoryTracker::LogMemoryUsage();
	GpuMemoryTracker::LogGpuMemoryUsage();
}
