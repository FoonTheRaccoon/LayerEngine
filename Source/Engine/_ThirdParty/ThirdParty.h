#pragma once

// --THIRD PARTY CONFIG/DEFINES/MACROS--
// We set all our definitions to configure the third party libraries before we include them.
#include "ThirdPartyConfig.h"
#include "ConstantsAndAliases.h"

// --STD LIBRARY--
#include <vector>
#include <string>
#include <array>
#include <unordered_map>
#include <chrono>
#include <memory>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <stdint.h>
#include <locale>
#include <codecvt>
#include <functional>
#include <execution>
#include <set>
#include <malloc.h>
#include <stdio.h>         
#include <stdlib.h>
#include <sys/stat.h> // For stat() on POSIX systems

// Platform includes
#if LAYER_PLATFORM_WINDOWS
#include <windows.h>

#elif LAYER_PLATFORM_LINUX
#include <mm_malloc.h>
#include <unistd.h>
#include <linux/limits.h>

#elif LAYER_PLATFORM_ANDROID
#include <mm_malloc.h>
#include <unistd.h>
#include <linux/limits.h>

#elif LAYER_PLATFORM_APPLE
#include <mm_malloc.h>
#include <mach-o/dyld.h>

#endif // Platform includes


// --IMGUI--
#include "imgui.h"					    // Imgui Core
#include "imgui_impl_glfw.h"			// Imgui glfw compatibility layer
#include "imgui_impl_vulkan.h"		    // Imgui vulkan compatibility layer

// --VULKAN--
#include "vulkan.h"					    // Core Vulkan


// --VULKAN MEMORY ALLOCATOR--
#include "vk_mem_alloc.h"	            // Vulkan memory allocator used for buffer/image buffer memory allocation (vmaCreateBuffer() / vmaCreateImage())


// --GLFW--
#include "glfw3.h"						// Responsible for windows creation TODO: Conditionally set if preprocessor to strip GLFW away for android builds 


// --GLM--
#include "glm.hpp"						// Math library
#include "gtc\matrix_transform.hpp"

