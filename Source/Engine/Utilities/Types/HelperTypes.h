#pragma once
#include "ThirdParty.h"


// Helper struct to pack relevant data for memory tracking. Used by both Host and GPU memory trackers
struct MemoryUsageInfo
{
	MemoryUsageInfo() = default;
	~MemoryUsageInfo() = default;

	const char* tagName = "";
	u64 allocations = 0;
	u64 size = 0;
	const char* sizeLabel = " XiB";
	f32 displaySize = 0.0f;

	// Updates size string to proper scope (KiB, MiB, and GiB)
	void SetDisplayLabel()
	{
		if (size >= GiB)
		{
			sizeLabel = " GiB";
			displaySize = static_cast<f32>(size) / static_cast<f32>(GiB);
		}
		else if (size >= MiB)
		{
			sizeLabel = " MiB";
			displaySize = static_cast<f32>(size) / static_cast<f32>(MiB);
		}
		else if (size >= KiB)
		{
			sizeLabel = " KiB";
			displaySize = static_cast<f32>(size) / static_cast<f32>(KiB);
		}
		else
		{
			sizeLabel = " B";
			displaySize = static_cast<f32>(size);
		}

	}
};