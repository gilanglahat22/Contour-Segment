@echo off
echo ContourSegment - Qt Installation Guide
echo =====================================

echo.
echo This script will help you install Qt for ContourSegment
echo.

echo Prerequisites:
echo - Windows 10 or later
echo - Visual Studio 2019 or later with C++ workload
echo - Internet connection for download
echo.

echo Qt Installation Steps:
echo 1. Go to https://www.qt.io/download
echo 2. Click "Download Qt"
echo 3. Create a free Qt account (or use existing)
echo 4. Download Qt Online Installer
echo 5. Run the installer
echo 6. Select Qt 6.5.3 with MSVC 2019 64-bit
echo 7. Install to C:\Qt (default location)
echo.

echo After installation:
echo 1. Run setup_env.bat to configure environment
echo 2. Run build.bat to build the project
echo 3. Run run.bat to start the application
echo.

echo Alternative: Use Qt Creator
echo - Install Qt Creator with Qt 6.5.3
echo - Open ContourSegment.pro in Qt Creator
echo - Configure kit and build
echo.

echo Press any key to open Qt download page...
pause >nul

start https://www.qt.io/download

echo.
echo Qt download page opened in your browser
echo After installation, run setup_env.bat
echo.

pause 