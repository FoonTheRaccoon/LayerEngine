#pragma once
#include "EngUtils.h"

#ifndef EDITOR_FILE_MANAGER_H
#define EDITOR_FILE_MANAGER_H


namespace EditorFileManager
{

	inline T_string editorCoreDirectory = {};

#ifdef _WIN64
	inline void _ProcessRecentWindowsError(const T_string& errorMsgStr, const LSTATUS& result)
	{
		LPVOID errorMsg;
		FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, result, 0, (LPSTR)&errorMsg, 0, NULL);
		LOG_ERROR(T_string(errorMsgStr, " Windows error message: ", static_cast<LPCSTR>(errorMsg)));
		LocalFree(errorMsg);
		return;
	}
#endif // _WIN64


	inline void RegisterFileExtension()
	{
		constexpr const char* extension = ".layer";

#ifdef _WIN64
		constexpr const char* ftype = "LayerEditor";

		// Construct the registry key paths
		T_string extensionKey("Software\\Classes\\", extension);
		T_string ftypeKey("Software\\Classes\\", ftype);

		char exePath[MAX_PATH];
		GetModuleFileNameA(NULL, exePath, MAX_PATH);
		

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


		// TODO: Have a custom icon for file type
		// Create Default Icon Key and modify value
		HKEY hIconKey;
		T_string iconKeyPath = ftypeKey + "\\DefaultIcon";
		result = RegCreateKeyExA(HKEY_CURRENT_USER, iconKeyPath.c_str(), 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &hIconKey, &disposition);
		if (result == ERROR_SUCCESS)
		{
			// Set the default icon for the file type (replace "YourIconPath" with the actual path to your icon)
			T_string iconPath("\"", exePath, "\", 1");
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
		T_string openKeyPath = ftypeKey + "\\shell\\open\\command";
		result = RegCreateKeyExA(HKEY_CURRENT_USER, openKeyPath.c_str(), 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &hOpenCommandKey, &disposition);
		if (result == ERROR_SUCCESS)
		{
			// Set the command to open the file with your program
			T_string openCommand("\"", exePath, "\" \"%1\"");
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
#elif defined __linux__
		// TODO: Verify this works on linux.
		const char* desktopFilePath = "~/.local/share/applications/LayerEditor.desktop";

		char exePath[PATH_MAX];
		ssize_t len = readlink("/proc/self/exe", exePath, sizeof(exePath) - 1);

		if (len != -1) 
		{
			exePath[len] = '\0';
		}
		else 
		{
			LOG_ERROR("Failed to set file extension! Reason: Failed to find program file path!");
			return;
		}

		// Create and write the .desktop file
		std::ofstream desktopFile(desktopFilePath);
		if (desktopFile.is_open()) {
			desktopFile << "[Desktop Entry]" << std::endl;
			desktopFile << "Version=1.0" << std::endl;
			desktopFile << "Type=Application" << std::endl;
			desktopFile << "Terminal=false" << std::endl;
			desktopFile << "Name=LayerEditor" << std::endl;
			desktopFile << "Exec=" << executablePath << " %f" << std::endl;
			desktopFile << "MimeType=application/" << extension << std::endl;
			desktopFile.close();
		}
		else
		{
			LOG_ERROR("Failed to set file extension! Reason: Failed to open destop file!");
		}
		
#elif defined __APPLE__
		// TODO: Figure this out and test it.
		// Define the path to the .plist file
		const char* plistFilePath = "~/Library/Preferences/com.apple.LaunchServices/com.apple.launchservices.secure.plist";

		// Create and write the .plist file
		std::ofstream plistFile(plistFilePath);
		if (plistFile.is_open()) {
			plistFile << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
			plistFile << "<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">" << std::endl;
			plistFile << "<plist version=\"1.0\">" << std::endl;
			plistFile << "<dict>" << std::endl;
			plistFile << "    <key>LSHandlers</key>" << std::endl;
			plistFile << "    <array>" << std::endl;
			plistFile << "        <dict>" << std::endl;
			plistFile << "            <key>LSHandlerContentTag</key>" << std::endl;
			plistFile << "            <string>" << extension << "</string>" << std::endl;
			plistFile << "            <key>LSHandlerRoleAll</key>" << std::endl;
			plistFile << "            <string>" << bundleIdentifier << "</string>" << std::endl;
			plistFile << "        </dict>" << std::endl;
			plistFile << "    </array>" << std::endl;
			plistFile << "</dict>" << std::endl;
			plistFile << "</plist>" << std::endl;
			plistFile.close();
			std::cout << "File extension association created successfully on macOS." << std::endl;
		}
		else {
			std::cerr << "Failed to create the .plist file." << std::endl;
		}

#endif
	}

}

#endif // EDITOR_FILE_MANAGER_H