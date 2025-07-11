# ContourSegment Installation Guide

## Overview

This guide will help you install and set up ContourSegment on your Windows system. The application requires Qt 6.5.3 and Visual Studio 2019/2022 with C++ support.

## System Requirements

### Minimum Requirements
- **Operating System**: Windows 10 (version 1903 or later)
- **Processor**: Intel Core i3 or AMD equivalent
- **Memory**: 4 GB RAM
- **Graphics**: DirectX 11 compatible graphics card
- **Storage**: 2 GB available space

### Recommended Requirements
- **Operating System**: Windows 11
- **Processor**: Intel Core i5 or AMD equivalent
- **Memory**: 8 GB RAM
- **Graphics**: DirectX 12 compatible graphics card
- **Storage**: 5 GB available space

## Prerequisites Installation

### 1. Visual Studio Installation

#### Download Visual Studio
1. Go to https://visualstudio.microsoft.com/downloads/
2. Download Visual Studio 2022 Community (free) or Professional/Enterprise
3. Run the installer

#### Install Required Components
During installation, select these workloads:
- **Desktop development with C++**
- **Universal Windows Platform development** (optional)

Required individual components:
- **MSVC v143 - VS 2022 C++ x64/x86 build tools**
- **Windows 10/11 SDK**
- **CMake tools for Visual Studio**

#### Verify Installation
```cmd
# Open Developer Command Prompt
# Start → Visual Studio 2022 → Developer Command Prompt

# Check compiler
cl /Bv

# Check MSBuild
msbuild /version
```

### 2. Qt Installation

#### Download Qt
1. Go to https://www.qt.io/download
2. Click "Download Qt"
3. Create a free Qt account (or use existing)
4. Download Qt Online Installer

#### Install Qt Components
Run the installer and select:
- **Qt 6.5.3** (latest stable)
- **MSVC 2019 64-bit** (required for Visual Studio)
- **Qt Creator** (optional IDE)
- **Qt Debug Information Files** (for debugging)

#### Installation Path
- **Default**: `C:\Qt\6.5.3\msvc2019_64`
- **Custom**: Choose a path without spaces

#### Verify Installation
```cmd
# Check Qt installation
dir C:\Qt\6.5.3\msvc2019_64\bin\qmake.exe

# Test Qt
C:\Qt\6.5.3\msvc2019_64\bin\qmake.exe -v
```

## ContourSegment Installation

### Method 1: Automated Setup (Recommended)

#### Step 1: Download Project
1. Clone or download the ContourSegment project
2. Extract to a directory (e.g., `C:\Projects\ContourSegment`)

#### Step 2: Setup Environment
```cmd
# Navigate to project directory
cd C:\Projects\ContourSegment

# Run setup script
setup_env.bat
```

#### Step 3: Build Application
```cmd
# Build in Release mode
build.bat

# Or build in Debug mode
build_debug.bat
```

#### Step 4: Run Application
```cmd
# Run the application
run.bat
```

### Method 2: Manual Setup

#### Step 1: Set Environment Variables
```cmd
# Set Qt directory
set QTDIR=C:\Qt\6.5.3\msvc2019_64

# Add Qt to PATH
set PATH=%QTDIR%\bin;%PATH%
```

#### Step 2: Build with Visual Studio
1. Open `ContourSegment.sln` in Visual Studio
2. Set configuration to `Release` and platform to `x64`
3. Build → Build Solution (Ctrl+Shift+B)

#### Step 3: Build with Command Line
```cmd
# Open Developer Command Prompt
# Navigate to project directory

# Build Release
msbuild ContourSegment.sln /p:Configuration=Release /p:Platform=x64

# Build Debug
msbuild ContourSegment.sln /p:Configuration=Debug /p:Platform=x64
```

### Method 3: Qt Creator Setup

#### Step 1: Open in Qt Creator
1. Install Qt Creator with Qt 6.5.3
2. Open `ContourSegment.pro` in Qt Creator
3. Configure kit for MSVC 2019 64-bit

#### Step 2: Build and Run
1. Click Build → Build Project
2. Click Run → Run (Ctrl+R)

## Verification

### Test Installation
```cmd
# Run diagnostics
troubleshoot.bat

# Run tests
test.bat

# Run demo
demo.bat
```

### Expected Output
- **Diagnostics**: All checks should pass
- **Tests**: All tests should pass
- **Demo**: Application should start with GUI

## Troubleshooting Installation

### Common Issues

#### Qt Not Found
```cmd
# Check Qt installation
dir C:\Qt\6.5.3\msvc2019_64\bin\qmake.exe

# If not found, reinstall Qt
install_qt.bat
```

#### Visual Studio Not Found
```cmd
# Check Visual Studio
where cl
where msbuild

# If not found, reinstall Visual Studio
# Include C++ workload
```

#### Build Errors
```cmd
# Clean and rebuild
clean.bat
build.bat

# Check for specific errors
troubleshoot.bat
```

### Environment Issues

#### PATH Problems
```cmd
# Check current PATH
echo %PATH%

# Add Qt to PATH manually
set PATH=C:\Qt\6.5.3\msvc2019_64\bin;%PATH%
```

#### Permission Issues
- Run as Administrator if needed
- Check antivirus software
- Verify Windows Defender settings

### Performance Issues

#### Slow Build
- Use SSD for project files
- Close other applications
- Use Release builds for testing

#### Memory Issues
- Close other applications
- Use Debug builds for development
- Check available RAM

## Post-Installation

### First Run
1. Start the application: `run.bat`
2. Verify GUI appears
3. Test basic drawing functionality
4. Check menu and toolbar

### Configuration
1. **Grid Settings**: View → Show Grid, Snap to Grid
2. **Zoom Controls**: Mouse wheel, Ctrl+Mouse wheel
3. **Drawing Tools**: Tools → Add Line, Add Arc

### Updates
1. **Check for Updates**: Monitor project repository
2. **Backup Work**: Save important contours
3. **Test After Updates**: Run tests after updates

## Uninstallation

### Remove Application
```cmd
# Clean build artifacts
clean.bat

# Remove executable
del x64\Release\ContourSegment.exe
del x64\Debug\ContourSegment.exe
```

### Remove Dependencies
- **Qt**: Uninstall via Control Panel
- **Visual Studio**: Uninstall via Control Panel
- **Environment Variables**: Remove QTDIR and Qt from PATH

## Support

### Getting Help
1. **Check Documentation**: README.md, user guides
2. **Run Diagnostics**: `troubleshoot.bat`
3. **Check Logs**: Console output, error messages
4. **Community Support**: Project issues/forums

### Useful Commands
```cmd
# Quick setup
setup_env.bat
build.bat
run.bat

# Development
dev.bat

# Troubleshooting
troubleshoot.bat
clean.bat
```

### Contact Information
- **Documentation**: Check docs/ folder
- **Issues**: Report via project repository
- **Questions**: Check troubleshooting guide

## Conclusion

After successful installation:
1. The application should start with a GUI
2. You can create and edit contours
3. All features should be functional
4. Tests should pass

If you encounter issues, refer to the troubleshooting guide or run the diagnostic script for assistance. 