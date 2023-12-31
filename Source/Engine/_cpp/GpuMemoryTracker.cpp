#include "GpuMemoryTracker.h"
#include "LayerMemory.h"
#include "Logger.h"
#include "ImGuiManager.h"
#include "HelperTypes.h"


namespace GpuMemoryTracker
{
	// Array to keep track of every GPU memory allocation
	std::array<MemoryUsageInfo, GPU_USAGE_MAX> _gpuMemoryUsage = {};

	// --Internal helpers--

	// Register function ImGui manager uses to draw GPU memory tracker UI
	void _DrawGpuMemoryTrackerUI();

	// Returns GPU usage tag name for given GpuMemoryUsageTag index
	const char* _GetGpuUsageTagName(u32 tag);
}



void GpuMemoryTracker::InitializeGpuMemoryTracker()
{
	// Set default values for each tag 
	for (size_t i = 0; i < _gpuMemoryUsage.size(); i++)
	{
		_gpuMemoryUsage[i].tagName = _GetGpuUsageTagName(i);
		_gpuMemoryUsage[i].allocations = 0;
		_gpuMemoryUsage[i].size = 0;
		_gpuMemoryUsage[i].SetDisplayLabel();
	}

	// Set function for GPU memory usage UI
	REGISTER_EDITOR_UI_WINDOW(nullptr, GpuMemoryTracker::_DrawGpuMemoryTrackerUI)
}

void GpuMemoryTracker::AllocatedGpuMemory(GpuMemoryUsageTag tag, u64 sizeOfAlloc)
{
	_gpuMemoryUsage[tag].size += sizeOfAlloc;
	_gpuMemoryUsage[tag].allocations += 1;
	_gpuMemoryUsage[tag].SetDisplayLabel();
}

void GpuMemoryTracker::DeallocatedGpuMemory(GpuMemoryUsageTag tag, u64 sizeOfAlloc)
{
	_gpuMemoryUsage[tag].size -= sizeOfAlloc;
	_gpuMemoryUsage[tag].allocations -= 1;
	_gpuMemoryUsage[tag].SetDisplayLabel();
}


void GpuMemoryTracker::LogGpuMemoryUsage()
{
	Logger::AddToSessionLogFile("------- GPU MEMORY USAGE (Allocation # | Size) -------");
	for (u64 i = 0; i < GPU_USAGE_MAX; i++)
	{
		Logger::AddToSessionLogFile(T_string(_gpuMemoryUsage[i].tagName,
			std::to_string(_gpuMemoryUsage[i].allocations), " Allocations | ", 
			std::to_string(_gpuMemoryUsage[i].displaySize), _gpuMemoryUsage[i].sizeLabel));
	}
}

void GpuMemoryTracker::_DrawGpuMemoryTrackerUI()
{
	ImGui::Begin("Memory");

	ImGui::SeparatorText("GPU Memory Usage");
	
	constexpr ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;

	if (ImGui::BeginTable("HostMemoryUsageTable", 3, flags))
	{
		ImGui::TableSetupColumn("Memory Type");
		ImGui::TableSetupColumn("# Allocations");
		ImGui::TableSetupColumn("Size");
		ImGui::TableHeadersRow();



		for (MemoryUsageInfo& trackerTag : _gpuMemoryUsage)
		{
			ImGui::TableNextRow();
			// Tag Name
			ImGui::TableSetColumnIndex(0);
			ImGui::TextUnformatted(trackerTag.tagName);
			// Number of Allocations
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%llu", trackerTag.allocations);
			// Byte Size
			ImGui::TableSetColumnIndex(2);
			ImGui::Text("%.3f%s", trackerTag.displaySize, trackerTag.sizeLabel);

		}
		ImGui::EndTable();
	}
	
	ImGui::End();
}


const char* GpuMemoryTracker::_GetGpuUsageTagName(u32 tag)
{
	switch (tag)
	{
	case GPU_USAGE_UNKNOWN:
		return "UNKNOWN:\t\t";
	case GPU_USAGE_UNIFORM_TEXEL_BUFFER:
		return "UNIFORM TEXEL BUFFERS:\t";
	case GPU_USAGE_STORAGE_TEXEL_BUFFER:
		return "STORAGE TEXEL BUFFERS:\t";
	case GPU_USAGE_UNIFORM_BUFFER:
		return "UNIFORM BUFFERS:\t";
	case GPU_USAGE_STORAGE_BUFFER:
		return "STORAGE BUFFERS:\t";
	case GPU_USAGE_VERTEX_BUFFER:
		return "VERTEX BUFFERS:\t\t";
	case GPU_USAGE_INDEX_BUFFER:
		return "INDEX BUFFERS:\t\t";
	case GPU_USAGE_SAMPLED_IMAGE:
		return "SAMPLED IMAGES:\t\t";
	case GPU_USAGE_STORAGE_IMAGE:
		return "STORAGE IMAGES:\t\t";
	case GPU_USAGE_ATTACHMENT_IMAGE:
		return "ATTACHMENT IMAGES:\t";
	case GPU_USAGE_MAX:
		return "GPU_USAGE_MAX (THIS SHOULDN'T BE PRINTED) ";
	default:
		return "!Gpu Memory Tracker Tag not found, this shouldn't be possible!";
	}
}