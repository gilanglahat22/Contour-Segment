# ContourSegment

A contour editor application for creating and manipulating geometric contours using Qt6 and C++.

## Features

- **Interactive Drawing**: Click and drag to draw line segments
- **Grid System**: Optional grid with snap-to-grid functionality
- **Zoom & Pan**: Ctrl+Wheel for zoom, Alt+Left Click for pan
- **Contour Validation**: Automatic validation of contour integrity
- **Modern GUI**: Clean interface with menu bar, toolbar, and status bar

## Prerequisites

- **Visual Studio 2019/2022** with C++ development tools
- **Qt 6.5.x** with MSVC 2019 64-bit components
- **Windows 10/11** (x64)

## Installation

### 1. Install Qt 6.5.x

1. Download Qt 6.5.x from [https://www.qt.io/download](https://www.qt.io/download)
2. During installation, select:
   - Qt 6.5.x
   - MSVC 2019 64-bit
   - Qt Creator (optional)
3. Install to default location: `C:\Qt\6.5.x\msvc2019_64`

### 2. Setup Environment

Run the setup script to configure environment variables:

```cmd
setup_env.bat
```

### 3. Build the Project
1. Open `ContourSegment.sln` in Visual Studio
2. Select `Debug` configuration and `x64` platform
3. Build the solution (Ctrl+Shift+B)

## Usage

### Drawing Contours

1. **Start the application**: Run `ContourSegment.exe`
2. **Draw segments**: Click and drag to create line segments
3. **Grid snapping**: Enable "Snap to Grid" for precise drawing
4. **View grid**: Toggle "Show Grid" to see the grid

### Navigation

- **Zoom**: Ctrl + Mouse Wheel
- **Pan**: Alt + Left Click and drag
- **Pan mode**: Press Space to toggle pan mode
- **Fit view**: Ctrl + 0 to fit contour to view

### File Operations

- **New**: Create a new contour (File → New)
- **Clear**: Remove all segments (Edit → Clear)
- **Save/Load**: File operations (planned for future versions)

## Project Structure

```
ContourSegment/
├── include/           # Header files
│   ├── Contour.h     # Contour class definition
│   ├── ContourScene.h # Qt scene for drawing
│   ├── MainWindow.h  # Main application window
│   └── ...
├── src/              # Source files
│   ├── Contour.cpp   # Contour implementation
│   ├── ContourScene.cpp # Qt scene implementation
│   ├── MainWindow.cpp # Main window implementation
│   └── ...
├── forms/            # Qt UI forms
├── docs/             # Documentation
├── tests/            # Unit tests
```

## Building from Source

### Requirements

- Visual Studio 2019/2022 with C++ tools
- Qt 6.5.x with MSVC 2019 64-bit
- CMake 3.16+ (optional, for alternative build)

### Build Steps

**Setup environment**:
   ```cmd
   setup_env.bat
   ```

## Troubleshooting

### Common Issues

1. **"QTDIR environment variable not set"**
   - Run `setup_env.bat` first
   - Ensure Qt is installed in the expected location

2. **"Qt libraries not found"**
   - Verify Qt installation path
   - Check that MSVC 2019 64-bit components are installed

3. **Build errors**
   - Ensure Visual Studio C++ tools are installed
   - Check that Qt version matches project configuration

### Getting Help

- Check the `docs/` directory for detailed documentation
- Review build logs for specific error messages

## Development

### Adding New Features

1. **UI Components**: Add to `MainWindow.h/cpp`
2. **Drawing Logic**: Modify `ContourScene.h/cpp`
3. **Contour Operations**: Extend `Contour.h/cpp`
4. **Tests**: Add to `tests/` directory

### Code Style

- Follow C++17/20 standards
- Use Qt naming conventions for Qt classes
- Include proper error handling
- Add unit tests for new functionality

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests if applicable
5. Submit a pull request

## Version History

- **v1.0.0**: Initial release with basic contour drawing functionality
- Basic line segment drawing
- Grid system with snap-to-grid
- Zoom and pan navigation
- Modern Qt6-based GUI 