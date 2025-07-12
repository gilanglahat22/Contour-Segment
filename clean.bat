@echo off
echo Cleaning ContourSegment Build Artifacts
echo ======================================

echo Removing build directories...
if exist "x64\Debug" (
    rmdir /s /q "x64\Debug"
    echo Removed x64\Debug
)

if exist "x64\Release" (
    rmdir /s /q "x64\Release"
    echo Removed x64\Release
)

if exist "x64" (
    rmdir /s /q "x64"
    echo Removed x64 directory
)

echo.
echo Clean completed!
echo You can now rebuild the project using build.bat
echo.

pause 