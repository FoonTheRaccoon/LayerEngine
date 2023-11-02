#include "EditorFileManager.h"
#include "Logger.h"
#include "FileHelper.h"
#include "ImGuiManager.h"

namespace EditorFileManager
{
    // Path, relative to EditorFileManager::editorCoreDirectory, where the imguiConfig.ini file should be stored
    constexpr const char* _editorImguiConfigDirPath = R"(EditorResources\Config\)";
    
    // Name of the imguiConfig.ini file
    constexpr const char* _editorImguiConfigFileName = "EditorImGuiConfig.ini";
    
    // Path to the editor exe
    T_string _editorExePath = {};
    
    #if LAYER_PLATFORM_WINDOWS
    void _ProcessRecentWindowsError(const T_string& errorMsgStr, const LSTATUS& result)
    {
        LPVOID errorMsg;
        FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, nullptr, result, 0, (LPSTR)&errorMsg, 0, nullptr);
        LOG_ERROR(T_string(errorMsgStr, " Windows error message: ", static_cast<LPCSTR>(errorMsg)))
        LocalFree(errorMsg);
    }
    #endif // LAYER_PLATFORM_WINDOWS
}

void EditorFileManager::SetupWorkingAndCoreDirectories()
{
    #if LAYER_PLATFORM_WINDOWS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // WINDOWS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    // Capture the Editor Core Directory and exe path
    char exePath[MAX_PATH];
    GetModuleFileNameA(nullptr, exePath, MAX_PATH);
    _editorExePath = exePath;
    T_string coreDirPath = exePath;
    
    u64 position = coreDirPath.find("Bin\\LayerEditor.exe");
    
    // Check if the snippet was found
    if (position != std::string::npos)
    {
        // Erase the snippet from the input string
        coreDirPath.erase(position, strlen("Bin\\LayerEditor.exe"));
    }
    
    EditorFileManager::editorCoreDirectory = coreDirPath;
    
    #elif LAYER_PLATFORM_LINUX || LAYER_PLATFORM_ANDROID
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // LINUX
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    // Capture the Editor Core Directory
	char exePath[PATH_MAX];
	ssize_t len = readlink("/proc/self/exe", exePath, sizeof(exePath) - 1);
 
	if (len != -1)
	{
		exePath[len] = '\0';
	}
    _editorExePath = exePath;
	T_string coreDirPath = exePath;

	// TODO: Change this to reflect the correct linux name and verify this works
	u64 position = coreDirPath.find("Bin\\LayerEditor.exe");

	// Check if the snippet was found
	if (position != std::string::npos)
	{
		// Erase the snippet from the input string
		coreDirPath.erase(position, strlen("Bin\\LayerEditor.exe"));
	}

	EditorFileManager::editorCoreDirectory = coreDirPath;
    
    #elif LAYER_PLATFORM_ALAYER_PLATFORM_APPLE
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // APPLE
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    // Capture the Editor Core Directory
	char exePath[1024];
	u32 size = sizeof(exePath);
	_NSGetExecutablePath(exePath, &size);

    _editorExePath = exePath;
	T_string coreDirPath = exePath;

	// TODO: Change this to reflect the correct apple name and verify this works
	u64 position = coreDirPath.find("Bin\\LayerEditor.exe");

	// Check if the snippet was found
	if (position != std::string::npos)
	{
		// Erase the snippet from the input string
		coreDirPath.erase(position, strlen("Bin\\LayerEditor.exe"));
	}

	EditorFileManager::editorCoreDirectory = coreDirPath;
    
    #endif
    
    // Make sure exe path was set
    LOG_ERROR_IF(_editorExePath.empty(), "Editor executable path was not set!")
    
    // Capture the working directory
    std::filesystem::path currentPath = std::filesystem::current_path();
    FileHelper::currentWorkingDirectory.AppendMany(currentPath.string(), "\\");
    
    // Set the name/path for the imguiConfig.ini file
    ImGuiManager::imguiConfigDirPath.AppendMany(EditorFileManager::editorCoreDirectory, _editorImguiConfigDirPath);
    ImGuiManager::imguiConfigFilePath.AppendMany(ImGuiManager::imguiConfigDirPath, _editorImguiConfigFileName);
}


void EditorFileManager::RegisterFileExtension()
{
    constexpr const char* extension = ".layer";
    
    #if LAYER_PLATFORM_WINDOWS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // WINDOWS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    constexpr const char* ftype = "LayerEditor";

    // Construct the registry key paths
    T_string extensionKey("Software\\Classes\\", extension);
    T_string ftypeKey("Software\\Classes\\", ftype);
    
    // Status checking helpers
    DWORD disposition;
    LSTATUS result;
    T_string errorMsgStr("");

    // Create the extension key and modify value
    HKEY hExtKey;
    result = RegCreateKeyExA(HKEY_CURRENT_USER, extensionKey.c_str(), 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &hExtKey, &disposition);
    if (result == ERROR_SUCCESS)
    {
        // Set the file extension association
        result = RegSetValueExA(hExtKey, nullptr, 0, REG_SZ, reinterpret_cast<const BYTE*>(ftype), static_cast<DWORD>(strlen(ftype)));
        if (result != ERROR_SUCCESS)
        {
            errorMsgStr = "Failed to set file extension value!";
            RegCloseKey(hExtKey);
            _ProcessRecentWindowsError(errorMsgStr, result);
            return;
        }
    }
    else
    {
        errorMsgStr = "Failed to create the extension key!";
        _ProcessRecentWindowsError(errorMsgStr, result);
        return;
    }
    RegCloseKey(hExtKey);


    // Create Default Icon Key and modify value
    HKEY hIconKey;
    T_string iconKeyPath = ftypeKey + "\\DefaultIcon";
    result = RegCreateKeyExA(HKEY_CURRENT_USER, iconKeyPath.c_str(), 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &hIconKey, &disposition);
    if (result == ERROR_SUCCESS)
    {
        // Set the default icon for the file type (replace "YourIconPath" with the actual path to your icon)
        T_string iconPath("\"", _editorExePath, R"(", 1)");
        result = RegSetValueExA(hIconKey, nullptr, 0, REG_SZ, reinterpret_cast<const BYTE*>(iconPath.c_str()), static_cast<DWORD>(strlen(iconPath.c_str())));
        if (result != ERROR_SUCCESS)
        {
            RegCloseKey(hIconKey);
            errorMsgStr = "Failed to set the DefaultIcon key value!";
            _ProcessRecentWindowsError(errorMsgStr, result);
            return;
        }
    }
    else
    {
        errorMsgStr = "Failed to create the DefaultIcon key!";
        _ProcessRecentWindowsError(errorMsgStr, result);
        return;
    }
    RegCloseKey(hIconKey);


    // Create Shell Open Command Key and modify value
    HKEY hOpenCommandKey;
    T_string openKeyPath = ftypeKey + R"(\shell\open\command)";
    result = RegCreateKeyExA(HKEY_CURRENT_USER, openKeyPath.c_str(), 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &hOpenCommandKey, &disposition);
    if (result == ERROR_SUCCESS)
    {
        // Set the command to open the file with your program
        T_string openCommand("\"", _editorExePath, R"(" "%1")");
        result = RegSetValueExA(hOpenCommandKey, nullptr, 0, REG_SZ, reinterpret_cast<const BYTE*>(openCommand.c_str()), static_cast<DWORD>(strlen(openCommand.c_str())));
        if (result != ERROR_SUCCESS)
        {
            RegCloseKey(hOpenCommandKey);
            errorMsgStr = "Failed to set the open command for the file type!";
            _ProcessRecentWindowsError(errorMsgStr, result);
            return;
        }
    }
    else
    {
        errorMsgStr = "Failed to create the Open Command key!";
        _ProcessRecentWindowsError(errorMsgStr, result);
        return;
    }
    RegCloseKey(hOpenCommandKey);
    
    #elif LAYER_PLATFORM_LINUX
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // LINUX
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    // TODO: Verify this works on linux.
    const char* desktopFilePath = "~/.local/share/applications/LayerEditor.desktop";
    // Create and write the .desktop file
    std::ofstream desktopFile(desktopFilePath);
    if (desktopFile.is_open()) {
        desktopFile << "[Desktop Entry]" << std::endl;
        desktopFile << "Version=1.0" << std::endl;
        desktopFile << "Type=Application" << std::endl;
        desktopFile << "Terminal=false" << std::endl;
        desktopFile << "Name=LayerEditor" << std::endl;
        desktopFile << "Exec=" << _editorExePath << " %f" << std::endl;
        desktopFile << "MimeType=application/" << extension << std::endl;
        desktopFile.close();
    }
    else
    {
        LOG_ERROR("Failed to set file extension! Reason: Failed to open destop file!");
    }
    
    #elif LAYER_PLATFORM_APPLE
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // APPLE
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    // TODO: Figure out how to register file a extension on Apple

    #endif
    
    // TODO: Does this apply to Android?
    
} // EditorFileManager::RegisterFileExtension




