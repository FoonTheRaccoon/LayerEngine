#pragma once
#pragma clang diagnostic push
#pragma ide diagnostic ignored "readability-redundant-declaration"
#include "ThirdParty.h"

enum MemoryTrackerTag : u32
{
	MT_UNKNOWN,
	MT_ENGINE,
	MT_EDITOR,
	MT_GAME,
	MT_GRAPHICS,
	MT_VULKAN,
	MT_VULKAN_INTERNAL,
	MT_TEXTURE,
	MT_AUDIO,
	MT_PHYSICS,
	MT_AI,
	MT_NETWORKING,
	MT_WORKER,
	MT_ENTITY,
	MT_COMPONENT,
	MT_TRANSFORM,
	MT_SCRIPT,
	MT_SCENE,
	MT_STRING,
	MT_OTHER,
	MT_TEMPORARY,
	MT_MAX_VALUE
};

// Overload new and delete ops to capture third party/miscellaneous allocations/frees assigning them to MT_UNKNOWN
_Ret_notnull_ _Post_writable_byte_size_(size)
void* __CRTDECL operator new(size_t _Size );

_Ret_notnull_ _Post_writable_byte_size_(size)
void* __CRTDECL operator new[](size_t size);

void __CRTDECL operator delete(void* memory, size_t size) noexcept;

void __CRTDECL operator delete[](void* memory,size_t size) noexcept;

// --MEMORY TRACKER--
namespace MemoryTracker
{
	// Initialize memory tracker and setup everything it needs
	void InitializeMemoryTracker();

	// Called anytime host memory is allocated, so we can update the relevant tracking info.
	void AllocatedHostMemory(MemoryTrackerTag tag, u64 sizeOfAlloc);

	// Called anytime host memory is deallocated, so we can update the relevant tracking info.
	void DeallocatedHostMemory(MemoryTrackerTag tag, u64 sizeOfAlloc);

	// Add the current host memory usage to the log file
	void LogMemoryUsage();
}

// Custom Allocator that Layer Containers use that tracks host memory usage via 'hostMemoryUsage'
template<typename TYPE>
struct MemoryTrackerAllocator
{
	using value_type = TYPE;

	explicit MemoryTrackerAllocator(MemoryTrackerTag tag = MT_UNKNOWN)
    : m_Tag(tag)
    {
        if (tag >= MT_MAX_VALUE) m_Tag = MT_UNKNOWN;
    }
    
	template<typename U>
	explicit MemoryTrackerAllocator(const MemoryTrackerAllocator<U>& other)
    : m_Tag(other.m_Tag)
    {
        if (other.m_Tag >= MT_MAX_VALUE || other.m_Tag < 0) m_Tag = MT_UNKNOWN;
    }

	TYPE* allocate(size_t size)
	{
		const size_t bytes = size * sizeof(TYPE);
		MemoryTracker::AllocatedHostMemory(m_Tag, bytes);
		return static_cast<TYPE*>(malloc(bytes));
	}
	void deallocate(TYPE* memory, size_t size) noexcept
	{
		const size_t bytes = size * sizeof(TYPE);
		MemoryTracker::DeallocatedHostMemory(m_Tag, bytes);
		free(memory);
	}

	MemoryTrackerTag m_Tag = MT_UNKNOWN;
};

template<typename TYPE> 
inline bool operator==(MemoryTrackerAllocator<TYPE> const&, MemoryTrackerAllocator<TYPE> const&) { return true; }

template<typename TYPE>
inline bool operator!=(MemoryTrackerAllocator<TYPE> const&, MemoryTrackerAllocator<TYPE> const&) { return false; }




#pragma clang diagnostic pop