@echo off
echo Setting up Qt environment...

REM Try to find Qt installation
set QTDIR=

REM Check common Qt installation paths
if exist "C:\Qt\6.5.3\msvc2019_64" (
    set QTDIR=C:\Qt\6.5.3\msvc2019_64
    echo Found Qt at: %QTDIR%
) else if exist "C:\Qt\6.5.2\msvc2019_64" (
    set QTDIR=C:\Qt\6.5.2\msvc2019_64
    echo Found Qt at: %QTDIR%
) else if exist "C:\Qt\6.5.1\msvc2019_64" (
    set QTDIR=C:\Qt\6.5.1\msvc2019_64
    echo Found Qt at: %QTDIR%
) else if exist "C:\Qt\6.5.0\msvc2019_64" (
    set QTDIR=C:\Qt\6.5.0\msvc2019_64
    echo Found Qt at: %QTDIR%
) else (
    echo Qt not found in common locations.
    echo Please install Qt 6.5.x from https://www.qt.io/download
    echo or set QTDIR environment variable manually.
    pause
    exit /b 1
)

REM Set environment variables
set PATH=%QTDIR%\bin;%PATH%
set INCLUDE=%QTDIR%\include;%INCLUDE%
set LIB=%QTDIR%\lib;%LIB%

echo Environment variables set:
echo QTDIR=%QTDIR%
echo PATH includes Qt bin directory
echo INCLUDE includes Qt include directory
echo LIB includes Qt lib directory

echo.
echo Environment setup complete!
pause 