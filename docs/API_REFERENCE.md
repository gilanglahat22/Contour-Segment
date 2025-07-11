# ContourSegment API Reference

## Overview

This document provides a comprehensive reference for the ContourSegment API, including all public classes, methods, and their usage.

## Core Classes

### Geometry Namespace

#### Point2D
```cpp
namespace geometry {
    class Point2D {
    public:
        double x, y;
        
        // Constructors
        Point2D() = default;
        Point2D(double x, double y);
        
        // Operators
        Point2D operator+(const Point2D& other) const;
        Point2D operator-(const Point2D& other) const;
        Point2D operator*(double scalar) const;
        Point2D operator/(double scalar) const;
        
        // Comparison
        bool operator==(const Point2D& other) const;
        bool operator!=(const Point2D& other) const;
        
        // Methods
        double distanceTo(const Point2D& other) const;
        bool isEqual(const Point2D& other, double epsilon = EPSILON) const;
        double magnitude() const;
        Point2D normalized() const;
    };
}
```

**Usage Example**:
```cpp
geometry::Point2D p1(1.0, 2.0);
geometry::Point2D p2(4.0, 6.0);
double distance = p1.distanceTo(p2);
geometry::Point2D sum = p1 + p2;
```

#### Vector2D
```cpp
namespace geometry {
    class Vector2D {
    public:
        double x, y;
        
        // Constructors
        Vector2D() = default;
        Vector2D(double x, double y);
        Vector2D(const Point2D& from, const Point2D& to);
        
        // Operators
        Vector2D operator+(const Vector2D& other) const;
        Vector2D operator-(const Vector2D& other) const;
        Vector2D operator*(double scalar) const;
        Vector2D operator/(double scalar) const;
        
        // Methods
        double magnitude() const;
        Vector2D normalized() const;
        double dotProduct(const Vector2D& other) const;
        double crossProduct(const Vector2D& other) const;
    };
}
```

**Usage Example**:
```cpp
geometry::Vector2D v1(3.0, 4.0);
geometry::Vector2D v2(1.0, 2.0);
double dot = v1.dotProduct(v2);
geometry::Vector2D normalized = v1.normalized();
```

### Segment Classes

#### Segment (Base Class)
```cpp
namespace contour {
    enum class SegmentType {
        Line,
        Arc
    };
    
    class Segment {
    public:
        virtual ~Segment() = default;
        
        // Pure virtual methods
        virtual geometry::Point2D getStartPoint() const = 0;
        virtual geometry::Point2D getEndPoint() const = 0;
        virtual SegmentType getType() const = 0;
        virtual std::unique_ptr<Segment> clone() const = 0;
        virtual double getLength() const = 0;
        virtual bool isEqual(const Segment& other, double epsilon) const = 0;
        virtual geometry::Vector2D getDirection() const = 0;
        virtual geometry::Point2D getPointAt(double t) const = 0;
        
        // Common methods
        bool isConnectedTo(const Segment& other, double epsilon = geometry::EPSILON) const;
    };
}
```

#### LineSegment
```cpp
namespace contour {
    class LineSegment : public Segment {
    public:
        LineSegment(const geometry::Point2D& start, const geometry::Point2D& end);
        
        // Implemented virtual methods
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

**Usage Example**:
```cpp
auto line = std::make_unique<LineSegment>(
    geometry::Point2D(0.0, 0.0),
    geometry::Point2D(3.0, 4.0)
);
double length = line->getLength();
geometry::Point2D midPoint = line->getPointAt(0.5);
```

#### ArcSegment
```cpp
namespace contour {
    class ArcSegment : public Segment {
    public:
        ArcSegment(const geometry::Point2D& center, double radius,
                   double startAngle, double endAngle, bool clockwise);
        
        // Implemented virtual methods
        geometry::Point2D getStartPoint() const override;
        geometry::Point2D getEndPoint() const override;
        SegmentType getType() const override;
        std::unique_ptr<Segment> clone() const override;
        double getLength() const override;
        bool isEqual(const Segment& other, double epsilon) const override;
        geometry::Vector2D getDirection() const override;
        geometry::Point2D getPointAt(double t) const override;
        
        // Arc-specific methods
        geometry::Point2D getCenter() const;
        double getRadius() const;
        double getStartAngle() const;
        double getEndAngle() const;
        bool isClockwise() const;
        double getAngleSpan() const;
    };
}
```

**Usage Example**:
```cpp
auto arc = std::make_unique<ArcSegment>(
    geometry::Point2D(0.0, 0.0),  // center
    5.0,                           // radius
    0.0,                           // start angle
    M_PI,                         // end angle
    false                         // counter-clockwise
);
double length = arc->getLength();
geometry::Point2D point = arc->getPointAt(0.5);
```

### Contour Class

#### Contour
```cpp
namespace contour {
    class Contour {
    public:
        // Type aliases
        using SegmentContainer = std::vector<std::unique_ptr<Segment>>;
        using iterator = SegmentContainer::iterator;
        using const_iterator = SegmentContainer::const_iterator;
        using size_type = SegmentContainer::size_type;
        
        // Constructors
        Contour() = default;
        explicit Contour(SegmentContainer segments);
        Contour(const Contour& other);
        Contour(Contour&&) noexcept = default;
        
        // Assignment
        Contour& operator=(const Contour& other);
        Contour& operator=(Contour&&) noexcept = default;
        
        // Segment management
        void addSegment(std::unique_ptr<Segment> segment);
        void insertSegment(size_type index, std::unique_ptr<Segment> segment);
        void removeSegment(size_type index);
        void clear();
        
        // Access methods
        size_type size() const;
        bool empty() const;
        Segment& operator[](size_type index);
        const Segment& operator[](size_type index) const;
        Segment& at(size_type index);
        const Segment& at(size_type index) const;
        
        // Iteration
        iterator begin();
        const_iterator begin() const;
        const_iterator cbegin() const;
        iterator end();
        const_iterator end() const;
        const_iterator cend() const;
        
        // Validation
        bool isValid(double epsilon = geometry::EPSILON) const;
        bool isClosed(double epsilon = geometry::EPSILON) const;
        
        // Utility methods
        double getTotalLength() const;
        std::vector<geometry::Point2D> getVertices() const;
        bool isEqual(const Contour& other, double epsilon = geometry::EPSILON) const;
    };
}
```

**Usage Example**:
```cpp
Contour contour;
contour.addSegment(std::make_unique<LineSegment>(
    geometry::Point2D(0.0, 0.0),
    geometry::Point2D(1.0, 0.0)
));
contour.addSegment(std::make_unique<LineSegment>(
    geometry::Point2D(1.0, 0.0),
    geometry::Point2D(1.0, 1.0)
));

bool isValid = contour.isValid();
double totalLength = contour.getTotalLength();

// Iterate over segments
for (const auto& segment : contour) {
    std::cout << "Segment length: " << segment.getLength() << std::endl;
}
```

### Factory Functions

#### Segment Creation
```cpp
namespace contour {
    // Line segment factory
    std::unique_ptr<Segment> createLineSegment(
        const geometry::Point2D& start,
        const geometry::Point2D& end
    );
    
    // Arc segment factory
    std::unique_ptr<Segment> createArcSegment(
        const geometry::Point2D& center,
        double radius,
        double startAngle,
        double endAngle,
        bool clockwise = false
    );
}
```

**Usage Example**:
```cpp
auto line = createLineSegment(
    geometry::Point2D(0.0, 0.0),
    geometry::Point2D(3.0, 4.0)
);

auto arc = createArcSegment(
    geometry::Point2D(0.0, 0.0),
    5.0,
    0.0,
    M_PI,
    false
);
```

### ContourBuilder

#### ContourBuilder
```cpp
namespace contour {
    class ContourBuilder {
    public:
        ContourBuilder& addLine(const geometry::Point2D& start, const geometry::Point2D& end);
        ContourBuilder& addArc(const geometry::Point2D& center, double radius,
                              double startAngle, double endAngle, bool clockwise = false);
        
        Contour build() &&;
        const Contour& get() const;
    };
}
```

**Usage Example**:
```cpp
auto contour = std::move(ContourBuilder()
    .addLine(geometry::Point2D(0.0, 0.0), geometry::Point2D(5.0, 0.0))
    .addArc(geometry::Point2D(5.0, 2.5), 2.5, -M_PI/2, M_PI/2, false)
    .addLine(geometry::Point2D(5.0, 5.0), geometry::Point2D(0.0, 5.0))
    .addLine(geometry::Point2D(0.0, 5.0), geometry::Point2D(0.0, 0.0)))
    .build();
```

### Utilities Namespace

#### Polyline Creation
```cpp
namespace contour::utilities {
    struct PolylineOptions {
        bool closePath = false;
        bool validateInput = true;
        bool removeCollinear = false;
        double collinearThreshold = 1e-6;
    };
    
    // Basic polyline creation
    Contour createPolylineContour(std::initializer_list<geometry::Point2D> points);
    Contour createPolylineContour(const std::vector<geometry::Point2D>& points);
    Contour createPolylineContour(std::span<const geometry::Point2D> points);
    Contour createPolylineContour(const geometry::Point2D* points, size_t count);
    
    // Advanced polyline creation with options
    Contour createPolylineContour(const std::vector<geometry::Point2D>& points,
                                 const PolylineOptions& options);
    
    // Utility functions
    std::vector<geometry::Point2D> extractPoints(const Contour& contour);
    bool arePointsCollinear(const geometry::Point2D& p1, const geometry::Point2D& p2,
                           const geometry::Point2D& p3, double threshold);
    bool validatePointSequence(const std::vector<geometry::Point2D>& points, bool allowDuplicates);
    std::vector<geometry::Point2D> removeConsecutiveDuplicates(
        const std::vector<geometry::Point2D>& points, double epsilon);
    std::vector<geometry::Point2D> removeCollinearPoints(
        const std::vector<geometry::Point2D>& points, double threshold);
}
```

**Usage Example**:
```cpp
std::vector<geometry::Point2D> points = {
    geometry::Point2D(0.0, 0.0),
    geometry::Point2D(3.0, 0.0),
    geometry::Point2D(3.0, 3.0),
    geometry::Point2D(0.0, 3.0)
};

// Basic polyline
auto polyline = utilities::createPolylineContour(points);

// Advanced polyline with options
utilities::PolylineOptions options;
options.closePath = true;
options.removeCollinear = true;
auto closedPolyline = utilities::createPolylineContour(points, options);
```

### Visualization Namespace

#### ContourVisualizer
```cpp
namespace contour::visualization {
    struct VisualizationOptions {
        int width = 60;
        int height = 20;
        char lineChar = '*';
        char arcChar = 'o';
        char vertexChar = '#';
        bool showCoordinates = false;
        double scale = 1.0;
    };
    
    class ContourVisualizer {
    public:
        explicit ContourVisualizer(const VisualizationOptions& options = {});
        
        // Visualization methods
        std::string visualizeContour(const Contour& contour);
        std::string visualizeContours(const std::vector<const Contour*>& contours);
        std::string generateContourTable(const Contour& contour);
        std::string generateCoordinateInfo(const Contour& contour);
    };
}
```

**Usage Example**:
```cpp
visualization::VisualizationOptions options;
options.width = 60;
options.height = 20;
options.showCoordinates = true;

ContourVisualizer visualizer(options);
std::string visualization = visualizer.visualizeContour(contour);
std::cout << visualization << std::endl;
```

## GUI Classes

### MainWindow
```cpp
class MainWindow : public QMainWindow {
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
private slots:
    // File operations
    void on_actionNew_triggered();
    void on_actionOpen_triggered();
    void on_actionSave_triggered();
    void on_actionExit_triggered();
    
    // Edit operations
    void on_actionClear_triggered();
    void on_actionUndo_triggered();
    void on_actionRedo_triggered();
    
    // Tools
    void on_actionAddLine_triggered();
    void on_actionAddArc_triggered();
    void on_actionValidate_triggered();
    
    // View operations
    void on_actionShowGrid_triggered();
    void on_actionSnapToGrid_triggered();
    void on_actionZoomIn_triggered();
    void on_actionZoomOut_triggered();
    void on_actionZoomFit_triggered();
    
    // Help
    void on_actionAbout_triggered();
    
    // Contour updates
    void updateContourInfo();
    
private:
    void setupUI();
    void setupConnections();
    void updateStatusBar();
    
    // UI Components
    QGraphicsView *m_graphicsView;
    ContourScene *m_scene;
    ContourView *m_view;
    
    // Actions
    QAction *m_actionNew;
    QAction *m_actionOpen;
    QAction *m_actionSave;
    QAction *m_actionExit;
    QAction *m_actionClear;
    QAction *m_actionUndo;
    QAction *m_actionRedo;
    QAction *m_actionAddLine;
    QAction *m_actionAddArc;
    QAction *m_actionValidate;
    QAction *m_actionShowGrid;
    QAction *m_actionSnapToGrid;
    QAction *m_actionZoomIn;
    QAction *m_actionZoomOut;
    QAction *m_actionZoomFit;
    QAction *m_actionAbout;
};
```

### ContourScene
```cpp
class ContourScene : public QGraphicsScene {
    Q_OBJECT
    
public:
    ContourScene(QObject *parent = nullptr);
    
    // Contour management
    void setContour(const contour::Contour& contour);
    const contour::Contour& getContour() const;
    void addLineSegment(const geometry::Point2D& start, const geometry::Point2D& end);
    void addArcSegment(const geometry::Point2D& center, double radius,
                       double startAngle, double endAngle, bool clockwise);
    void clear();
    
    // Validation
    bool isValid() const;
    bool isClosed() const;
    
    // Grid system
    void setShowGrid(bool show);
    void setSnapToGrid(bool snap);
    void setGridSize(double size);
    
    // Coordinate conversion
    geometry::Point2D sceneToWorld(const QPointF& scenePos) const;
    QPointF worldToScene(const geometry::Point2D& worldPos) const;
    
signals:
    void contourModified();
    
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void drawBackground(QPainter *painter, const QRectF &rect) override;
    
private:
    void updateScene();
    void drawGrid(QPainter *painter, const QRectF &rect);
    void drawContour(QPainter *painter);
    
    contour::Contour m_contour;
    bool m_showGrid;
    bool m_snapToGrid;
    double m_gridSize;
    double m_scale;
    bool m_isDrawing;
    geometry::Point2D m_startPoint;
    geometry::Point2D m_currentPoint;
    QGraphicsLineItem *m_previewItem;
};
```

### ContourView
```cpp
class ContourView : public QGraphicsView {
    Q_OBJECT
    
public:
    explicit ContourView(ContourScene *scene, QWidget *parent = nullptr);
    ~ContourView() = default;
    
protected:
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    
private:
    ContourScene *m_scene;
    bool m_isPanning;
    QPoint m_lastPanPoint;
};
```

## Constants and Configuration

### Geometry Constants
```cpp
namespace geometry {
    constexpr double EPSILON = 1e-6;
    constexpr double PI = 3.14159265358979323846;
    constexpr double TWO_PI = 2.0 * PI;
    constexpr double HALF_PI = PI / 2.0;
}
```

### Error Handling

#### Exceptions
```cpp
// Common exceptions thrown by the API
std::invalid_argument  // Invalid parameters
std::out_of_range     // Index out of bounds
std::runtime_error    // Runtime errors
```

#### Error Messages
```cpp
// Common error message patterns
"LineSegment: start and end points cannot be the same"
"ArcSegment: radius must be positive"
"ArcSegment: start and end angles cannot be the same"
"Contour::addSegment: segment cannot be null"
"Contour::at: index out of range"
"createPolylineContour: need at least 2 points"
```

## Best Practices

### Memory Management
```cpp
// Use smart pointers for ownership
std::unique_ptr<Segment> segment = createLineSegment(start, end);
contour.addSegment(std::move(segment));

// Use const references for read-only access
const auto& segment = contour[0];
geometry::Point2D start = segment.getStartPoint();
```

### Error Handling
```cpp
try {
    auto segment = createLineSegment(start, end);
    contour.addSegment(std::move(segment));
} catch (const std::invalid_argument& e) {
    std::cerr << "Invalid segment: " << e.what() << std::endl;
} catch (const std::exception& e) {
    std::cerr << "Unexpected error: " << e.what() << std::endl;
}
```

### Performance
```cpp
// Cache validation results
bool isValid = contour.isValid();  // Calculated once
bool isStillValid = contour.isValid();  // Uses cache

// Use move semantics
Contour contour1 = createContour();
Contour contour2 = std::move(contour1);  // Efficient transfer
```

### Thread Safety
```cpp
// The API is not thread-safe by default
// For multi-threaded access, use external synchronization
std::mutex contourMutex;
{
    std::lock_guard<std::mutex> lock(contourMutex);
    contour.addSegment(std::move(segment));
}
```

## Examples

### Complete Example
```cpp
#include "include/Contour.h"
#include "include/ContourUtilities.h"
#include <iostream>

int main() {
    try {
        // Create a simple square contour
        contour::Contour square;
        
        // Add line segments
        square.addSegment(createLineSegment(
            geometry::Point2D(0.0, 0.0),
            geometry::Point2D(2.0, 0.0)
        ));
        square.addSegment(createLineSegment(
            geometry::Point2D(2.0, 0.0),
            geometry::Point2D(2.0, 2.0)
        ));
        square.addSegment(createLineSegment(
            geometry::Point2D(2.0, 2.0),
            geometry::Point2D(0.0, 2.0)
        ));
        square.addSegment(createLineSegment(
            geometry::Point2D(0.0, 2.0),
            geometry::Point2D(0.0, 0.0)
        ));
        
        // Validate the contour
        if (square.isValid()) {
            std::cout << "Square contour is valid!" << std::endl;
            std::cout << "Total length: " << square.getTotalLength() << std::endl;
            std::cout << "Is closed: " << (square.isClosed() ? "Yes" : "No") << std::endl;
        } else {
            std::cout << "Square contour is invalid!" << std::endl;
        }
        
        // Iterate over segments
        for (const auto& segment : square) {
            std::cout << "Segment type: " << static_cast<int>(segment.getType()) << std::endl;
            std::cout << "Segment length: " << segment.getLength() << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
```

This API reference provides comprehensive documentation for all public interfaces in the ContourSegment library. For more detailed examples and usage patterns, refer to the unit tests and user guides. 