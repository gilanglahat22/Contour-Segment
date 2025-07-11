@echo off
echo ContourSegment - Contour Editor
echo ===============================

REM Check if executable exists
if not exist "x64\Release\ContourSegment.exe" (
    if not exist "x64\Debug\ContourSegment.exe" (
        echo Error: ContourSegment.exe not found
        echo Please build the project first using build.bat
        echo or open ContourSegment.sln in Visual Studio
        pause
        exit /b 1
    ) else (
        echo Running Debug version...
        x64\Debug\ContourSegment.exe
    )
) else (
    echo Running Release version...
    x64\Release\ContourSegment.exe
)

pause 