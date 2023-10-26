#include "Editor.h"
#include "EditorUtils.h"


void Run()
{
	// Capture the working directory
	std::filesystem::path currentPath = std::filesystem::current_path();
	FileHelper::currentWorkingDirectory.AppendMany(currentPath.string(), "\\");

	ImGuiManager::imguiConfigFilePath.AppendMany(EditorFileManager::editorCoreDirectory, "EditorResources\\Config\\");

	Editor editor;

	editor.StartUp();

	EditorFileManager::RegisterFileExtension();

	editor.RunGameLoop();
	editor.Shutdown();
}


#ifdef _WIN64
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	// Capture the Editor Core Directory
	char exePath[MAX_PATH];
	GetModuleFileNameA(NULL, exePath, MAX_PATH);
	T_string exePathString = exePath;

	u64 position = exePathString.find("Bin\\LayerEditor.exe");

	// Check if the snippet was found
	if (position != std::string::npos) 
	{
		// Erase the snippet from the input string
		exePathString.erase(position, strlen("Bin\\LayerEditor.exe"));
	}

	EditorFileManager::editorCoreDirectory = exePathString;

	// After the platform specific start up is done we start the editor
	Run();

	return EXIT_SUCCESS;
}
#endif // _WIN64

#ifdef __linux__
int main(int argc, char* argv[])
{
	// Capture the Editor Core Directory
	char exePath[PATH_MAX];
	ssize_t len = readlink("/proc/self/exe", exePath, sizeof(exePath) - 1);

	if (len != -1) 
	{
		exePath[len] = '\0';
	}

	T_string exePathString = exePath;

	// TODO: Change this to reflect the correct linux name and verify this works
	u64 position = exePathString.find("Layer Editor.exe");

	// Check if the snippet was found
	if (position != std::string::npos)
	{
		// Erase the snippet from the input string
		exePathString.erase(position, strlen("Layer Editor.exe"));
	}

	EditorFileManager::editorCoreDirectory = exePathString;

	// After the platform specific start up is done we start the editor
	Run();
}
#endif // __linux__

#ifdef __APPLE__
int main(int argc, char* argv[])
{
	// Capture the Editor Core Directory
	char exePath[1024];
	u32 size = sizeof(exePath);
	_NSGetExecutablePath(exePath, &size);

	T_string exePathString = exePath;

	// TODO: Change this to reflect the correct apple name and verify this works
	u64 position = exePathString.find("Layer Editor.exe");

	// Check if the snippet was found
	if (position != std::string::npos)
	{
		// Erase the snippet from the input string
		exePathString.erase(position, strlen("Layer Editor.exe"));
	}

	EditorFileManager::editorCoreDirectory = exePathString;

	// After the platform specific start up is done we start the editor
	Run();
}
#endif // __APPLE__


