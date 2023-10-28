#pragma once
#include "ThirdParty.h"

// Global Constants/defines
namespace GlobalConstants
{
	//Platform Usage
#ifdef __ANDROID__
	constexpr bool bOnAndroid = true;
#else 
	constexpr bool bOnAndroid = false;
#endif

#ifdef _WIN64
	constexpr bool bOnWindows = true;
#else 
	constexpr bool bOnWindows = false;
#endif

#if defined(__linux__) && !defined(__ANDROID__)
	constexpr bool bOnLinux = true;
#else 
	constexpr bool bOnLinux = false;
#endif

#ifdef __APPLE__
	constexpr bool bOnAppleOS = true;
#else 
	constexpr bool bOnAppleOS = false;
#endif

#ifdef _DEBUG
	constexpr bool bDebug = true;
#else 
	constexpr bool bDebug = false;
#endif


	// Layer Defines
#if defined(LAYER_DEBUG)
		constexpr bool bLayerDebugOn = true;
#else
		constexpr bool bLayerDebugOn = false;
#endif

#if defined(LAYER_EDITOR)
	constexpr bool bUsingLayerEditor = true;
#else
	constexpr bool bUsingLayerEditor = false;
#endif

#if defined(LAYER_USE_LIVE_LOGGER)
	constexpr bool bUsingLiveLogger = true;
#else
	constexpr bool bUsingLiveLogger = false;
#endif

#if defined(LAYER_USE_VERBOSE_LOGGER)
	constexpr bool bUsingVerboseLogger = true;
#else
	constexpr bool bUsingVerboseLogger = false;
#endif

#if defined(LAYER_USE_VALIDATION_LAYERS)
	constexpr bool bEnableValidationLayers = true;
#else
	constexpr bool bEnableValidationLayers = false;
#endif


} // namespace GlobalConstants

// Platform Aliases
#ifdef __ANDROID__
using Window = ANativeWindow;
#else
using Window = GLFWwindow;
#endif

