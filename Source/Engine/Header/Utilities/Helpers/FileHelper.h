#pragma once
#include "ThirdParty.h"
#include "GlobalConstants.h"
#include "Logger.h"
#include "LayerContainers.h"
#include "StringHelper.h"


#ifndef FILE_HELPER_H
#define FILE_HELPER_H


namespace FileHelper
{
	inline T_string currentWorkingDirectory = {};

	inline bool ValidPath(const char* path)
	{
		for (u64 i = 0; path[i] != '\0'; i++)
		{
			const char c = path[i];
			if ( c == '*' || c == '?' || c == '\"' || c == '<' || c == '>' || c == '|')
				return false;
		}

		return true;
	}

	// Create a folder if it doesn't exist, does nothing if it does
	inline void CreateFolderIfAbsent(const char* folderPath, bool bFromCurrentWorkingDirectory = true)
	{
		if (!ValidPath(folderPath))
		{
			LOG_ERROR(T_string("Folder path: \"", folderPath, "\" contains invalid symbol ( *, ?, \", <, >, |) "));
			return;
		}

		T_string currentFolder("");
		if (bFromCurrentWorkingDirectory)
		{
			currentFolder  = currentWorkingDirectory;
		}

		for (u64 i = 0; folderPath[i] != '\0'; i++)
		{
			// We keep appending till we finish the folder and check if it exists and create if it it doesn't, we repeat this for every folder in the chain
			currentFolder.push_back(folderPath[i]);
			if ((folderPath[i] == '\\' || folderPath[i] == '/' || folderPath[i + 1] == '\0') && currentFolder.size() > 3)
			{
				std::filesystem::path path(currentFolder.c_str());

				// Create the folder if it doesn't exist
				if (!std::filesystem::exists(path) || !std::filesystem::is_directory(path))
				{
					std::filesystem::create_directories(path);
				}
			}
		}
	}

	inline void OpenFileInExternalProgram(const char* filePath, bool bFromCurrentWorkingDirectory = true)
	{
		if (!ValidPath(filePath))
		{
			LOG_ERROR(T_string("File path: \"", filePath, "\" contains invalid symbol ( :, *, ?, \", <, >, |) "));
			return;
		}

		T_string fullPath;
		if (bFromCurrentWorkingDirectory)
		{
			fullPath.AppendMany(currentWorkingDirectory, filePath);
		}
		else
		{
			fullPath = filePath;
		}

		if constexpr (GlobalConstants::bOnWindows)
		{
			HINSTANCE result = ShellExecuteA(NULL, "open", fullPath.c_str(), NULL, NULL, SW_SHOWNORMAL);

			if ((u64)result <= 32)
			{
				// Error occurred
				LOG_ERROR(T_string("Failed to open file: \"", fullPath, "\" result == ", std::to_string((u64)result)));
			}
			return;
		}


		//TODO: Confirm this works on Non windows machines
		const char* command;
		if constexpr (GlobalConstants::bOnLinux)
		{
			command = "xdg-open"; // Use xdg-open to open the default text editor
		}
		else if constexpr (GlobalConstants::bOnAppleOS)
		{
			command = "open"; // Use open to open the default text editor
		}
		else if constexpr (GlobalConstants::bOnAndroid)
		{
			//TODO: Implement android version
		}

		T_string fullCommand(command, " ", fullPath);

		// Execute the command to open the file
		u32 result = std::system(fullCommand.c_str());

		// Check if the command execution was successful
		if (result != 0)
		{
			// Error occurred
			LOG_ERROR(T_string("Failed to open file: \"", fullPath, "\" result == ", std::to_string((int)result)));
		}

	}

	inline void WriteStringToFile(const T_string& str, const char* filePath, bool bFromCurrentWorkingDirectory = true)
	{
		if (!ValidPath(filePath))
		{
			LOG_ERROR(T_string("File path: \"", filePath, "\" contains invalid symbol ( :, *, ?, \", <, >, |) "));
			return;
		}

		T_string fullPath;
		if (bFromCurrentWorkingDirectory)
		{
			fullPath.AppendMany(currentWorkingDirectory, filePath);
		}
		else
		{
			fullPath = filePath;
		}

		std::ofstream file(fullPath.c_str());

		if (file.is_open())
		{
			file << str;
			file.close();
		}

	}

} // namespace FileHelper

#endif //FILE_HELPER_H
