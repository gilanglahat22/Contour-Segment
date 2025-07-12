@echo off
echo Testing compilation without Qt...
echo.

REM Try to find cl.exe (Visual Studio compiler)
where cl >nul 2>&1
if %errorlevel% neq 0 (
    echo Error: Visual C++ compiler (cl.exe) not found
    echo Please run from Visual Studio Developer Command Prompt
    pause
    exit /b 1
)

echo Found Visual C++ compiler
echo.

REM Try to compile a simple test
echo Testing compilation of main.cpp...
cl /std:c++17 /EHsc /I"include" main.cpp src\*.cpp /Fe:test_compile.exe

if %errorlevel% equ 0 (
    echo.
    echo ✓ Compilation successful! Error std::span has been fixed.
    echo.
    del test_compile.exe
    del *.obj
) else (
    echo.
    echo ✗ Compilation failed. There may be other errors.
    echo.
    del *.obj
)

pause 