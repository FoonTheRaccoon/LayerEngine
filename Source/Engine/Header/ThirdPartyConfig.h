#pragma once

// All our defines/macros for third party libraries here
// ______________________________________________________________________________________

// --IMGUI--
// -IMGUI DEFINES-


// --VULKAN MEMORY ALLOCATOR--
// -VMA DEFINES-
// Located in ThirdParty.cpp

// --VULKAN--
// -VULKAN DEFINES-
#ifdef __ANDROID__
#define VK_USE_PLATFORM_ANDROID_KHR	// GLFW will set other Vulkan platforms, we must manually set Android
#endif

// --GLFW--
// -GLFW DEFINES-
#define GLFW_INCLUDE_NONE	// We will manually include our Vulkan API path

// --GLM--
// -GLM DEFINES-
// Base Behavior/Build
//#define GLM_FORCE_MESSAGES				// Verbose output of glm config during compile
#define GLM_FORCE_SWIZZLE				// Allows swizzling of vecs (i.e. vec.xwy or vec.rrbg but not vec.rzy (can't mix rgba and xyzw swizzles)
#define GLM_FORCE_DEPTH_ZERO_TO_ONE		// Sets depth clip space from 0 to 1 instead of -1 to 1.
#define GLM_FORCE_UNRESTRICTED_GENTYPE	// Relaxes rules on what types glm functions can take.
#define GLM_FORCE_CXX17					// Forces c++ 17 features
#define GLM_FORCE_INLINE				// Forces the compiler to inline GLM code.
#define GLM_ENABLE_EXPERIMENTAL			// Allows use of experimental GTX extension headers


// SIMD
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES // Pads vec memory to set length. Required for SIMD
#if defined(_WIN64) || (defined(__linux__) && !defined(__ANDROID__))
#define GLM_FORCE_AVX2		// Force AVX2 if building to x86 platforms, else glm will automatically set SIMD for other systems (Android/apple silicon NEON)
#endif
// TODO: Verify glm Neon SIMD is being built on android/macOS

// Optional
// #define GLM_FORCE_LEFT_HANDED		// If defined, will use left handed coordinate system instead of right handed default.
// #define GLM_FORCE_SIZE_T_LENGTH		// Makes vec.length() returns a glm::length_t, a typedef of int following GLSL instead of int.


