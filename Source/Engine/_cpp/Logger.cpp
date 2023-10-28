#include "Logger.h"
#include "GlobalConstants.h"
#include "ImGuiManager.h"
#include "FileHelper.h"

// Private
namespace Logger
{
	// Global log message buffer that will be used by _DrawLogUI()
	T_vector<LogLine, MT_ENGINE> _liveLogLineBuffer = {};

	// Global log buffer that will hold all log records
	T_string _sessionLogBuffer = {};

	// Path relative to the current working directory to place the session log
	constexpr const char* _sessionLogPath = "Logs\\SessionLog.txt";

	// 

	void _OpenLogFile();
	void _DrawLogUI();
}

void Logger::InitilizeLogging()
{
	FileHelper::CreateFolderIfAbsent("Logs\\");
	if constexpr (GlobalConstants::bUsingLiveLogger)
	{
		REGISTER_EDITOR_UI(nullptr, Logger::_DrawLogUI);
	}
}

void Logger::ShutdownLogging()
{
	FileHelper::WriteStringToFile(_sessionLogBuffer, _sessionLogPath);
}

void Logger::AddToSessionLogFile(const char* message)
{
	_sessionLogBuffer.AppendMany(message, "\n");
}

void Logger::AddToSessionLogFile(const T_string& message)
{
	_sessionLogBuffer.AppendMany(message, "\n");
}

void Logger::PrintLog(LogSeverityLevel severityLevel, const char* message)
{
	constexpr const char* severityStrings[] = {
		"[FATAL]: ",
		"[ERROR]: ",
		"[WARNING]: ",
		"[INFO]: ",
		"[BENCHMARK]: ",
		"[DEBUG]: ",
		"" // OTHER
		};

	constexpr ImVec4 severityColors[] = {
		{ 0.45f, 0.0f, 0.0f, 1.0f },	// FATAL == Dark Red
		{ 0.85f, 0.5f, 0.0f, 1.0f },	// ERROR == Dark Orange
		{ 1.0f, 1.0f, 0.0f, 1.0f },		// WARNING == Yellow
		{ 1.0f, 1.0f, 1.0f, 1.0f },		// INFO == White
		{ 0.73f, 0.29f, 1.0f, 1.0f },	// BENCHMARK == Light Purple
		{ 0.29f, 0.87f, 1.0f, 1.0f },	// DEBUG == Light Blue
		{ 0.0f, 0.80f, 0.0f, 1.0f }		// OTHER == Green
	};

	const T_string printBuffer = T_string(severityStrings[severityLevel], message);
	_sessionLogBuffer.AppendMany(printBuffer, "\n");

	if constexpr (GlobalConstants::bUsingLiveLogger)
	{
		_liveLogLineBuffer.emplace_back(severityColors[severityLevel], printBuffer);
	}


	// TODO: Update this to pop up messages on Linux and MacOS
	if (severityLevel == LOG_SEVERITY_FATAL)
	{
		if constexpr (GlobalConstants::bOnWindows)
		{
			MessageBoxA(NULL, message, "Fatal Error", MB_ICONERROR | MB_OK);
		}

		Logger::fatalShutdownBroadcaster.Broadcast();
		ShutdownLogging();
		std::exit(EXIT_FAILURE);
	}
	else if (severityLevel == LOG_SEVERITY_ERROR)
	{
		if constexpr (GlobalConstants::bOnWindows)
		{
			MessageBoxA(NULL, message, "Error", MB_ICONERROR | MB_OK);
		}
	}
}

void Logger::PrintLog(LogSeverityLevel severityLevel, const T_string& message)
{
	PrintLog(severityLevel, message.c_str());
}

void Logger::_OpenLogFile()
{
	FileHelper::WriteStringToFile(_sessionLogBuffer, _sessionLogPath);
	FileHelper::OpenFileInExternalProgram(_sessionLogPath);
}

void Logger::_DrawLogUI()
{
	static ImGuiTextFilter filter;

	ImGui::Begin("Log");

	bool openLog = ImGui::Button("Open Session Log");
	if (openLog)
		_OpenLogFile();

	ImGui::SameLine();
	bool closeEditor = ImGui::Button("Close Editor | Open Log");
	if (closeEditor)
	{
		Logger::fatalShutdownBroadcaster.Broadcast();
		_OpenLogFile();
		std::exit(EXIT_SUCCESS);
	}

	bool clear = ImGui::Button("Clear");
	ImGui::SameLine();
	filter.Draw("Filter", -100.0f);

	ImGui::Separator();

	if (ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar))
	{
		if (clear)
			_liveLogLineBuffer.clear();

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		
		if (filter.IsActive())
		{
			for (const LogLine& logLine : _liveLogLineBuffer)
			{
				if (filter.PassFilter(logLine.text.c_str()))
					ImGui::TextColored(logLine.color, "%s", logLine.text.c_str());
			}
		}
		else
		{
			// Here we instead demonstrate using the clipper to only process lines that are within the visible area.
			// If you have tens of thousands of items and their processing cost is non-negligible, coarse clipping them
			// on your side is recommended. Using ImGuiListClipper requires
			// - A) random access into your data
			// - B) items all being the  same height,
			// both of which we can handle since we have an array pointing to the beginning of each line of text.
			// When using the filter (in the block of code above) we don't have random access into the data to display
			// anymore, which is why we don't use the clipper. Storing or skimming through the search result would make
			// it possible (and would be recommended if you want to search through tens of thousands of entries).
			ImGuiListClipper clipper;
			clipper.Begin((u32)_liveLogLineBuffer.size());
			while (clipper.Step())
			{
				for (int lineNumber = clipper.DisplayStart; lineNumber < clipper.DisplayEnd; lineNumber++)
				{
					ImGui::TextColored(_liveLogLineBuffer[lineNumber].color, "%s",_liveLogLineBuffer[lineNumber].text.c_str());
				}
			}
			clipper.End();
		}

		ImGui::PopStyleVar();

		// Keep up at the bottom of the scroll region if we were already at the bottom at the beginning of the frame.
		// Using a scrollbar or mouse-wheel will take away from the bottom edge.
		if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
			ImGui::SetScrollHereY(1.0f);
	}
	ImGui::EndChild();
	ImGui::End();
}



