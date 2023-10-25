#pragma once

#ifndef LAYER_ENGINE_UTILITIES_H
#define LAYER_ENGINE_UTILITIES_H

// Third party includes that have their defines/config done in 'ThirdParty\ThirdPartyConfig.h'
#include "ThirdParty.h"

// Include any global constants that will never change
#include "GlobalConstants.h"

// Include functions that will return strings to various Vulkan enums and other struct members
#include "VkNames.h"

// Events
#include "Broadcaster.h"

// HelperFunctions
#include "FileHelper.h"
#include "StringHelper.h"
#include "Timer.h"

// Logger
#include "Logger.h"
#include "LoggingCallbacks.h"

//Memory
#include "MemoryTracker.h"
#include "GpuMemoryTracker.h"
#include "LayerMemory.h"
#include "LayerContainers.h"

//Types
#include "HelperTypes.h"
#include "VkTypes.h"

// UI
#include "ImGuiManager.h"


namespace EngineUtilities
{
	inline void InitilizeEngineUtilities()
	{
		// ImGuiManger Gets setup in the Render Manager since it require Vulkan prerequisites
		Logger::InitilizeLogging();
		MemoryTracker::InitilizeMemoryTracker();
		GpuMemoryTracker::InitilizeGpuMemoryTracker();
	}

	inline void ShutdownEngineUtilities()
	{
		Logger::ShutdownLogging();
	}
}

#endif // LAYER_ENGINE_UTILITIES_H