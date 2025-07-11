@echo off
echo ContourSegment Troubleshooting
echo =============================

echo.
echo Checking environment...

REM Check Qt installation
if defined QTDIR (
    echo ✓ QTDIR is set to: %QTDIR%
    if exist "%QTDIR%\bin\qmake.exe" (
        echo ✓ Qt installation found
    ) else (
        echo ✗ Qt installation not found at %QTDIR%
    )
) else (
    echo ✗ QTDIR environment variable not set
)

echo.

REM Check Visual Studio
where msbuild >nul 2>&1
if %errorlevel% equ 0 (
    echo ✓ MSBuild found
) else (
    echo ✗ MSBuild not found - run from Visual Studio Developer Command Prompt
)

echo.

REM Check C++ compiler
where cl >nul 2>&1
if %errorlevel% equ 0 (
    echo ✓ Visual C++ compiler found
) else (
    echo ✗ Visual C++ compiler not found
)

echo.

REM Check if project files exist
if exist "ContourSegment.sln" (
    echo ✓ Visual Studio solution found
) else (
    echo ✗ ContourSegment.sln not found
)

if exist "ContourSegment.vcxproj" (
    echo ✓ Visual Studio project found
) else (
    echo ✗ ContourSegment.vcxproj not found
)

echo.

REM Check if executable exists
if exist "x64\Release\ContourSegment.exe" (
    echo ✓ Release executable found
) else (
    echo ✗ Release executable not found - build the project first
)

if exist "x64\Debug\ContourSegment.exe" (
    echo ✓ Debug executable found
) else (
    echo ✗ Debug executable not found - build the project first
)

echo.
echo Troubleshooting complete!
echo.

if exist "x64\Release\ContourSegment.exe" (
    echo Would you like to run the application? (Y/N)
    set /p choice=
    if /i "%choice%"=="Y" (
        echo Running ContourSegment...
        x64\Release\ContourSegment.exe
    )
)

pause 