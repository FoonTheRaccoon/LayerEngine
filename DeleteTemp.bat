@echo off
setlocal enabledelayedexpansion

rem List of folder names to be deleted
set "foldersToDelete=Bin build out"

rem Get the current directory of the batch script
for %%A in ("%~dp0.") do set "currentDir=%%~fA"

rem Loop through each folder in the list and delete them
for %%F in (%foldersToDelete%) do (
    set "folderPath=!currentDir!\%%F"
    if exist "!folderPath!" (
        echo Deleting folder: !folderPath!
        rmdir /s /q "!folderPath!"
    ) else (
        echo Folder not found: !folderPath!
    )
)

endlocal