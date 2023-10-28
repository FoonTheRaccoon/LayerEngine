#pragma once
#include "ThirdParty.h"
#include "LayerContainers.h"
#include "Broadcaster.h"
#include "vk_enum_string_helper.h"


// --LOGGER--

enum LogSeverityLevel : u32
{
	LOG_SEVERITY_FATAL,
	LOG_SEVERITY_ERROR,
	LOG_SEVERITY_WARNING,
	LOG_SEVERITY_INFO,
	LOG_SEVERITY_BENCHMARK,
	LOG_SEVERITY_DEBUG,
	LOG_SEVERITY_OTHER
};

// Logger functions that should only be called by logger macros
namespace Logger
{
	struct LogLine
	{
		ImVec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
		T_string text;
	};

	// Keeps track of any shutdown functions needed for exit so we can call them in case of a fatal error log
	inline Broadcaster<void()> fatalShutdownBroadcaster;

	// Used to initialize Layer logging
	void InitilizeLogging();

	// Called when logging window is called, does final write to log file.
	void ShutdownLogging();

	// Updates session log file.
	void AddToSessionLogFile(const char* message);
	void AddToSessionLogFile(const T_string& message);

	// Prints to log and adds to buffer that will be added to log file.
	void PrintLog(LogSeverityLevel severityLevel, const char* message);

	// PrintLog that can take a T_string. Just passes message.c_str() to main PrintLog()
	void PrintLog(LogSeverityLevel severityLevel, const T_string& message);
}

// -LOGGER MACROS-

// -Helper Macros
#define _PRINT_MESSAGE_INFO(severity, message_in)		\
		Logger::PrintLog(severity, T_string(message_in, \
			" >>> Line: ", std::to_string(__LINE__),	\
			" | File: ", __FILE__,						\
			" | Function: ", __PRETTY_FUNCTION__));

// -Basic loggers (Log File + Live)
#define LOG_FATAL(message)						_PRINT_MESSAGE_INFO(LOG_SEVERITY_FATAL, message);	// Send a FATAL log message with location info
#define LOG_FATAL_MIN(message)					Logger::PrintLog(LOG_SEVERITY_FATAL, message);		// Send a FATAL log message without location info
#define LOG_ERROR(message)						_PRINT_MESSAGE_INFO(LOG_SEVERITY_ERROR, message);	// Send a ERROR log message with location info
#define LOG_ERROR_MIN(message)					Logger::PrintLog(LOG_SEVERITY_ERROR, message);		// Send a ERROR log message without location info
#define LOG_WARNING(message)					_PRINT_MESSAGE_INFO(LOG_SEVERITY_WARNING, message);	// Send a WARNING log message with location info
#define LOG_WARNING_MIN(message)				Logger::PrintLog(LOG_SEVERITY_WARNING, message);	// Send a WARNING log message without location info
// -Conditional loggers (Log File + Live)
#define LOG_FATAL_IF(condition, message)		if(condition){ _PRINT_MESSAGE_INFO(LOG_SEVERITY_FATAL, message); }	// Send a FATAL log message with location info if condition is true
#define LOG_FATAL_MIN_IF(condition, message)	if(condition){ Logger::PrintLog(LOG_SEVERITY_FATAL, message); }		// Send a FATAL log message without location info if condition is true
#define LOG_ERROR_IF(condition, message)		if(condition){ _PRINT_MESSAGE_INFO(LOG_SEVERITY_ERROR, message); }	// Send a ERROR log message with location info if condition is true
#define LOG_ERROR_MIN_IF(condition, message)	if(condition){ Logger::PrintLog(LOG_SEVERITY_ERROR, message); }		// Send a ERROR log message without location info if condition is true
#define LOG_WARNING_IF(condition, message)		if(condition){ _PRINT_MESSAGE_INFO(LOG_SEVERITY_WARNING, message); }// Send a WARNING log message with location info if condition is true
#define LOG_WARNING_MIN_IF(condition, message)	if(condition){ Logger::PrintLog(LOG_SEVERITY_WARNING, message); }	// Send a WARNING log message without location info if condition is true


// Prints assertion log FATAL if ptr == nullptr (Log File + Live)
#define ASSERT_PTR(ptr)																					\
	if (ptr == nullptr)	[[unlikely]]																	\
	{																									\
		_PRINT_MESSAGE_INFO(LOG_SEVERITY_FATAL, T_string("Assertion Failed: ", STRINGIFY(ptr), " == nullptr"));	\
	}

// Checks that ptr is not nullptr and allows you to scope your code if it's valid
#define ASSERT_PTR_THEN_DO(ptr)																					\
	if (ptr == nullptr)	[[unlikely]]																			\
	{																											\
		_PRINT_MESSAGE_INFO(LOG_SEVERITY_FATAL, T_string("Assertion Failed: ", STRINGIFY(ptr), " == nullptr"));	\
	}																											\
	else [[likely]] // Your code in scope here

// Prints assertion log WARNING if condition doesn't evaluate to true
#define ASSERT_TRUE(condition)																			 \
	if (!(condition)) [[unlikely]]																		 \
	{																									 \
		_PRINT_MESSAGE_INFO(LOG_SEVERITY_WARNING, T_string("Assertion Failed: ", STRINGIFY(condition))); \
	}

// -VULKAN SPECIFIC LOGGER MACRO-
// VKResult Handler
#define LOG_VKRESULT(func)																				\
	VkResult _MACRO_CONCAT( vkresult, __LINE__ ) = func;												\
	if (_MACRO_CONCAT( vkresult, __LINE__ ) != VK_SUCCESS)	[[unlikely]]								\
	{																									\
		_PRINT_MESSAGE_INFO(LOG_SEVERITY_ERROR,															\
			T_string("VkResult == ", string_VkResult(_MACRO_CONCAT( vkresult, __LINE__ ))));	\
	}

// --USE VERBOSE LOGGER--
#if defined(LAYER_USE_VERBOSE_LOGGER)
// -Basic loggers (Verbose)

#define LOG_INFO(message)						Logger::PrintLog(LOG_SEVERITY_INFO, message);			// Send a INFO log message
#define LOG_BENCHMARK(message)					Logger::PrintLog(LOG_SEVERITY_BENCHMARK, message);		// Send a BENCHMARK log message
#define LOG_OTHER(message)						Logger::PrintLog(LOG_SEVERITY_OTHER, message);			// Send a OTHER log message
// -Conditional loggers (Verbose)
#define LOG_INFO_IF(condition, message)			if(condition){ Logger::PrintLog(LOG_SEVERITY_INFO, message); }			// Send a INFO log message if condition is true
#define LOG_BENCHMARK_IF(condition, message)	if(condition){ Logger::PrintLog(LOG_SEVERITY_BENCHMARK, message); }		// Send a BENCHMARK log message if condition is true
#define LOG_OTHER_IF(condition, message)		if(condition){ Logger::PrintLog(LOG_SEVERITY_OTHER, message); }			// Send a OTHER log message if condition is true


// --NO VERBOSE LOGGER--
#else // Define all logging macros to blank
#define LOG_INFO(message)
#define LOG_BENCHMARK(message)
#define LOG_OTHER(message)	
#define LOG_INFO_IF(condition, message)	
#define LOG_BENCHMARK_IF(condition, message)
#define LOG_OTHER_IF(condition, message)
#endif // defined(LAYER_USE_VERBOSE_LOGGER)


// -Debug specific loggers/asserts
#ifdef LAYER_DEBUG
#define LOG_DEBUG(message)					Logger::PrintLog(LOG_SEVERITY_DEBUG, message);					// Send a DEBUG log message, disabled in non-debug builds
#define LOG_DEBUG_IF(condition, message)	if(condition){ Logger::PrintLog(LOG_SEVERITY_DEBUG, message); }	// Send a DEBUG log message if condition is true, disabled in non-debug builds

// Prints assertion log DEBUG if condition doesn't evaluate to true, disabled in non-debug builds
#define ASSERT_TRUE_DEBUG(condition)																			\
	if (!(condition)) [[unlikely]]																				\
	{																											\
		_PRINT_MESSAGE_INFO(LOG_SEVERITY_DEBUG, T_string("Debug Assertion Failed: ", STRINGIFY(condition)));	\
	}


#else // Define all Debug logging macros to blank in release mode
#define LOG_DEBUG(message)	
#define LOG_DEBUG_IF(condition, message)	
#define ASSERT_TRUE_DEBUG(condition)
#endif // _DEBUG


