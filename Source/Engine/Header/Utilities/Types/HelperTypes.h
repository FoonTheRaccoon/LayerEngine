#pragma once
#include "GlobalConstants.h"


// Helper struct to pack relevant data for memory tracking. Used by both Host and GPU memory trackers
struct MemoryUsageInfo
{
	const char* tagName;
	u64 allocations;
	u64 size;
	const char* sizeLabel = " XiB";
	f32 displaySize = 0.0f;

	void SetDisplayLabel()
	{
		if (size >= GiB)
		{
			sizeLabel = " GiB";
			displaySize = size / static_cast<f32>(GiB);
		}
		else if (size >= MiB)
		{
			sizeLabel = " MiB";
			displaySize = size / static_cast<f32>(MiB);
		}
		else if (size >= KiB)
		{
			sizeLabel = " KiB";
			displaySize = size / static_cast<f32>(KiB);
		}
		else
		{
			sizeLabel = " B";
			displaySize = static_cast<f32>(size);
		}

	}
};