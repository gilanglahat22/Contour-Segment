@echo off
echo ContourSegment Debug Build
echo ==========================

REM Check if Qt is installed
if not defined QTDIR (
    echo Error: QTDIR environment variable not set
    echo Please run setup_env.bat first
    pause
    exit /b 1
)

echo Qt Directory: %QTDIR%

REM Check if Visual Studio is available
where msbuild >nul 2>&1
if %errorlevel% neq 0 (
    echo Error: MSBuild not found
    echo Please run this script from Visual Studio Developer Command Prompt
    pause
    exit /b 1
)

echo Building ContourSegment in Debug mode...
echo.

REM Build the project in Debug mode
msbuild ContourSegment.sln /p:Configuration=Debug /p:Platform=x64 /m

if %errorlevel% equ 0 (
    echo.
    echo Debug build successful!
    echo.
    echo Running debug version...
    echo.
    x64\Debug\ContourSegment.exe
) else (
    echo.
    echo Debug build failed!
    pause
    exit /b 1
)

pause 