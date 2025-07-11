@echo off
echo ContourSegment Test Runner
echo =========================

REM Check if executable exists
if not exist "x64\Release\ContourSegment.exe" (
    if not exist "x64\Debug\ContourSegment.exe" (
        echo Error: ContourSegment.exe not found
        echo Please build the project first using build.bat
        pause
        exit /b 1
    ) else (
        echo Running tests with Debug version...
        x64\Debug\ContourSegment.exe --test-only
    )
) else (
    echo Running tests with Release version...
    x64\Release\ContourSegment.exe --test-only
)

echo.
echo Tests completed!
echo.

pause 