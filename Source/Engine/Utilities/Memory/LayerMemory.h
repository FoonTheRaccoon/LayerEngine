#pragma once
#include "ThirdParty.h"


namespace LayerMemory
{
	inline void* AlignedMalloc(size_t size, size_t alignment) 
	{
		void* ptr = nullptr;

	#if defined(_WIN64)
		ptr = _aligned_malloc(size, alignment);
	#elif defined(__linux__) || defined(__ANDROID__)
		if (posix_memalign(&ptr, alignment, size) != 0)
		{
			ptr = nullptr;
		}
	#elif defined(__APPLE__)
		posix_memalign(&ptr, alignment, size);
	#else
		return malloc(size);
	#endif

		return ptr;
	}

	inline void AlignedFree(void* ptr) 
	{
		if (ptr == nullptr) { return;}

	#if defined(_WIN64)
		_aligned_free(ptr);
	#else
		free(ptr);
	#endif
	}

	
}


