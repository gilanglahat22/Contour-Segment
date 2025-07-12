# ContourSegment User Guide

## Introduction

ContourSegment is a modern C++ application for creating and editing 2D contours with a Qt-based graphical user interface. This guide will help you understand how to use all the features of the application.

## Getting Started

### Installation
1. **Install Prerequisites**:
   - Visual Studio 2019/2022 with C++ workload
   - Qt 6.5.3 with MSVC 2019 64-bit components
   - Run `install_qt.bat` for detailed instructions

2. **Setup Environment**:
   ```cmd
   setup_env.bat
   ```

3. **Build the Application**:
   ```cmd
   build.bat
   ```

4. **Run the Application**:
   ```cmd
   run.bat
   ```

## Main Interface

### Window Layout
- **Menu Bar**: File, Edit, Tools, View, Help menus
- **Toolbar**: Quick access to common functions
- **Graphics View**: Main drawing area
- **Status Bar**: Shows current contour status

### Menu System

#### File Menu
- **New (Ctrl+N)**: Create a new contour
- **Open (Ctrl+O)**: Load contour from file (planned)
- **Save (Ctrl+S)**: Save contour to file (planned)
- **Exit (Ctrl+Q)**: Close the application

#### Edit Menu
- **Undo (Ctrl+Z)**: Undo last action (planned)
- **Redo (Ctrl+Y)**: Redo last action (planned)
- **Clear**: Remove all segments from current contour

#### Tools Menu
- **Add Line (L)**: Enter line drawing mode
- **Add Arc (A)**: Enter arc drawing mode
- **Validate (V)**: Check contour validity

#### View Menu
- **Show Grid**: Toggle grid visibility
- **Snap to Grid**: Enable/disable grid snapping
- **Zoom In (Ctrl++)**: Increase zoom level
- **Zoom Out (Ctrl+-)**: Decrease zoom level
- **Zoom Fit (Ctrl+0)**: Fit contour to view

#### Help Menu
- **About**: Show application information

## Drawing Contours

### Creating Line Segments
1. **Start Drawing**:
   - Click "Add Line" in the toolbar or press 'L'
   - The cursor will change to indicate drawing mode

2. **Draw Segments**:
   - Click and drag to create line segments
   - Each segment connects to the previous one
   - Release mouse button to finish segment

3. **Complete Contour**:
   - Continue adding segments
   - Connect back to the starting point to close the contour
   - The application validates connectivity automatically

### Creating Arc Segments
1. **Start Arc Mode**:
   - Click "Add Arc" in the toolbar or press 'A'
   - Enter arc drawing mode

2. **Define Arc**:
   - Click to set center point
   - Drag to set radius and angles
   - Release to finish arc segment

### Grid System
- **Show Grid**: Toggle grid visibility on/off
- **Snap to Grid**: Automatically align points to grid
- **Grid Size**: Configurable spacing (default: 20 units)

## Navigation

### Mouse Controls
- **Left Click + Drag**: Create line segments
- **Middle Mouse**: Pan the view
- **Mouse Wheel**: Zoom in/out
- **Ctrl + Mouse Wheel**: Zoom with mouse position

### Keyboard Shortcuts
- **Ctrl + N**: New contour
- **Ctrl + O**: Open contour
- **Ctrl + S**: Save contour
- **Ctrl + Z**: Undo (planned)
- **Ctrl + Y**: Redo (planned)
- **V**: Validate contour
- **L**: Add line mode
- **A**: Add arc mode
- **Ctrl + 0**: Reset zoom to fit view
- **Ctrl + +**: Zoom in
- **Ctrl + -**: Zoom out

## Validation

### Contour Validation
The application automatically validates contours for:
- **Connectivity**: Segments must connect end-to-begin
- **Closure**: Contour should be closed (optional)
- **Minimum Segments**: At least 2 segments required

### Validation Status
- **Valid**: Contour meets all requirements
- **Invalid**: Issues found (check status bar for details)
- **Open**: Contour is not closed
- **Disconnected**: Segments don't connect properly

### Validation Tools
- **Real-time**: Automatic validation as you draw
- **Manual**: Click "Validate" or press 'V' for detailed check
- **Status Bar**: Shows current validation status

## Advanced Features

### View Controls
- **Zoom**: Multiple zoom levels with smooth transitions
- **Pan**: Navigate large contours easily
- **Fit to View**: Automatically scale to show entire contour
- **Grid**: Visual reference for precise drawing

### Drawing Modes
- **Line Mode**: Create straight line segments
- **Arc Mode**: Create curved segments (planned)
- **Freehand**: Draw without grid snapping (planned)

### File Operations
- **New**: Start fresh contour
- **Open**: Load existing contour (planned)
- **Save**: Save current contour (planned)
- **Export**: Export to various formats (planned)

## Troubleshooting

### Common Issues

#### Application Won't Start
1. **Check Qt Installation**:
   - Run `setup_env.bat`
   - Verify `QTDIR` environment variable
   - Ensure Qt libraries are in PATH

2. **Check Visual Studio**:
   - Install Visual Studio with C++ workload
   - Run from Developer Command Prompt

#### Build Errors
1. **Qt Not Found**:
   - Run `setup_env.bat`
   - Check Qt installation path
   - Verify Qt version compatibility

2. **Compiler Errors**:
   - Check C++20 support
   - Verify Visual Studio installation
   - Clean and rebuild project

#### Runtime Errors
1. **Missing DLLs**:
   - Check Qt DLL availability
   - Install Visual C++ redistributables
   - Run as administrator if needed

2. **Graphics Issues**:
   - Update graphics drivers
   - Check OpenGL support
   - Verify display settings

### Getting Help
1. **Run Diagnostics**:
   ```cmd
   troubleshoot.bat
   ```

2. **Check Console Output**:
   - Look for error messages
   - Check Qt debug output

3. **Verify Installation**:
   - Qt 6.5.3 with MSVC 2019 64-bit
   - Visual Studio 2019/2022
   - Windows 10 or later

## Tips and Best Practices

### Drawing Tips
1. **Use Grid Snapping**: Enable for precise alignment
2. **Plan Your Contour**: Think about the overall shape first
3. **Validate Frequently**: Check validity as you draw
4. **Use Zoom**: Zoom in for detailed work, out for overview

### Performance Tips
1. **Close Unused Applications**: Free up system resources
2. **Use Release Build**: Better performance than debug
3. **Limit Contour Size**: Very large contours may slow down
4. **Regular Saves**: Save work frequently (when implemented)

### Workflow Tips
1. **Start Simple**: Begin with basic shapes
2. **Iterate**: Refine contours gradually
3. **Test Validation**: Check validity at each step
4. **Backup Work**: Keep copies of important contours

## Future Features

### Planned Enhancements
- **File I/O**: Save and load contours
- **Undo/Redo**: History management
- **More Segment Types**: Bezier curves, splines
- **Advanced Tools**: Boolean operations, offsetting
- **Export Formats**: SVG, DXF, JSON
- **3D Support**: Extend to 3D geometry

### Development
- **Plugin System**: Extensible architecture
- **Scripting**: Python integration
- **Cloud Storage**: Online contour sharing
- **Collaboration**: Multi-user editing

## Support

### Documentation
- **README.md**: Project overview and setup
- **QUICK_START.md**: Getting started guide
- **USER_GUIDE.md**: This comprehensive guide

### Scripts
- **build.bat**: Build the application
- **run.bat**: Run the application
- **test.bat**: Run unit tests
- **troubleshoot.bat**: Diagnose issues
- **setup_env.bat**: Configure environment

### Getting Help
1. Check this user guide first
2. Run `troubleshoot.bat` for diagnostics
3. Check console output for error messages
4. Verify all prerequisites are installed
5. Contact development team for issues

## Conclusion

ContourSegment provides a powerful and intuitive interface for creating and editing 2D contours. With its modern Qt-based GUI, real-time validation, and comprehensive toolset, it's designed to make contour creation both efficient and enjoyable.

Start with simple shapes, explore the various tools, and gradually master the advanced features. The application is designed to grow with your needs, from basic line segments to complex multi-segment contours. 