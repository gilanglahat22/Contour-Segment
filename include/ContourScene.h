#pragma once

#include "Contour.h"
#include "Segment.h"
#include "Geometry.h"
#include <QGraphicsScene>
#include <QWidget>
#include <QColor>
#include <memory>
#include <vector>
#include <map>

// Forward declarations
class QGraphicsEllipseItem;
class QGraphicsLineItem;
class QGraphicsPathItem;

enum class EditMode {
    Draw,
    Edit
};

enum class DrawingMode {
    Line,
    Arc
};

struct VertexInfo {
    size_t segmentIndex;
    bool isStartPoint;
    QGraphicsEllipseItem* marker;
    geometry::Point2D originalPoint;
};

struct SegmentVisualInfo {
    QGraphicsItem* visualItem;
    QColor color;
    double thickness;
};

class ContourScene : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit ContourScene(QWidget* parent = nullptr);
    
    void setContour(const contour::Contour& contour);
    void addLineSegment(const geometry::Point2D& start, const geometry::Point2D& end);
    void addArcSegment(const geometry::Point2D& center, double radius, 
                      double startAngle, double endAngle, bool clockwise = false);
    void clear();
    
    bool isValid() const;
    bool isClosed() const;
    
    void setShowGrid(bool show);
    void setSnapToGrid(bool snap);
    
    // Edit mode methods
    void setEditMode(EditMode mode);
    EditMode getEditMode() const { return m_editMode; }
    
    // Drawing mode methods
    void setDrawingMode(DrawingMode mode);
    DrawingMode getDrawingMode() const { return m_drawingMode; }
    
    // Color management
    void setSegmentColor(size_t segmentIndex, const QColor& color);
    void setSegmentThickness(size_t segmentIndex, double thickness);
    QColor getSegmentColor(size_t segmentIndex) const;
    double getSegmentThickness(size_t segmentIndex) const;
    
    // Vertex editing
    void startVertexEdit(size_t segmentIndex, bool isStartPoint);
    void finishVertexEdit();
    bool isVertexEditing() const { return m_editingVertex != nullptr; }
    
    // Getter methods
    const contour::Contour& getContour() const { return m_contour; }
    size_t getSegmentCount() const { return m_contour.size(); }
    
    // Signal emission control
    void setSignalEmissionEnabled(bool enabled) { m_isEmittingSignals = enabled; }
    bool isSignalEmissionEnabled() const { return m_isEmittingSignals; }

signals:
    void contourModified();
    void vertexEditStarted(size_t segmentIndex, bool isStartPoint);
    void vertexEditFinished();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void drawBackground(QPainter* painter, const QRectF& rect) override;

private:
    void updateScene();
    void createSegmentItem(const contour::Segment& segment, size_t segmentIndex);
    QPointF snapToGrid(const QPointF& point) const;
    void cleanupPreviewItem();
    void cleanupArcDrawing();
    
    // Vertex editing helpers
    void createVertexMarkers();
    void updateVertexMarker(size_t segmentIndex, bool isStartPoint, const QPointF& pos);
    void updateVertexMarkersForSegment(size_t segmentIndex);
    void updateVertexMarkersForSegmentOnly(size_t segmentIndex);
    void removeVertexMarkers();
    VertexInfo* findVertexAt(const QPointF& pos);
    void updateSegmentAfterVertexEdit(size_t segmentIndex, bool isStartPoint, const geometry::Point2D& newPoint);
    void updateSegmentVisual(size_t segmentIndex);
    void updateSegmentVisualOnly(size_t segmentIndex);
    void createSegmentItemOnly(const contour::Segment& segment, size_t segmentIndex);
    
    // Color helpers
    QColor getDefaultSegmentColor(size_t segmentIndex) const;
    double getDefaultSegmentThickness(size_t segmentIndex) const;

    contour::Contour m_contour;
    bool m_showGrid;
    bool m_snapToGrid;
    double m_gridSize;
    double m_scale;
    bool m_isDrawing;
    QGraphicsItem* m_previewItem;
    QPointF m_lastPoint;
    
    // Edit mode
    EditMode m_editMode;
    
    // Drawing mode
    DrawingMode m_drawingMode;
    
    // Arc drawing state
    bool m_isDrawingArc;
    QPointF m_arcCenter;
    QPointF m_arcStartPoint;
    QGraphicsEllipseItem* m_arcCenterMarker;
    QGraphicsEllipseItem* m_arcRadiusMarker;
    int m_arcStep; // Tracking urutan klik pada mode Draw Arc
    
    // Vertex editing
    std::vector<VertexInfo> m_vertices;
    VertexInfo* m_editingVertex;
    QPointF m_editStartPos;
    
    // Visual properties
    std::map<size_t, SegmentVisualInfo> m_segmentVisuals;
    std::vector<QColor> m_defaultColors;
    
    // Prevent infinite recursion
    bool m_isUpdatingScene;
    
    // Mouse event throttling
    QPointF m_lastMousePos;
    bool m_isMouseDragging;
    int m_mouseUpdateCounter;
    
    // Signal emission control
    bool m_isEmittingSignals;
    
    // Recursion prevention
    bool m_isInVertexUpdate;
    int m_recursionDepth;
    
    // Additional recursion protection
    bool m_updateInProgress;
    bool m_vertexUpdateInProgress;
}; 