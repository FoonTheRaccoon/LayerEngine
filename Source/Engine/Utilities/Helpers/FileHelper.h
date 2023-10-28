#pragma once
#include "ThirdParty.h"
#include "LayerContainers.h"


namespace FileHelper
{
	inline T_string currentWorkingDirectory = {};

	// Create a folder if it doesn't exist, does nothing if it does
	void CreateFolderIfAbsent(const char* folderPath, bool bFromCurrentWorkingDirectory = true);

	// Tries to open a given file either with an absolute path or a path relative to the current working directory
	void OpenFileInExternalProgram(const char* filePath, bool bFromCurrentWorkingDirectory = true);

	// Writes string buffer to given file either with an absolute path or a path relative to the current working directory
	void WriteStringToFile(const T_string& str, const char* filePath, bool bFromCurrentWorkingDirectory = true);

} // namespace FileHelper

