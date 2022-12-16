@echo off

set INSTALLER=x64\Release\softcam_installer.exe
set TARGET=dist\bin\x64\softcam.dll

echo ################################################################
echo Softcam Installer (softcam_installer.exe) will uninstall Softcam
echo (softcam.dll) from your system.
echo ################################################################
echo.

%INSTALLER% unregister %TARGET%

if %ERRORLEVEL% == 0 (
  echo.
  echo Successfully done.
  echo.
) else (
  echo.
  echo The process has been canceled or failed.
  echo.
)
pause
