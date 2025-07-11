# ContourSegment Troubleshooting Guide

## Quick Diagnosis

Run the diagnostic script to check your environment:
```cmd
troubleshoot.bat
```

This will check:
- Qt installation and configuration
- Visual Studio tools availability
- Project file integrity
- Build artifacts status

## Common Issues and Solutions

### 1. Build Issues

#### Qt Not Found
**Symptoms**: Build errors about missing Qt headers or libraries
```
fatal error C1083: Cannot open include file: 'QtCore/QApplication': No such file or directory
```

**Solutions**:
1. **Check Qt Installation**:
   ```cmd
   setup_env.bat
   ```

2. **Verify QTDIR Environment Variable**:
   ```cmd
   echo %QTDIR%
   ```
   Should show path like: `C:\Qt\6.5.3\msvc2019_64`

3. **Manual Qt Setup**:
   ```cmd
   set QTDIR=C:\Qt\6.5.3\msvc2019_64
   set PATH=%QTDIR%\bin;%PATH%
   ```

4. **Reinstall Qt**:
   - Download Qt 6.5.3 from https://www.qt.io/download
   - Install with MSVC 2019 64-bit components
   - Run `install_qt.bat` for instructions

#### Visual Studio Not Found
**Symptoms**: MSBuild not found, compiler errors
```
'msbuild' is not recognized as an internal or external command
```

**Solutions**:
1. **Use Developer Command Prompt**:
   - Start → Visual Studio 2019 → Developer Command Prompt
   - Navigate to project directory
   - Run build commands

2. **Install Visual Studio**:
   - Download Visual Studio 2019/2022 Community
   - Install with C++ workload
   - Include MSVC v143 toolset

3. **Add to PATH**:
   ```cmd
   set PATH=%PATH%;C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin
   ```

#### C++20 Compiler Issues
**Symptoms**: Compiler errors about C++20 features
```
error C2039: 'span': is not a member of 'std'
```

**Solutions**:
1. **Update Visual Studio**: Use VS 2019 16.11+ or VS 2022
2. **Check Compiler Version**:
   ```cmd
   cl /Bv
   ```
   Should show v143 toolset

3. **Verify C++20 Support**:
   - Project settings → C/C++ → Language → C++ Language Standard → ISO C++20 Standard

### 2. Runtime Issues

#### Missing Qt DLLs
**Symptoms**: Application won't start, missing DLL errors
```
The program can't start because Qt6Core.dll is missing
```

**Solutions**:
1. **Copy Qt DLLs**:
   ```cmd
   copy %QTDIR%\bin\Qt6Core.dll x64\Release\
   copy %QTDIR%\bin\Qt6Gui.dll x64\Release\
   copy %QTDIR%\bin\Qt6Widgets.dll x64\Release\
   ```

2. **Add Qt to PATH**:
   ```cmd
   set PATH=%QTDIR%\bin;%PATH%
   ```

3. **Install Visual C++ Redistributables**:
   - Download from Microsoft
   - Install both x86 and x64 versions

#### Graphics Issues
**Symptoms**: Blank window, rendering problems, crashes
```
OpenGL error: Invalid operation
```

**Solutions**:
1. **Update Graphics Drivers**:
   - Download latest drivers from manufacturer
   - Restart after installation

2. **Check OpenGL Support**:
   ```cmd
   set QT_OPENGL=software
   ContourSegment.exe
   ```

3. **Disable Hardware Acceleration**:
   - Add to environment: `QT_OPENGL=software`

#### Memory Issues
**Symptoms**: Crashes, memory leaks, performance problems
```
Exception thrown: std::bad_alloc
```

**Solutions**:
1. **Check Available Memory**:
   - Close other applications
   - Free up system memory

2. **Use Debug Build**:
   ```cmd
   build_debug.bat
   ```

3. **Enable Memory Profiling**:
   - Visual Studio → Debug → Performance Profiler
   - Select Memory Usage

### 3. GUI Issues

#### Window Not Appearing
**Symptoms**: Application starts but no window visible
```
Process started but no GUI visible
```

**Solutions**:
1. **Check Task Manager**:
   - Look for ContourSegment.exe in processes
   - End task if stuck

2. **Run from Command Line**:
   ```cmd
   ContourSegment.exe
   ```
   Check for error messages

3. **Check Display Settings**:
   - Verify display scaling
   - Check multiple monitor setup

#### Menu/Toolbar Not Working
**Symptoms**: GUI elements unresponsive
```
Buttons don't respond to clicks
```

**Solutions**:
1. **Check Event Loop**:
   - Ensure QApplication is created
   - Verify signal/slot connections

2. **Debug UI**:
   ```cpp
   qDebug() << "Button clicked";
   ```

3. **Recreate UI**:
   - Clean build: `clean.bat`
   - Rebuild: `build.bat`

#### Drawing Issues
**Symptoms**: Can't draw, segments not appearing
```
Mouse clicks not creating segments
```

**Solutions**:
1. **Check Drawing Mode**:
   - Verify correct mode is active
   - Check mouse event handling

2. **Debug Mouse Events**:
   ```cpp
   qDebug() << "Mouse press at:" << event->pos();
   ```

3. **Check Scene Setup**:
   - Verify scene is properly initialized
   - Check view/scene connections

### 4. Performance Issues

#### Slow Rendering
**Symptoms**: Laggy interface, slow drawing
```
GUI feels unresponsive
```

**Solutions**:
1. **Use Release Build**:
   ```cmd
   build.bat
   ```

2. **Optimize Rendering**:
   - Reduce scene complexity
   - Use hardware acceleration

3. **Profile Performance**:
   - Visual Studio Profiler
   - Identify bottlenecks

#### Memory Leaks
**Symptoms**: Increasing memory usage
```
Memory usage grows over time
```

**Solutions**:
1. **Use Smart Pointers**:
   - Ensure proper ownership
   - Avoid raw pointers

2. **Check Qt Object Hierarchy**:
   - Verify parent-child relationships
   - Use Qt's object tree

3. **Memory Profiling**:
   - Visual Studio Memory Profiler
   - Identify leaks

### 5. Development Issues

#### Debugging Problems
**Symptoms**: Can't debug, breakpoints not working
```
Debugger not attaching
```

**Solutions**:
1. **Check Debug Configuration**:
   - Verify Debug build
   - Check debug symbols

2. **Use Debug Output**:
   ```cpp
   qDebug() << "Debug message";
   ```

3. **Enable Qt Debugging**:
   ```cmd
   set QT_LOGGING_RULES=*.debug=true
   ```

#### Test Failures
**Symptoms**: Unit tests failing
```
Test suite reports failures
```

**Solutions**:
1. **Run Tests Individually**:
   ```cmd
   test.bat
   ```

2. **Check Test Environment**:
   - Verify test data
   - Check epsilon values

3. **Debug Tests**:
   ```cpp
   std::cout << "Test value:" << value << std::endl;
   ```

## Advanced Troubleshooting

### Environment Variables
```cmd
# Required for Qt
set QTDIR=C:\Qt\6.5.3\msvc2019_64
set PATH=%QTDIR%\bin;%PATH%

# Optional for debugging
set QT_LOGGING_RULES=*.debug=true
set QT_OPENGL=software
```

### Build Commands
```cmd
# Clean build
clean.bat
build.bat

# Debug build
build_debug.bat

# Test build
test.bat
```

### Logging and Debugging
```cpp
// Add to code for debugging
qDebug() << "Debug message";
qWarning() << "Warning message";
qCritical() << "Critical error";

// Check contour state
qDebug() << "Contour size:" << contour.size();
qDebug() << "Contour valid:" << contour.isValid();
```

### System Requirements Check
```cmd
# Check Windows version
ver

# Check available memory
wmic computersystem get TotalPhysicalMemory

# Check graphics card
wmic path win32_VideoController get name

# Check Visual Studio
where cl
where msbuild
```

## Getting Help

### Before Asking for Help
1. **Run Diagnostics**:
   ```cmd
   troubleshoot.bat
   ```

2. **Check Logs**:
   - Console output
   - Qt debug messages
   - Visual Studio output

3. **Reproduce Issue**:
   - Document exact steps
   - Note system configuration
   - Include error messages

### Information to Provide
- **System Information**: Windows version, memory, graphics
- **Software Versions**: Qt, Visual Studio, compiler
- **Error Messages**: Exact text and context
- **Steps to Reproduce**: Detailed sequence
- **Expected vs Actual**: What should happen vs what happens

### Support Channels
1. **Documentation**: Check README.md and user guides
2. **Scripts**: Use provided batch files
3. **Debugging**: Use Visual Studio debugger
4. **Community**: Check project issues/forums

## Prevention

### Best Practices
1. **Regular Updates**: Keep Qt and Visual Studio updated
2. **Clean Builds**: Use `clean.bat` before major changes
3. **Testing**: Run tests after modifications
4. **Backup**: Keep working versions of code
5. **Documentation**: Update docs with changes

### Environment Management
1. **Use Scripts**: Rely on provided batch files
2. **Check Dependencies**: Verify all requirements
3. **Isolate Issues**: Test in clean environment
4. **Version Control**: Use git for code management

## Conclusion

Most issues can be resolved by:
1. Running `troubleshoot.bat` for diagnosis
2. Checking environment variables and paths
3. Using clean builds and proper tools
4. Following the error messages and logs

If problems persist, collect the diagnostic information and provide it when seeking help. 