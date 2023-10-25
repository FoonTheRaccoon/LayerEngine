#include "MemoryTracker.h"
#include "EngUtils.h"


// -----NEW/DELETE OVERLOADS------
void* __CRTDECL operator new(size_t size)
{
	// Capture third party/miscellaneous allocations adding them to MT_UNKOWN
	MemoryTracker::AllocatedHostMemory(MT_UNKOWN, size);
	return malloc(size);
}

void* __CRTDECL operator new[](size_t size)
{
	// Capture third party/miscellaneous allocations adding them to MT_UNKOWN
	MemoryTracker::AllocatedHostMemory(MT_UNKOWN, size);
	return malloc(size);
}

void __CRTDECL operator delete(void* memory, size_t size) noexcept
{
	// Capture third party/miscellaneous frees subtracting them from MT_UNKOWN
	MemoryTracker::DeallocatedHostMemory(MT_UNKOWN, size);
	free(memory);
}

void __CRTDECL operator delete[](void* memory, size_t size) noexcept
{
	// Capture third party/miscellaneous frees subtracting them from MT_UNKOWN
	MemoryTracker::DeallocatedHostMemory(MT_UNKOWN, size);
	free(memory);
}



namespace MemoryTracker
{
	// Array to keep track of every host memory allocation
	std::array<MemoryUsageInfo, MT_MAX_VALUE> _hostMemoryUsage;

	// Registered broadcast function for drawing Memory tracking UI
	void _DrawMemoryTrackerUI();

	// Returns string name for MemoryTrackerTags
	const char* _GetMemoryTrackerTagName(int tag);
}

void MemoryTracker::InitilizeMemoryTracker()
{
	// Set default values for each tag 
	for (size_t i = 0; i < _hostMemoryUsage.size(); i++)
	{
		_hostMemoryUsage[i].tagName = _GetMemoryTrackerTagName(i);
		_hostMemoryUsage[i].allocations = 0;
		_hostMemoryUsage[i].size = 0;
		_hostMemoryUsage[i].SetDisplayLabel();
	}

	REGISTER_EDITOR_UI(nullptr, MemoryTracker::_DrawMemoryTrackerUI);
}

void MemoryTracker::AllocatedHostMemory(MemoryTrackerTag tag, u64 sizeOfAlloc)
{
	_hostMemoryUsage[tag].size += sizeOfAlloc;
	_hostMemoryUsage[tag].allocations += 1;
	_hostMemoryUsage[tag].SetDisplayLabel();
}

void MemoryTracker::DeallocatedHostMemory(MemoryTrackerTag tag, u64 sizeOfAlloc)
{
	_hostMemoryUsage[tag].size -= sizeOfAlloc;
	_hostMemoryUsage[tag].allocations -= 1;
	_hostMemoryUsage[tag].SetDisplayLabel();
}

void MemoryTracker::LogMemoryUsage()
{
	Logger::AddToLogFile("------- HOST MEMORY USAGE (Allocation # | Size) -------");
	for (u64 i = 0; i < MT_MAX_VALUE; i++)
	{
			Logger::AddToLogFile(T_string(_hostMemoryUsage[i].tagName, 
				std::to_string(_hostMemoryUsage[i].allocations), " Allocations | ", 
				std::to_string(_hostMemoryUsage[i].displaySize), _hostMemoryUsage[i].sizeLabel));
	}
}

void MemoryTracker::_DrawMemoryTrackerUI()
{
	ImGui::Begin("Memory");

	ImGui::SeparatorText("Host Memory Usage");

	constexpr ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;

	if (ImGui::BeginTable("HostMemoryUsageTable", 3, flags))
	{
		ImGui::TableSetupColumn("Memory Type");
		ImGui::TableSetupColumn("# Allocations");
		ImGui::TableSetupColumn("Size");
		ImGui::TableHeadersRow();



		for (u64 row = 0; row < _hostMemoryUsage.size(); row++)
		{
			ImGui::TableNextRow();
			// Tag Name
			ImGui::TableSetColumnIndex(0);
			ImGui::TextUnformatted(_hostMemoryUsage[row].tagName);
			// Number of Allocations
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%llu", _hostMemoryUsage[row].allocations);
			// Byte Size
			ImGui::TableSetColumnIndex(2);
			ImGui::Text("%.3f%s", _hostMemoryUsage[row].displaySize, _hostMemoryUsage[row].sizeLabel);

		}
		ImGui::EndTable();
	}

	ImGui::End();
}

const char* MemoryTracker::_GetMemoryTrackerTagName(int tag)
{
	switch (tag)
	{
		case MT_UNKOWN:
			return "UNKOWN:\t\t";
		case MT_ENGINE:
			return "ENGINE:\t\t";
		case MT_EDITOR:
			return "EDITOR:\t\t";
		case MT_GAME:
			return "GAME:\t\t";
		case MT_GRAPHICS:
			return "GRAPHICS:\t";
		case MT_VULKAN:
			return "VULKAN:\t\t";
		case MT_VULKAN_INTERNAL:
			return "VK_INTERNAL:\t";
		case MT_TEXTURE:
			return "TEXTURES:\t";
		case MT_AUDIO:
			return "AUDIO:\t\t";
		case MT_PHYSICS:
			return "PHYSICS:\t";
		case MT_AI:
			return "AI:\t\t";
		case MT_NETWORKING:
			return "NETWORKING:\t";
		case MT_WORKER:
			return "WORKERS:\t";
		case MT_ENTITY:
			return "ENTITIES:\t";
		case MT_COMPONENT:
			return "COMPONENTS:\t";
		case MT_TRANSFORM:
			return "TRANSFORMS:\t";
		case MT_SCRIPT:
			return "SCRIPTS:\t";
		case MT_SCENE:
			return "SCENES:\t\t";
		case MT_STRING:
			return "STRINGS:\t";
		case MT_OTHER:
			return "OTHER:\t\t";
		case MT_TEMPORARY:
			return "TEMPORARY:\t";
		case MT_MAX_VALUE:
			return "MAX_VALUE (THIS SHOULDN'T BE PRINTED) ";
		default:
			return "!Memory Tracker Tag not found, this shouldn't be possible!";
	}
}


