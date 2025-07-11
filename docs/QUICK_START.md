# ContourSegment Quick Start Guide

## Getting Started

### 1. Install Prerequisites
- **Visual Studio 2019/2022** with C++ workload
- **Qt 6.5.3** with MSVC 2019 64-bit components
- Run `install_qt.bat` for installation instructions

### 2. Setup Environment
```cmd
setup_env.bat
```

### 3. Build the Application
```cmd
build.bat
```

### 4. Run the Application
```cmd
run.bat
```

## Using the GUI

### Basic Drawing
1. **Start Drawing**: Click "Add Line" or press 'L'
2. **Create Segments**: Click and drag to create line segments
3. **Complete Contour**: Connect back to the starting point
4. **Validate**: Click "Validate" or press 'V' to check

### Navigation
- **Zoom**: Mouse wheel or Ctrl + Mouse wheel
- **Pan**: Middle mouse button or Alt + Left click
- **Reset View**: Ctrl + 0

### Grid System
- **Show Grid**: Toggle grid visibility
- **Snap to Grid**: Automatically align to grid points
- **Grid Size**: Configurable spacing (default: 20 units)

### Keyboard Shortcuts
- **Ctrl + N**: New contour
- **Ctrl + O**: Open contour
- **Ctrl + S**: Save contour
- **V**: Validate contour
- **L**: Add line mode
- **A**: Add arc mode

## Troubleshooting

### Common Issues
1. **Qt Not Found**: Run `setup_env.bat`
2. **Build Errors**: Run `troubleshoot.bat`
3. **Runtime Errors**: Check Qt DLL availability

### Getting Help
- Run `troubleshoot.bat` for environment diagnostics
- Check console output for error messages
- Verify Qt installation path in `QTDIR`

## Advanced Features

### File Operations
- **New**: Clear current contour
- **Open**: Load contour from JSON (planned)
- **Save**: Save contour to JSON (planned)

### Edit Operations
- **Clear**: Remove all segments
- **Undo/Redo**: Planned for future versions

### View Options
- **Show Grid**: Toggle grid display
- **Snap to Grid**: Enable grid snapping
- **Zoom Controls**: In, Out, Fit to View

## Development

### Building from Source
```cmd
# Using Visual Studio
msbuild ContourSegment.sln /p:Configuration=Release

# Using Qt Creator
# Open ContourSegment.pro and build
```

### Project Structure
```
ContourSegment/
├── main.cpp              # Application entry point
├── include/              # Header files
├── src/                  # Source files
├── forms/                # Qt UI files
├── tests/                # Unit tests
├── build.bat            # Build script
├── run.bat              # Run script
└── troubleshoot.bat     # Diagnostics
```

### Adding Features
1. **New Segment Types**: Extend `Segment` base class
2. **UI Enhancements**: Modify `MainWindow` and UI files
3. **File Formats**: Implement in `MainWindow` slots
4. **Validation Rules**: Update `Contour::isValid()`

## Support

For issues or questions:
1. Run `troubleshoot.bat` for diagnostics
2. Check console output for error messages
3. Verify Qt and Visual Studio installation
4. Ensure all prerequisites are installed

## Next Steps

After mastering the basics:
- Explore advanced contour creation techniques
- Learn about different segment types
- Understand validation and error handling
- Contribute to the project development 