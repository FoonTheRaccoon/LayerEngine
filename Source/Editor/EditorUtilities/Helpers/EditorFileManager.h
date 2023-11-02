#pragma once
#include "ThirdParty.h"
#include "LayerContainers.h"


namespace EditorFileManager
{
 
	inline T_string editorCoreDirectory = {};
    
    // Sets the EditorFileManager::editorCoreDirectory and FileHelper::currentWorkingDirectory
    void SetupWorkingAndCoreDirectories();
    
    // Registers the '.layer' extension with the OS
	void RegisterFileExtension();
 
}
