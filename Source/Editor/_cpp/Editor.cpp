#include "Editor.h"
#include "Engine.h"
#include "Timer.h"
#include "Logger.h"
#include "MemoryTracker.h"
#include "GpuMemoryTracker.h"
#include "EditorFileManager.h"


void Editor::StartUp()
{
	TIMER_LOG("Editor::StartUp()")

    // Set Working directory, for project files, and core directory, for editor resources
    EditorFileManager::SetupWorkingAndCoreDirectories();
    // Register the .layer file extension with the OS to open up the editor
    EditorFileManager::RegisterFileExtension();
    
    // Start the engine with a default window name and size
	Engine::StartUp("Layer Editor", 1280, 720);

	// Add Shutdown function to call list if fatal error occurs somewhere
	Logger::fatalShutdownBroadcaster.Register(nullptr, &Editor::Shutdown);
    
    
    // Throw post editor start up memory usage into the session log for auditing
	Logger::AddToSessionLogFile("Memory Usage After Engine Start Up:");
	MemoryTracker::LogMemoryUsage();
	GpuMemoryTracker::LogGpuMemoryUsage();
}

void Editor::RunAppLoop()
{
    // TODO: Pull loop out to be the responsibility of the exe
	Engine::Run();
}

void Editor::Shutdown()
{
	Engine::Shutdown();
    
    // Throw post editor shutdown up memory usage into the session log for auditing
	Logger::AddToSessionLogFile("Memory Usage After Engine Shutdown:");
	MemoryTracker::LogMemoryUsage();
	GpuMemoryTracker::LogGpuMemoryUsage();
}
