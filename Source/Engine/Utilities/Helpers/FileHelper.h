#pragma once
#include "ThirdParty.h"
#include "LayerContainers.h"


namespace FileHelper
{
    // Folder context of the current executable using the engine. It is up to the editor/game file manager to set this.
    // If Editor: It should be the project folder that the .layer file resides or the exe bin folder if no project is open.FileHelper::currentWorkingDirectory
    // If Game: It should be the root directory of the game.
	inline T_string currentWorkingDirectory = {};

	// Create a folder, if it doesn't exist, with an absolute path or a path relative to the current working directory (FileHelper::currentWorkingDirectory), does nothing if it does exist.
	void CreateFolderIfAbsent(const char* folderPath, bool bFromCurrentWorkingDirectory = true);

	// Tries to open a given file either with an absolute path or a path relative to the current working directory (FileHelper::currentWorkingDirectory)
	void OpenFileInExternalProgram(const char* filePath, bool bFromCurrentWorkingDirectory = true);

	// Writes string buffer to given file either with an absolute path or a path relative to the current working directory (FileHelper::currentWorkingDirectory)
	void WriteStringToFile(const T_string& str, const char* filePath, bool bFromCurrentWorkingDirectory = true);

} // namespace FileHelper

