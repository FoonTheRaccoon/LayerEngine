#include "FileHelper.h"
#include "Logger.h"
#include "StringHelper.h"

namespace FileHelper
{
	// Checks if given path is valid
	bool _ValidPath(const char* path);
}


void FileHelper::CreateFolderIfAbsent(const char* folderPath, bool bFromCurrentWorkingDirectory)
{
	if (!_ValidPath(folderPath)) return;

	T_string currentFolder("");
	if (bFromCurrentWorkingDirectory)
	{
		currentFolder = currentWorkingDirectory;
	}

	for (u64 i = 0; folderPath[i] != '\0'; i++)
	{
		// We keep appending till we finish the folder and check if it exists and create if it doesn't, we repeat this for every folder in the chain
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

void FileHelper::OpenFileInExternalProgram(const char* filePath, bool bFromCurrentWorkingDirectory)
{
	if (!_ValidPath(filePath)) return;

	T_string fullPath;
	if (bFromCurrentWorkingDirectory)
	{
		fullPath.AppendMany(currentWorkingDirectory, filePath);
	}
	else
	{
		fullPath = filePath;
	}
    
    #if LAYER_PLATFORM_WINDOWS
		HINSTANCE result = ShellExecuteA(nullptr, "open", fullPath.c_str(), nullptr, nullptr, SW_SHOWNORMAL);

		if ((u64)result <= 32)
		{
			// Error occurred
			LOG_ERROR(T_string("Failed to open file: \"", fullPath, "\" result == ", std::to_string((u64)result)))
		}
    #else
        //TODO: Confirm this works on Non windows machines
        const char* command;
        #if LAYER_PLATFORM_LINUX
            command = "xdg-open"; // Use xdg-open to open the default text editor
        #elif LAYER_PLATFORM_APPLE
            command = "open"; // Use open to open the default text editor
        #elif LAYER_PLATFORM_ALAYER_PLATFORM_ANDROID
            //TODO: Implement android version
        #endif
    
        T_string fullCommand(command, " ", fullPath);
    
        // Execute the command to open the file
        u32 result = std::system(fullCommand.c_str());
    
        // Check if the command execution was successful
        if (result != 0)
        {
            // Error occurred
            LOG_ERROR(T_string("Failed to open file: \"", fullPath, "\" result == ", std::to_string((int)result)))
        }
    #endif

}

void FileHelper::WriteStringToFile(const T_string& str, const char* filePath, bool bFromCurrentWorkingDirectory)
{
	if (!_ValidPath(filePath)) return;

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

bool FileHelper::_ValidPath(const char* path)
{
    for (u64 i = 0; path[i] != '\0'; i++)
    {
        const char c = path[i];
        if (c == '*' || c == '?' || c == '\"' || c == '<' || c == '>' || c == '|')
        {
            LOG_ERROR(T_string("Folder path: \"", path, "\" contains invalid symbol ( *, ?, \", <, >, |) "))
            return false;
        }
    }
    
    return true;
}