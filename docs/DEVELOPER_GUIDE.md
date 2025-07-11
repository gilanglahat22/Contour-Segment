# ContourSegment Developer Guide

## Introduction

This guide is for developers who want to extend or modify the ContourSegment application. It covers the architecture, code organization, and how to add new features.

## Architecture Overview

### Design Patterns
- **MVC (Model-View-Controller)**: Separates data, presentation, and logic
- **Observer Pattern**: Qt signals/slots for loose coupling
- **Factory Pattern**: Segment creation with factory functions
- **Strategy Pattern**: Different validation strategies

### Core Components

#### Model Layer
- `Contour`: Main data structure for contour management
- `Segment`: Base class for geometric segments
- `LineSegment`: Concrete line segment implementation
- `ArcSegment`: Concrete arc segment implementation

#### View Layer
- `MainWindow`: Main application window
- `ContourView`: Interactive graphics view
- `ContourScene`: Graphics scene for rendering

#### Controller Layer
- Event handling in view classes
- Business logic in model classes
- Signal/slot connections for updates

## Project Structure

```
ContourSegment/
├── main.cpp                 # Application entry point
├── include/                 # Header files
│   ├── Geometry.h          # 2D geometry primitives
│   ├── Segment.h           # Segment base class
│   ├── Contour.h           # Contour class
│   ├── ContourUtilities.h  # Utility functions
│   ├── ContourVisualizer.h # Visualization
│   ├── MainWindow.h        # Main window
│   ├── ContourScene.h      # Graphics scene
│   └── ContourView.h       # Graphics view
├── src/                    # Implementation files
│   ├── Geometry.cpp        # Geometry implementations
│   ├── Segment.cpp         # Segment implementations
│   ├── Contour.cpp         # Contour implementations
│   ├── ContourUtilities.cpp # Utility implementations
│   ├── ContourVisualizer.cpp # Visualization implementations
│   ├── MainWindow.cpp      # Main window implementation
│   ├── ContourScene.cpp    # Scene implementation
│   └── ContourView.cpp     # View implementation
├── forms/                  # Qt UI files
│   └── mainwindow.ui      # Main window UI
├── tests/                  # Unit tests
│   └── ContourTests.cpp   # Test suite
└── docs/                   # Documentation
    ├── QUICK_START.md     # Quick start guide
    ├── USER_GUIDE.md      # User guide
    └── DEVELOPER_GUIDE.md # This guide
```

## Core Classes

### Geometry Classes

#### Point2D
```cpp
class Point2D {
    double x, y;
    // Methods for point operations
};
```

#### Vector2D
```cpp
class Vector2D {
    double x, y;
    // Methods for vector operations
};
```

### Segment Classes

#### Segment (Base Class)
```cpp
class Segment {
public:
    virtual ~Segment() = default;
    virtual geometry::Point2D getStartPoint() const = 0;
    virtual geometry::Point2D getEndPoint() const = 0;
    virtual SegmentType getType() const = 0;
    virtual std::unique_ptr<Segment> clone() const = 0;
    virtual double getLength() const = 0;
    virtual bool isEqual(const Segment& other, double epsilon) const = 0;
    virtual geometry::Vector2D getDirection() const = 0;
    virtual geometry::Point2D getPointAt(double t) const = 0;
};
```

#### LineSegment
```cpp
class LineSegment : public Segment {
private:
    geometry::Point2D m_start, m_end;
public:
    LineSegment(const geometry::Point2D& start, const geometry::Point2D& end);
    // Implementation of virtual methods
};
```

#### ArcSegment
```cpp
class ArcSegment : public Segment {
private:
    geometry::Point2D m_center;
    double m_radius, m_startAngle, m_endAngle;
    bool m_clockwise;
public:
    ArcSegment(const geometry::Point2D& center, double radius, 
               double startAngle, double endAngle, bool clockwise);
    // Implementation of virtual methods
};
```

### Contour Class
```cpp
class Contour {
private:
    std::vector<std::unique_ptr<Segment>> m_segments;
    mutable std::optional<bool> m_validityCache;
    mutable double m_lastEpsilon;
public:
    // Segment management
    void addSegment(std::unique_ptr<Segment> segment);
    void insertSegment(size_type index, std::unique_ptr<Segment> segment);
    void removeSegment(size_type index);
    
    // Validation
    bool isValid(double epsilon = geometry::EPSILON) const;
    bool isClosed(double epsilon = geometry::EPSILON) const;
    
    // Iteration
    iterator begin() { invalidateCache(); return m_segments.begin(); }
    const_iterator begin() const { return m_segments.begin(); }
    iterator end() { invalidateCache(); return m_segments.end(); }
    const_iterator end() const { return m_segments.end(); }
};
```

## GUI Architecture

### MainWindow
- **Purpose**: Main application window and controller
- **Responsibilities**: Menu management, toolbar, status bar
- **Key Methods**:
  - `setupUI()`: Initialize user interface
  - `setupConnections()`: Connect signals and slots
  - Event handlers for menu actions

### ContourScene
- **Purpose**: Graphics scene for contour rendering
- **Responsibilities**: Drawing, mouse interaction, grid system
- **Key Methods**:
  - `addLineSegment()`: Add line to contour
  - `addArcSegment()`: Add arc to contour
  - `setShowGrid()`: Toggle grid visibility
  - `setSnapToGrid()`: Toggle grid snapping

### ContourView
- **Purpose**: Interactive view with zoom/pan
- **Responsibilities**: Navigation, mouse events, keyboard shortcuts
- **Key Methods**:
  - `wheelEvent()`: Handle zoom
  - `mousePressEvent()`: Handle panning
  - `keyPressEvent()`: Handle shortcuts

## Adding New Features

### 1. Adding New Segment Types

#### Step 1: Create Header File
```cpp
// include/BezierSegment.h
#pragma once
#include "Segment.h"

namespace contour {
    class BezierSegment : public Segment {
    private:
        geometry::Point2D m_start, m_end, m_control1, m_control2;
    public:
        BezierSegment(const geometry::Point2D& start, 
                     const geometry::Point2D& control1,
                     const geometry::Point2D& control2,
                     const geometry::Point2D& end);
        
        // Implement virtual methods
        geometry::Point2D getStartPoint() const override;
        geometry::Point2D getEndPoint() const override;
        SegmentType getType() const override;
        std::unique_ptr<Segment> clone() const override;
        double getLength() const override;
        bool isEqual(const Segment& other, double epsilon) const override;
        geometry::Vector2D getDirection() const override;
        geometry::Point2D getPointAt(double t) const override;
    };
}
```

#### Step 2: Create Implementation File
```cpp
// src/BezierSegment.cpp
#include "../include/BezierSegment.h"
#include <cmath>

namespace contour {
    BezierSegment::BezierSegment(const geometry::Point2D& start,
                                 const geometry::Point2D& control1,
                                 const geometry::Point2D& control2,
                                 const geometry::Point2D& end)
        : m_start(start), m_end(end), m_control1(control1), m_control2(control2)
    {
        // Validation logic
    }
    
    // Implement all virtual methods
    geometry::Point2D BezierSegment::getStartPoint() const {
        return m_start;
    }
    
    geometry::Point2D BezierSegment::getEndPoint() const {
        return m_end;
    }
    
    SegmentType BezierSegment::getType() const {
        return SegmentType::Bezier;
    }
    
    std::unique_ptr<Segment> BezierSegment::clone() const {
        return std::make_unique<BezierSegment>(*this);
    }
    
    double BezierSegment::getLength() const {
        // Implement length calculation for Bezier curve
        // This is a simplified approximation
        return m_start.distanceTo(m_control1) + 
               m_control1.distanceTo(m_control2) + 
               m_control2.distanceTo(m_end);
    }
    
    bool BezierSegment::isEqual(const Segment& other, double epsilon) const {
        if (other.getType() != SegmentType::Bezier) return false;
        
        const auto& otherBezier = static_cast<const BezierSegment&>(other);
        return m_start.isEqual(otherBezier.m_start, epsilon) &&
               m_end.isEqual(otherBezier.m_end, epsilon) &&
               m_control1.isEqual(otherBezier.m_control1, epsilon) &&
               m_control2.isEqual(otherBezier.m_control2, epsilon);
    }
    
    geometry::Vector2D BezierSegment::getDirection() const {
        return (m_end - m_start).normalized();
    }
    
    geometry::Point2D BezierSegment::getPointAt(double t) const {
        if (t < 0.0 || t > 1.0) {
            throw std::invalid_argument("BezierSegment::getPointAt: t must be in range [0,1]");
        }
        
        // Cubic Bezier curve formula
        double t2 = t * t;
        double t3 = t2 * t;
        double mt = 1.0 - t;
        double mt2 = mt * mt;
        double mt3 = mt2 * mt;
        
        return m_start * mt3 + 
               m_control1 * (3 * mt2 * t) + 
               m_control2 * (3 * mt * t2) + 
               m_end * t3;
    }
}
```

#### Step 3: Update SegmentType Enum
```cpp
// include/Segment.h
enum class SegmentType {
    Line,
    Arc,
    Bezier  // Add new type
};
```

#### Step 4: Add Factory Function
```cpp
// include/ContourUtilities.h
std::unique_ptr<Segment> createBezierSegment(const geometry::Point2D& start,
                                            const geometry::Point2D& control1,
                                            const geometry::Point2D& control2,
                                            const geometry::Point2D& end);
```

#### Step 5: Update Project Files
- Add new files to `ContourSegment.vcxproj`
- Add new files to `ContourSegment.pro`
- Update filters in `ContourSegment.vcxproj.filters`

### 2. Adding UI Features

#### Adding Menu Items
```cpp
// In MainWindow::setupUI()
QMenu *toolsMenu = menuBar()->addMenu(tr("Tools"));
m_actionAddBezier = toolsMenu->addAction(tr("Add Bezier"), 
                                        this, &MainWindow::on_actionAddBezier_triggered);
```

#### Adding Event Handlers
```cpp
// In MainWindow.h
private slots:
    void on_actionAddBezier_triggered();

// In MainWindow.cpp
void MainWindow::on_actionAddBezier_triggered()
{
    statusBar()->showMessage(tr("Click to set control points for Bezier curve"));
    // Implement Bezier drawing mode
}
```

#### Adding Drawing Mode
```cpp
// In ContourScene.h
enum class DrawingMode {
    None,
    Line,
    Arc,
    Bezier
};

private:
    DrawingMode m_drawingMode;
    geometry::Point2D m_bezierPoints[4];
    int m_bezierPointIndex;

// In ContourScene.cpp
void ContourScene::setDrawingMode(DrawingMode mode)
{
    m_drawingMode = mode;
    m_bezierPointIndex = 0;
}

void ContourScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_drawingMode == DrawingMode::Bezier) {
        handleBezierDrawing(event);
    } else {
        // Existing line/arc handling
    }
}

void ContourScene::handleBezierDrawing(QGraphicsSceneMouseEvent *event)
{
    QPointF scenePos = event->scenePos();
    geometry::Point2D worldPos = sceneToWorld(scenePos);
    
    m_bezierPoints[m_bezierPointIndex] = worldPos;
    m_bezierPointIndex++;
    
    if (m_bezierPointIndex == 4) {
        // Create Bezier segment
        auto segment = createBezierSegment(m_bezierPoints[0], m_bezierPoints[1],
                                         m_bezierPoints[2], m_bezierPoints[3]);
        m_contour.addSegment(std::move(segment));
        updateScene();
        emit contourModified();
        
        m_bezierPointIndex = 0;
        m_drawingMode = DrawingMode::None;
    }
}
```

### 3. Adding File I/O

#### JSON Serialization
```cpp
// In MainWindow.cpp
void MainWindow::on_actionSave_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save Contour"), "", tr("JSON Files (*.json)"));
        
    if (fileName.isEmpty()) return;
    
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, tr("Error"),
                           tr("Cannot write file %1").arg(fileName));
        return;
    }
    
    QJsonDocument doc = serializeContour(m_scene->getContour());
    file.write(doc.toJson());
    
    statusBar()->showMessage(tr("Saved contour to %1").arg(fileName));
}

QJsonDocument MainWindow::serializeContour(const contour::Contour& contour)
{
    QJsonObject root;
    QJsonArray segmentsArray;
    
    for (const auto& segment : contour) {
        QJsonObject segmentObj;
        segmentObj["type"] = static_cast<int>(segment.getType());
        
        switch (segment.getType()) {
            case contour::SegmentType::Line: {
                const auto& line = static_cast<const contour::LineSegment&>(segment);
                segmentObj["start"] = serializePoint(line.getStartPoint());
                segmentObj["end"] = serializePoint(line.getEndPoint());
                break;
            }
            case contour::SegmentType::Arc: {
                const auto& arc = static_cast<const contour::ArcSegment&>(segment);
                segmentObj["center"] = serializePoint(arc.getCenter());
                segmentObj["radius"] = arc.getRadius();
                segmentObj["startAngle"] = arc.getStartAngle();
                segmentObj["endAngle"] = arc.getEndAngle();
                segmentObj["clockwise"] = arc.isClockwise();
                break;
            }
            // Add cases for new segment types
        }
        
        segmentsArray.append(segmentObj);
    }
    
    root["segments"] = segmentsArray;
    return QJsonDocument(root);
}

QJsonObject MainWindow::serializePoint(const geometry::Point2D& point)
{
    QJsonObject obj;
    obj["x"] = point.x;
    obj["y"] = point.y;
    return obj;
}
```

## Testing

### Unit Tests
```cpp
// In tests/ContourTests.cpp
void testBezierSegment(TestSuite& suite)
{
    std::cout << "\n=== Testing BezierSegment ===" << std::endl;
    
    auto bezier = createBezierSegment(
        Point2D(0.0, 0.0),    // start
        Point2D(1.0, 1.0),    // control1
        Point2D(2.0, 1.0),    // control2
        Point2D(3.0, 0.0)     // end
    );
    
    suite.runTest("Bezier segment creation", bezier != nullptr);
    suite.runTest("Bezier start point", bezier->getStartPoint().isEqual(Point2D(0.0, 0.0)));
    suite.runTest("Bezier end point", bezier->getEndPoint().isEqual(Point2D(3.0, 0.0)));
    suite.runTest("Bezier type", bezier->getType() == SegmentType::Bezier);
    
    // Test point at parameter t
    auto pointAtHalf = bezier->getPointAt(0.5);
    suite.runTest("Bezier point at t=0.5", !std::isnan(pointAtHalf.x));
    
    // Test cloning
    auto cloned = bezier->clone();
    suite.runTest("Bezier cloning", cloned != nullptr);
    suite.runTest("Bezier equality", bezier->isEqual(*cloned));
}
```

### Integration Tests
```cpp
void testBezierContour(TestSuite& suite)
{
    std::cout << "\n=== Testing Bezier Contour ===" << std::endl;
    
    Contour contour;
    contour.addSegment(createBezierSegment(
        Point2D(0.0, 0.0), Point2D(1.0, 1.0), 
        Point2D(2.0, 1.0), Point2D(3.0, 0.0)
    ));
    contour.addSegment(createBezierSegment(
        Point2D(3.0, 0.0), Point2D(4.0, -1.0), 
        Point2D(5.0, -1.0), Point2D(6.0, 0.0)
    ));
    contour.addSegment(createBezierSegment(
        Point2D(6.0, 0.0), Point2D(5.0, 1.0), 
        Point2D(4.0, 1.0), Point2D(3.0, 0.0)
    ));
    contour.addSegment(createBezierSegment(
        Point2D(3.0, 0.0), Point2D(2.0, 1.0), 
        Point2D(1.0, 1.0), Point2D(0.0, 0.0)
    ));
    
    suite.runTest("Bezier contour creation", contour.size() == 4);
    suite.runTest("Bezier contour validation", contour.isValid());
    suite.runTest("Bezier contour closure", contour.isClosed());
}
```

## Performance Considerations

### Memory Management
- Use smart pointers (`std::unique_ptr`) for ownership
- Avoid unnecessary copying with move semantics
- Use `const` references for read-only access

### Caching
- Cache validation results with epsilon awareness
- Cache expensive calculations
- Use lazy evaluation where appropriate

### Optimization
- Profile with Visual Studio Profiler
- Use Release builds for performance testing
- Optimize hot paths identified by profiling

## Debugging

### Visual Studio Debugging
1. Set breakpoints in critical code paths
2. Use watch windows for complex objects
3. Enable Qt debug output with `QT_LOGGING_RULES`

### Qt Debugging
```cpp
// Enable debug output
qDebug() << "Debug message";
qWarning() << "Warning message";
qCritical() << "Critical error";

// Debug contour state
qDebug() << "Contour size:" << contour.size();
qDebug() << "Contour valid:" << contour.isValid();
```

### Common Debugging Scenarios
1. **Segmentation Faults**: Check null pointer access
2. **Memory Leaks**: Use Visual Studio Memory Profiler
3. **UI Issues**: Check signal/slot connections
4. **Performance**: Profile with Visual Studio Profiler

## Best Practices

### Code Style
- Follow existing naming conventions
- Use meaningful variable and function names
- Add comments for complex logic
- Keep functions small and focused

### Error Handling
- Use exceptions for exceptional cases
- Validate input parameters
- Provide meaningful error messages
- Log errors for debugging

### Testing
- Write unit tests for new features
- Test edge cases and error conditions
- Maintain test coverage
- Run tests before committing changes

### Documentation
- Update header comments for new classes
- Document public interfaces
- Update user guide for new features
- Add examples for complex usage

## Deployment

### Building Release Version
```cmd
msbuild ContourSegment.sln /p:Configuration=Release /p:Platform=x64
```

### Dependencies
- Qt 6.5.3 runtime libraries
- Visual C++ redistributables
- Windows 10 or later

### Distribution
- Include Qt DLLs in distribution
- Test on clean systems
- Provide installation instructions
- Create installer package

## Conclusion

This developer guide provides the foundation for extending ContourSegment. The modular architecture makes it easy to add new features while maintaining code quality and performance.

Remember to:
- Follow the established patterns
- Write comprehensive tests
- Document your changes
- Consider performance implications
- Test thoroughly before releasing

The application is designed to be extensible, so don't hesitate to add new features that enhance its capabilities. 