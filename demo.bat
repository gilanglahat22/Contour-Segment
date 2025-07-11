@echo off
echo ContourSegment Demo Runner
echo =========================

REM Check if executable exists
if not exist "x64\Release\ContourSegment.exe" (
    if not exist "x64\Debug\ContourSegment.exe" (
        echo Error: ContourSegment.exe not found
        echo Please build the project first using build.bat
        pause
        exit /b 1
    ) else (
        echo Running demo with Debug version...
        x64\Debug\ContourSegment.exe --demo-only
    )
) else (
    echo Running demo with Release version...
    x64\Release\ContourSegment.exe --demo-only
)

echo.
echo Demo completed!
echo.

pause 