@echo off
echo ContourSegment Development Tools
echo ================================

echo.
echo Available development tasks:
echo 1. Build Release
echo 2. Build Debug
echo 3. Run Tests
echo 4. Run Demo
echo 5. Clean Build
echo 6. Troubleshoot
echo 7. Setup Environment
echo 8. Exit
echo.

set /p choice="Enter your choice (1-8): "

if "%choice%"=="1" (
    call build.bat
) else if "%choice%"=="2" (
    call build_debug.bat
) else if "%choice%"=="3" (
    call test.bat
) else if "%choice%"=="4" (
    call demo.bat
) else if "%choice%"=="5" (
    call clean.bat
) else if "%choice%"=="6" (
    call troubleshoot.bat
) else if "%choice%"=="7" (
    call setup_env.bat
) else if "%choice%"=="8" (
    echo Goodbye!
    exit /b 0
) else (
    echo Invalid choice. Please enter 1-8.
    pause
    call dev.bat
)

pause 