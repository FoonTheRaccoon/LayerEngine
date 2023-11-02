#pragma once
#include "ThirdParty.h"

namespace LayerMemory
{
	inline void* AlignedMalloc(size_t size, size_t alignment) 
	{
		void* ptr;

	#if LAYER_PLATFORM_WINDOWS
		ptr = _aligned_malloc(size, alignment);
	#elif LAYER_PLATFORM_LINUX || LAYER_PLATFORM_ANDROID
		if (posix_memalign(&ptr, alignment, size) != 0)
		{
			ptr = nullptr;
		}
	#elif LAYER_PLATFORM_APPLE
		posix_memalign(&ptr, alignment, size);
	#else
		return malloc(size);
	#endif

		return ptr;
	}

	inline void AlignedFree(void* ptr) 
	{
		if (ptr == nullptr) { return;}

	#if LAYER_PLATFORM_WINDOWS
		_aligned_free(ptr);
	#else
		free(ptr);
	#endif
	}

	
} // namespace LayerMemory


