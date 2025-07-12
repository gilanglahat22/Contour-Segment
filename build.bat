@echo off
echo ContourSegment Build Script
echo ===========================

REM Check if Qt is installed
if not defined QTDIR (
    echo Error: QTDIR environment variable not set
    echo Please set QTDIR to your Qt installation path
    echo Example: set QTDIR=C:\Qt\6.5.3\msvc2019_64
    pause
    exit /b 1
)

echo Qt Directory: %QTDIR%

REM Check if Visual Studio is available
where msbuild >nul 2>&1
if %errorlevel% neq 0 (
    echo Error: MSBuild not found
    echo Please run this script from Visual Studio Developer Command Prompt
    echo or add MSBuild to your PATH
    pause
    exit /b 1
)

echo Building ContourSegment...
echo.

REM Build the project
msbuild ContourSegment.sln /p:Configuration=Release /p:Platform=x64 /m

if %errorlevel% equ 0 (
    echo.
    echo Build successful!
    echo.
    echo Running application...
    echo.
    x64\Release\ContourSegment.exe
) else (
    echo.
    echo Build failed!
    pause
    exit /b 1
)

pause 