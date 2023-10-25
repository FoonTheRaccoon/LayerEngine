#pragma once
#include "ThirdParty.h"
#include "GlobalConstants.h"
#include "LayerContainers.h"
#include "Logger.h"



#ifndef STRING_HELPER_H
#define STRING_HELPER_H

namespace StringHelpers
{
	inline T_wstring CharStrToWideStr(const char* str)
	{
		// Convert the narrow string to a wide string and show error message
		u32 wcharsNum = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
		T_wstring wideStr;
		wideStr.reserve(wcharsNum);
		MultiByteToWideChar(CP_UTF8, 0, str, -1, wideStr.data(), wcharsNum);
		return wideStr;
	}

	inline T_wstring CharStrToWideStr(const T_string& str)
	{
		return CharStrToWideStr(str.c_str());
	}

	inline T_string WideStrToCharStr(const wchar_t* wideStr)
	{
		if (wideStr == nullptr) { return T_string(); }	// Return an empty string for nullptr input 

		size_t bufferSize = 0;

		// Get the required buffer size
		LOG_ERROR_IF(wcstombs_s(&bufferSize, nullptr, 0, wideStr, 0) != 0, "Wide char to char conversion error!");


		// Allocate a buffer for the conversion
		char* charBuffer = new char[bufferSize];

		// Perform the conversion
		if (wcstombs_s(nullptr, charBuffer, bufferSize, wideStr, bufferSize) != 0) 
		{
			// Conversion error
			delete[] charBuffer;
			LOG_ERROR("Wide char to char conversion error!");
			return T_string();
		}

		// Create a T_string from the converted char string
		T_string charStr(charBuffer);

		// Clean up the buffer
		delete[] charBuffer;

		return charStr;
	}


	inline u64 GetCstrSize(const char* str)
	{
		u64 index = 0;
		while (str[index] != '\0')
		{
			index++;
		}
		return index;
	}
}


#endif // STRING_HELPER_H
