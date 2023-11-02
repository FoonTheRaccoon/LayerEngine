#pragma once
#include "Logger.h"
#include "MemoryTracker.h"
#include "GpuMemoryTracker.h"


namespace EngineUtilities
{
	inline void InitializeEngineUtilities()
	{
		// ImGuiManger Gets setup in the Render Manager since it require Vulkan prerequisites
        Logger::InitializeLogging();
        MemoryTracker::InitializeMemoryTracker();
        GpuMemoryTracker::InitializeGpuMemoryTracker();
	}

	inline void ShutdownEngineUtilities()
	{
		Logger::ShutdownLogging();
	}
}
