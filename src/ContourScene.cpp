#include "../include/ContourScene.h"
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsLineItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsPathItem>
#include <QPainterPath>
#include <QDebug>
#include <cmath>
#include <limits>
#include <iostream>
#include <cstdlib>
#include <utility>
#define _USE_MATH_DEFINES
#include <math.h>
#include <QSignalBlocker>

ContourScene::ContourScene(QWidget* parent)
    : QGraphicsScene(parent)
    , m_showGrid(true)
    , m_snapToGrid(true)
    , m_gridSize(20.0)
    , m_scale(1.0)
    , m_isDrawing(false)
    , m_previewItem(nullptr)
    , m_editMode(EditMode::Draw)
    , m_drawingMode(DrawingMode::Line)
    , m_editingVertex(nullptr)
    , m_isDrawingArc(false)
    , m_arcCenterMarker(nullptr)
    , m_arcRadiusMarker(nullptr)
    , m_arcStep(0)
    , m_isUpdatingScene(false)
    , m_isMouseDragging(false)
    , m_mouseUpdateCounter(0)
    , m_isEmittingSignals(true)
    , m_isInVertexUpdate(false)
    , m_recursionDepth(0)
    , m_updateInProgress(false)
    , m_vertexUpdateInProgress(false)
{
    setSceneRect(-500, -500, 1000, 1000);
    
    // Initialize default colors
    m_defaultColors = {
        QColor(255, 0, 0),    // Red
        QColor(0, 255, 0),    // Green
        QColor(0, 0, 255),    // Blue
        QColor(255, 255, 0),  // Yellow
        QColor(255, 0, 255),  // Magenta
        QColor(0, 255, 255),  // Cyan
        QColor(255, 165, 0),  // Orange
        QColor(128, 0, 128),  // Purple
        QColor(0, 128, 0),    // Dark Green
        QColor(128, 128, 0)   // Olive
    };
    
    std::cout << "ContourScene initialized with enhanced recursion protection" << std::endl;
}

void ContourScene::setContour(const contour::Contour& contour)
{
    try {
        if (m_updateInProgress) {
            std::cout << "Preventing recursive setContour call" << std::endl;
            return;
        }
        
        m_updateInProgress = true;
        m_contour = contour;
        updateScene();
        
        if (m_isEmittingSignals && !m_isUpdatingScene) {
            std::cout << "[TRACE] emit contourModified" << std::endl;
        emit contourModified();
        }
        
        m_updateInProgress = false;
    } catch (const std::exception& e) {
        std::cerr << "Error setting contour: " << e.what() << std::endl;
        m_updateInProgress = false;
    }
}

void ContourScene::addLineSegment(const geometry::Point2D& start, const geometry::Point2D& end)
{
    try {
        // Basic validation
        if (std::isnan(start.x) || std::isnan(start.y) || 
            std::isnan(end.x) || std::isnan(end.y)) {
            std::cerr << "Invalid point coordinates" << std::endl;
            return;
        }

        // Check if points are too close
        if (start.isEqual(end, 1e-6)) {
            std::cerr << "Start and end points are too close" << std::endl;
            return;
        }

        auto segment = contour::createLineSegment(start, end);
        if (!segment) {
            std::cerr << "Failed to create line segment" << std::endl;
            return;
        }
        
        m_contour.addSegment(std::move(segment));
        
        // Use minimal update to prevent recursion
        if (!m_isUpdatingScene && !m_updateInProgress) {
        updateScene();
            if (m_isEmittingSignals) {
                std::cout << "[TRACE] emit contourModified" << std::endl;
        emit contourModified();
            }
        }
        
        std::cout << "Added line segment: (" << start.x << "," << start.y << ") to (" 
                  << end.x << "," << end.y << ")" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error adding line segment: " << e.what() << std::endl;
    }
}

void ContourScene::addArcSegment(const geometry::Point2D& center, double radius, 
                                double startAngle, double endAngle, bool clockwise)
{
    try {
        // Basic validation
        if (std::isnan(center.x) || std::isnan(center.y)) {
            std::cerr << "Invalid center point coordinates" << std::endl;
            return;
        }
        
        if (std::isnan(radius) || radius <= 0) {
            std::cerr << "Invalid radius value" << std::endl;
            return;
        }
        
        if (std::isnan(startAngle) || std::isnan(endAngle)) {
            std::cerr << "Invalid angle values" << std::endl;
            return;
        }

        auto segment = contour::createArcSegment(center, radius, startAngle, endAngle, clockwise);
        if (!segment) {
            std::cerr << "Failed to create arc segment" << std::endl;
            return;
        }
        
        m_contour.addSegment(std::move(segment));
        
        // Use minimal update to prevent recursion
        if (!m_isUpdatingScene && !m_updateInProgress) {
        updateScene();
            if (m_isEmittingSignals) {
                std::cout << "[TRACE] emit contourModified" << std::endl;
        emit contourModified();
            }
        }
        
        std::cout << "Added arc segment: center(" << center.x << "," << center.y 
                  << "), radius=" << radius << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error adding arc segment: " << e.what() << std::endl;
    }
}

void ContourScene::clear()
{
    try {
        if (m_updateInProgress) {
            std::cout << "Preventing recursive clear call" << std::endl;
            return;
        }
        
        m_updateInProgress = true;
        QGraphicsScene::clear();
        m_contour = contour::Contour();
        cleanupPreviewItem();
        removeVertexMarkers();
        m_segmentVisuals.clear();
        m_vertices.clear();
        m_editingVertex = nullptr;
        
        if (m_isEmittingSignals && !m_isUpdatingScene) {
            std::cout << "[TRACE] emit contourModified" << std::endl;
        emit contourModified();
        }
        
        m_updateInProgress = false;
    } catch (const std::exception& e) {
        std::cerr << "Error clearing scene: " << e.what() << std::endl;
        m_updateInProgress = false;
    }
}

// Edit mode methods
void ContourScene::setEditMode(EditMode mode)
{
    std::cout << "[TRACE] setEditMode called. Flags: isUpdatingScene=" << m_isUpdatingScene
              << ", updateInProgress=" << m_updateInProgress
              << ", isInVertexUpdate=" << m_isInVertexUpdate
              << ", vertexUpdateInProgress=" << m_vertexUpdateInProgress << std::endl;
    // Prevent recursion by checking if we're already updating
    if (m_isUpdatingScene || m_updateInProgress || m_isInVertexUpdate || m_vertexUpdateInProgress) {
        std::cout << "Preventing recursive setEditMode call" << std::endl;
        return;
    }

    if (m_editMode != mode) {
        m_editMode = mode;
        
        if (mode == EditMode::Edit) {
            createVertexMarkers();
        } else {
            removeVertexMarkers();
            if (m_editingVertex) {
                finishVertexEdit();
            }
        }
        
        std::cout << "Edit mode changed to: " << (mode == EditMode::Draw ? "Draw" : "Edit") << std::endl;
    }
}

void ContourScene::setDrawingMode(DrawingMode mode)
{
    std::cout << "[TRACE] setDrawingMode called. Flags: isUpdatingScene=" << m_isUpdatingScene
              << ", updateInProgress=" << m_updateInProgress
              << ", isInVertexUpdate=" << m_isInVertexUpdate
              << ", vertexUpdateInProgress=" << m_vertexUpdateInProgress << std::endl;
    // Prevent recursion by checking if we're already updating
    if (m_isUpdatingScene || m_updateInProgress || m_isInVertexUpdate || m_vertexUpdateInProgress) {
        std::cout << "Preventing recursive setDrawingMode call" << std::endl;
        return;
    }

    if (m_drawingMode != mode) {
        m_drawingMode = mode;
        
        // Clean up any ongoing drawing
        if (m_isDrawing) {
            cleanupPreviewItem();
            m_isDrawing = false;
        }
        
        if (m_isDrawingArc) {
            cleanupArcDrawing();
        }
        
        std::cout << "Drawing mode changed to: " << (mode == DrawingMode::Line ? "Line" : "Arc") << std::endl;
    }
}

// Color management methods
void ContourScene::setSegmentColor(size_t segmentIndex, const QColor& color)
{
    std::cout << "[TRACE] setSegmentColor called. Flags: isUpdatingScene=" << m_isUpdatingScene
              << ", updateInProgress=" << m_updateInProgress
              << ", isInVertexUpdate=" << m_isInVertexUpdate
              << ", vertexUpdateInProgress=" << m_vertexUpdateInProgress << std::endl;
    // Prevent recursion by checking if we're already updating
    if (m_isUpdatingScene || m_updateInProgress || m_isInVertexUpdate || m_vertexUpdateInProgress) {
        std::cout << "Preventing recursive setSegmentColor call" << std::endl;
        return;
    }

    if (segmentIndex >= m_contour.size()) {
        std::cerr << "Invalid segment index: " << segmentIndex << std::endl;
        return;
    }
    
    auto it = m_segmentVisuals.find(segmentIndex);
    if (it != m_segmentVisuals.end()) {
        it->second.color = color;
        
        // Update the visual item
        if (it->second.visualItem) {
            if (QGraphicsLineItem* lineItem = qgraphicsitem_cast<QGraphicsLineItem*>(it->second.visualItem)) {
                lineItem->setPen(QPen(color, it->second.thickness));
            } else if (QGraphicsPathItem* pathItem = qgraphicsitem_cast<QGraphicsPathItem*>(it->second.visualItem)) {
                pathItem->setPen(QPen(color, it->second.thickness));
            }
        }
        
        std::cout << "Set segment " << segmentIndex << " color to " << color.name().toStdString() << std::endl;
    }
}

void ContourScene::setSegmentThickness(size_t segmentIndex, double thickness)
{
    std::cout << "[TRACE] setSegmentThickness called. Flags: isUpdatingScene=" << m_isUpdatingScene
              << ", updateInProgress=" << m_updateInProgress
              << ", isInVertexUpdate=" << m_isInVertexUpdate
              << ", vertexUpdateInProgress=" << m_vertexUpdateInProgress << std::endl;
    // Prevent recursion by checking if we're already updating
    if (m_isUpdatingScene || m_updateInProgress || m_isInVertexUpdate || m_vertexUpdateInProgress) {
        std::cout << "Preventing recursive setSegmentThickness call" << std::endl;
        return;
    }

    if (segmentIndex >= m_contour.size()) {
        std::cerr << "Invalid segment index: " << segmentIndex << std::endl;
        return;
    }
    
    if (thickness <= 0.0) {
        std::cerr << "Invalid thickness value: " << thickness << std::endl;
        return;
    }
    
    auto it = m_segmentVisuals.find(segmentIndex);
    if (it != m_segmentVisuals.end()) {
        it->second.thickness = thickness;
        
        // Update the visual item
        if (it->second.visualItem) {
            if (QGraphicsLineItem* lineItem = qgraphicsitem_cast<QGraphicsLineItem*>(it->second.visualItem)) {
                lineItem->setPen(QPen(it->second.color, thickness));
            } else if (QGraphicsPathItem* pathItem = qgraphicsitem_cast<QGraphicsPathItem*>(it->second.visualItem)) {
                pathItem->setPen(QPen(it->second.color, thickness));
            }
        }
        
        std::cout << "Set segment " << segmentIndex << " thickness to " << thickness << std::endl;
    }
}

QColor ContourScene::getSegmentColor(size_t segmentIndex) const
{
    if (segmentIndex >= m_contour.size()) {
        return QColor();
    }
    
    auto it = m_segmentVisuals.find(segmentIndex);
    if (it != m_segmentVisuals.end()) {
        return it->second.color;
    }
    
    return getDefaultSegmentColor(segmentIndex);
}

double ContourScene::getSegmentThickness(size_t segmentIndex) const
{
    if (segmentIndex >= m_contour.size()) {
        return 2.0;
    }
    
    auto it = m_segmentVisuals.find(segmentIndex);
    if (it != m_segmentVisuals.end()) {
        return it->second.thickness;
    }
    
    return getDefaultSegmentThickness(segmentIndex);
}

// Vertex editing methods
void ContourScene::startVertexEdit(size_t segmentIndex, bool isStartPoint)
{
    if (segmentIndex >= m_contour.size()) {
        std::cerr << "Invalid segment index for vertex edit: " << segmentIndex << std::endl;
        return;
    }
    
    try {
        const auto& segment = m_contour.at(segmentIndex);
        geometry::Point2D point = isStartPoint ? segment.getStartPoint() : segment.getEndPoint();
        
        m_editingVertex = new VertexInfo{segmentIndex, isStartPoint, nullptr, point};
        m_editStartPos = QPointF(point.x, point.y);
        
        std::cout << "[TRACE] emit vertexEditStarted" << std::endl;
        emit vertexEditStarted(segmentIndex, isStartPoint);
        std::cout << "Started editing vertex " << (isStartPoint ? "start" : "end") 
                  << " of segment " << segmentIndex << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error starting vertex edit: " << e.what() << std::endl;
    }
}

void ContourScene::finishVertexEdit()
{
    if (m_editingVertex) {
        delete m_editingVertex;
        m_editingVertex = nullptr;
        std::cout << "[TRACE] emit vertexEditFinished" << std::endl;
        emit vertexEditFinished();
        std::cout << "Finished vertex editing" << std::endl;
    }
}

// Vertex editing helper methods
void ContourScene::createVertexMarkers()
{
    std::cout << "[TRACE] createVertexMarkers called. Flags: isUpdatingScene=" << m_isUpdatingScene
              << ", updateInProgress=" << m_updateInProgress
              << ", isInVertexUpdate=" << m_isInVertexUpdate
              << ", vertexUpdateInProgress=" << m_vertexUpdateInProgress << std::endl;
    // Prevent recursion by checking if we're already updating
    if (m_isUpdatingScene || m_updateInProgress || m_isInVertexUpdate || m_vertexUpdateInProgress) {
        std::cout << "Preventing recursive createVertexMarkers call" << std::endl;
        return;
    }

    QSignalBlocker blocker(this); // Blokir signal Qt internal selama update
    removeVertexMarkers();
    m_vertices.clear();
    
    for (size_t i = 0; i < m_contour.size(); ++i) {
        try {
            const auto& segment = m_contour.at(i);
            
            // Create start point marker
            auto startPoint = segment.getStartPoint();
            QGraphicsEllipseItem* startMarker = new QGraphicsEllipseItem(startPoint.x - 4, startPoint.y - 4, 8, 8);
            startMarker->setBrush(Qt::red);
            startMarker->setPen(QPen(Qt::black, 1));
            startMarker->setFlag(QGraphicsItem::ItemIsSelectable, true);
            startMarker->setFlag(QGraphicsItem::ItemIsMovable, true); // Aktifkan drag
            addItem(startMarker);
            
            VertexInfo startVertex{i, true, startMarker, startPoint};
            m_vertices.push_back(startVertex);
            
            // Create end point marker
            auto endPoint = segment.getEndPoint();
            QGraphicsEllipseItem* endMarker = new QGraphicsEllipseItem(endPoint.x - 4, endPoint.y - 4, 8, 8);
            endMarker->setBrush(Qt::blue);
            endMarker->setPen(QPen(Qt::black, 1));
            endMarker->setFlag(QGraphicsItem::ItemIsSelectable, true);
            endMarker->setFlag(QGraphicsItem::ItemIsMovable, true); // Aktifkan drag
            addItem(endMarker);
            
            VertexInfo endVertex{i, false, endMarker, endPoint};
            m_vertices.push_back(endVertex);
            
        } catch (const std::exception& e) {
            std::cerr << "Error creating vertex markers for segment " << i << ": " << e.what() << std::endl;
        }
    }
    
    std::cout << "Created " << m_vertices.size() << " vertex markers" << std::endl;
}

void ContourScene::updateVertexMarker(size_t segmentIndex, bool isStartPoint, const QPointF& pos)
{
    for (auto& vertex : m_vertices) {
        if (vertex.segmentIndex == segmentIndex && vertex.isStartPoint == isStartPoint) {
            if (vertex.marker) {
                vertex.marker->setPos(pos.x() - 4, pos.y() - 4);
            }
            break;
        }
    }
}

void ContourScene::removeVertexMarkers()
{
    std::cout << "[TRACE] removeVertexMarkers called. Flags: isUpdatingScene=" << m_isUpdatingScene
              << ", updateInProgress=" << m_updateInProgress
              << ", isInVertexUpdate=" << m_isInVertexUpdate
              << ", vertexUpdateInProgress=" << m_vertexUpdateInProgress << std::endl;
    // Prevent recursion by checking if we're already updating
    if (m_isUpdatingScene || m_updateInProgress || m_isInVertexUpdate || m_vertexUpdateInProgress) {
        std::cout << "Preventing recursive removeVertexMarkers call" << std::endl;
        return;
    }

    QSignalBlocker blocker(this); // Blokir signal Qt internal selama update
    for (auto& vertex : m_vertices) {
        if (vertex.marker) {
            try {
                if (vertex.marker->scene() == this) {
                    removeItem(vertex.marker);
                }
                delete vertex.marker;
            } catch (const std::exception& e) {
                std::cerr << "Error removing vertex marker: " << e.what() << std::endl;
            }
            vertex.marker = nullptr;
        }
    }
    m_vertices.clear();
}

VertexInfo* ContourScene::findVertexAt(const QPointF& pos)
{
    const double tolerance = 6.0; // 6 pixels tolerance
    
    for (auto& vertex : m_vertices) {
        if (vertex.marker) {
            QRectF bounds = vertex.marker->boundingRect();
            QPointF markerPos = vertex.marker->pos() + QPointF(4, 4); // Center of marker
            QPointF diff = pos - markerPos;
            
            if (std::abs(diff.x()) <= tolerance && std::abs(diff.y()) <= tolerance) {
                return &vertex;
            }
        }
    }
    return nullptr;
}

void ContourScene::updateSegmentAfterVertexEdit(size_t segmentIndex, bool isStartPoint, const geometry::Point2D& newPoint)
{
    std::cout << "[TRACE] updateSegmentAfterVertexEdit called. Flags: isUpdatingScene=" << m_isUpdatingScene
              << ", updateInProgress=" << m_updateInProgress
              << ", isInVertexUpdate=" << m_isInVertexUpdate
              << ", vertexUpdateInProgress=" << m_vertexUpdateInProgress << std::endl;
    try {
        // Prevent recursion by checking if we're already updating
        if (m_isUpdatingScene || m_isInVertexUpdate || m_vertexUpdateInProgress || m_updateInProgress) {
            std::cout << "Preventing recursive vertex update" << std::endl;
            return;
        }
        
        // Track recursion depth
        m_recursionDepth++;
        if (m_recursionDepth > 2) { // Reduced from 3 to 2
            std::cout << "Recursion depth limit reached: " << m_recursionDepth << std::endl;
            m_recursionDepth--;
            return;
        }
        
        m_isInVertexUpdate = true;
        m_vertexUpdateInProgress = true;
        
        // Create a new segment with the updated point
        const auto& oldSegment = m_contour.at(segmentIndex);
        
        std::unique_ptr<contour::Segment> newSegment;
        
        if (oldSegment.getType() == contour::SegmentType::Line) {
            const auto& lineSegment = dynamic_cast<const contour::LineSegment&>(oldSegment);
            geometry::Point2D start = isStartPoint ? newPoint : lineSegment.getStartPoint();
            geometry::Point2D end = isStartPoint ? lineSegment.getEndPoint() : newPoint;
            newSegment = contour::createLineSegment(start, end);
        } else if (oldSegment.getType() == contour::SegmentType::Arc) {
            // For arcs, we need to recalculate the arc parameters
            // This is a simplified implementation - in a real app you might want more sophisticated arc editing
            const auto& arcSegment = dynamic_cast<const contour::ArcSegment&>(oldSegment);
            geometry::Point2D center = arcSegment.getCenter();
            double radius = arcSegment.getRadius();
            
            // Calculate new angles based on the moved point
            double newAngle;
            if (isStartPoint) {
                newAngle = std::atan2(newPoint.y - center.y, newPoint.x - center.x);
            } else {
                newAngle = std::atan2(newPoint.y - center.y, newPoint.x - center.x);
            }
            
            double startAngle = isStartPoint ? newAngle : arcSegment.getStartAngle();
            double endAngle = isStartPoint ? arcSegment.getEndAngle() : newAngle;
            
            newSegment = contour::createArcSegment(center, radius, startAngle, endAngle, arcSegment.isClockwise());
        }
        
        if (newSegment) {
            // Replace the segment in the contour
            m_contour.replaceSegment(segmentIndex, std::move(newSegment));
            
            // Update only the specific visual item without triggering full scene update
            updateSegmentVisualOnly(segmentIndex);
            
            // Update vertex markers for this segment only
            if (m_editMode == EditMode::Edit) {
                updateVertexMarkersForSegmentOnly(segmentIndex);
            }
            
            // Emit signal only if not in recursive update
            if (m_isEmittingSignals && !m_isUpdatingScene) {
                std::cout << "[TRACE] emit contourModified" << std::endl;
                emit contourModified();
            }
            
            std::cout << "Updated segment " << segmentIndex << " " 
                      << (isStartPoint ? "start" : "end") << " point to (" 
                      << newPoint.x << "," << newPoint.y << ")" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error updating segment after vertex edit: " << e.what() << std::endl;
    }
    
    // Cleanup recursion tracking
    m_isInVertexUpdate = false;
    m_vertexUpdateInProgress = false;
    m_recursionDepth--;
}

void ContourScene::updateSegmentVisualOnly(size_t segmentIndex)
{
    std::cout << "[TRACE] updateSegmentVisualOnly called. Flags: isUpdatingScene=" << m_isUpdatingScene
              << ", updateInProgress=" << m_updateInProgress << std::endl;
    try {
        if (segmentIndex >= m_contour.size()) {
            return;
        }
        
        // Prevent recursion
        if (m_isUpdatingScene || m_updateInProgress) {
            return;
        }
        
        const auto& segment = m_contour.at(segmentIndex);
        auto it = m_segmentVisuals.find(segmentIndex);
        
        if (it != m_segmentVisuals.end() && it->second.visualItem) {
            // Remove old visual item
            removeItem(it->second.visualItem);
            delete it->second.visualItem;
            
            // Create new visual item without triggering scene update
            createSegmentItemOnly(segment, segmentIndex);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error updating segment visual: " << e.what() << std::endl;
    }
}

void ContourScene::updateVertexMarkersForSegmentOnly(size_t segmentIndex)
{
    std::cout << "[TRACE] updateVertexMarkersForSegmentOnly called. Flags: isUpdatingScene=" << m_isUpdatingScene
              << ", updateInProgress=" << m_updateInProgress
              << ", isInVertexUpdate=" << m_isInVertexUpdate
              << ", vertexUpdateInProgress=" << m_vertexUpdateInProgress << std::endl;
    try {
        if (segmentIndex >= m_contour.size()) {
            return;
        }
        
        // Prevent recursion during vertex editing
        if (m_isUpdatingScene || m_isMouseDragging || m_isInVertexUpdate || m_vertexUpdateInProgress || m_updateInProgress) {
            std::cout << "Preventing recursive vertex marker update" << std::endl;
            return;
        }
        
        // Track recursion depth
        m_recursionDepth++;
        if (m_recursionDepth > 1) { // Reduced from 2 to 1
            std::cout << "Vertex marker recursion depth limit reached: " << m_recursionDepth << std::endl;
            m_recursionDepth--;
            return;
        }
        
        const auto& segment = m_contour.at(segmentIndex);
        
        // Remove existing vertex markers for this segment only
        for (auto it = m_vertices.begin(); it != m_vertices.end();) {
            if (it->segmentIndex == segmentIndex) {
                if (it->marker) {
                    removeItem(it->marker);
                    delete it->marker;
                }
                it = m_vertices.erase(it);
            } else {
                ++it;
            }
        }
        
        // Create new vertex markers for this segment only
        auto startPoint = segment.getStartPoint();
        auto endPoint = segment.getEndPoint();
        
        // Create start point marker
        QGraphicsEllipseItem* startMarker = new QGraphicsEllipseItem(startPoint.x - 4, startPoint.y - 4, 8, 8);
        startMarker->setBrush(Qt::red);
        startMarker->setPen(QPen(Qt::black, 1));
        startMarker->setFlag(QGraphicsItem::ItemIsSelectable, true);
        startMarker->setFlag(QGraphicsItem::ItemIsMovable, true); // Aktifkan drag
        addItem(startMarker);
        
        VertexInfo startVertex{segmentIndex, true, startMarker, startPoint};
        m_vertices.push_back(startVertex);
        
        // Create end point marker
        QGraphicsEllipseItem* endMarker = new QGraphicsEllipseItem(endPoint.x - 4, endPoint.y - 4, 8, 8);
        endMarker->setBrush(Qt::blue);
        endMarker->setPen(QPen(Qt::black, 1));
        endMarker->setFlag(QGraphicsItem::ItemIsSelectable, true);
        endMarker->setFlag(QGraphicsItem::ItemIsMovable, true); // Aktifkan drag
        addItem(endMarker);
        
        VertexInfo endVertex{segmentIndex, false, endMarker, endPoint};
        m_vertices.push_back(endVertex);
        
        std::cout << "Updated vertex markers for segment " << segmentIndex << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error updating vertex markers: " << e.what() << std::endl;
    }
    
    m_recursionDepth--;
}

// Color helper methods
QColor ContourScene::getDefaultSegmentColor(size_t segmentIndex) const
{
    if (m_defaultColors.empty()) {
        return QColor(0, 0, 0); // Black as fallback
    }
    return m_defaultColors[segmentIndex % m_defaultColors.size()];
}

double ContourScene::getDefaultSegmentThickness(size_t segmentIndex) const
{
    return 2.0; // Default thickness
}

bool ContourScene::isValid() const
{
    return m_contour.isValid();
}

bool ContourScene::isClosed() const
{
    return m_contour.isClosed();
}

void ContourScene::setShowGrid(bool show)
{
    if (m_showGrid != show) {
        m_showGrid = show;
        update();
    }
}

void ContourScene::setSnapToGrid(bool snap)
{
    m_snapToGrid = snap;
}

void ContourScene::cleanupPreviewItem()
{
    if (m_previewItem) {
        try {
            // Check if item is still in the scene before removing
            if (m_previewItem->scene() == this) {
                removeItem(m_previewItem);
            }
            delete m_previewItem;
        } catch (const std::exception& e) {
            std::cerr << "Error cleaning up preview item: " << e.what() << std::endl;
        }
        m_previewItem = nullptr;
    }
}

void ContourScene::cleanupArcDrawing()
{
    if (m_arcCenterMarker) {
        try {
            if (m_arcCenterMarker->scene() == this) {
                removeItem(m_arcCenterMarker);
            }
            delete m_arcCenterMarker;
        } catch (const std::exception& e) {
            std::cerr << "Error cleaning up arc center marker: " << e.what() << std::endl;
        }
        m_arcCenterMarker = nullptr;
    }
    
    if (m_arcRadiusMarker) {
        try {
            if (m_arcRadiusMarker->scene() == this) {
                removeItem(m_arcRadiusMarker);
            }
            delete m_arcRadiusMarker;
        } catch (const std::exception& e) {
            std::cerr << "Error cleaning up arc radius marker: " << e.what() << std::endl;
        }
        m_arcRadiusMarker = nullptr;
    }
    
    m_isDrawingArc = false;
}

void ContourScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    std::cout << "[TRACE] mousePressEvent: m_editMode=" << (int)m_editMode << ", m_drawingMode=" << (int)m_drawingMode << std::endl;
    if (!event || event->button() != Qt::LeftButton) {
        QGraphicsScene::mousePressEvent(event);
                return;
            }
    try {
        QPointF pos = event->scenePos();
        if (m_snapToGrid) {
            pos = snapToGrid(pos);
        }
        if (m_editMode == EditMode::Edit) {
            // Check if clicking on a vertex
            VertexInfo* vertex = findVertexAt(pos);
            if (vertex) {
                startVertexEdit(vertex->segmentIndex, vertex->isStartPoint);
                m_editStartPos = pos;
                m_lastMousePos = pos;
                m_mouseUpdateCounter = 0;
                m_isMouseDragging = true;
                return;
            }
        } else if (m_editMode == EditMode::Draw) {
            if (m_drawingMode == DrawingMode::Line) {
                // Line drawing
                m_isDrawing = true;
                m_lastPoint = pos;
                
                // Create preview line item
                QGraphicsLineItem* lineItem = new QGraphicsLineItem();
            if (lineItem) {
                    lineItem->setLine(QLineF(m_lastPoint, m_lastPoint));
                    lineItem->setPen(QPen(Qt::blue, 2, Qt::DashLine));
                addItem(lineItem);
                m_previewItem = lineItem;
            }
                
                std::cout << "Started drawing line at: (" << m_lastPoint.x() << "," << m_lastPoint.y() << ")" << std::endl;
            } else if (m_drawingMode == DrawingMode::Arc) {
                if (!m_isDrawingArc) {
                    // Klik 1: set center
                    m_isDrawingArc = true;
                    m_arcCenter = pos;
                    m_arcStartPoint = pos;
                    m_arcStep = 1;
                    // Marker center
                    m_arcCenterMarker = new QGraphicsEllipseItem(pos.x() - 3, pos.y() - 3, 6, 6);
                    m_arcCenterMarker->setBrush(Qt::yellow);
                    m_arcCenterMarker->setPen(QPen(Qt::black));
                    addItem(m_arcCenterMarker);
                    std::cout << "[TRACE] Arc center set at: (" << pos.x() << "," << pos.y() << ")" << std::endl;
                } else if (m_arcStep == 1) {
                    // Klik 2: set start
                    m_arcStartPoint = pos;
                    m_arcStep = 2;
                    // Marker radius
                    m_arcRadiusMarker = new QGraphicsEllipseItem(pos.x() - 2, pos.y() - 2, 4, 4);
                    m_arcRadiusMarker->setBrush(QColor(255, 165, 0));
                    m_arcRadiusMarker->setPen(QPen(Qt::black));
                    addItem(m_arcRadiusMarker);
                    // Preview arc
                    double radius = std::sqrt(std::pow(pos.x() - m_arcCenter.x(), 2) + std::pow(pos.y() - m_arcCenter.y(), 2));
                    QPainterPath path;
                    QRectF rect(m_arcCenter.x() - radius, m_arcCenter.y() - radius, 2 * radius, 2 * radius);
                    path.moveTo(m_arcStartPoint);
                    path.arcTo(rect, 0, 0.1); // Dummy preview
                    QGraphicsPathItem* arcItem = new QGraphicsPathItem(path);
                    arcItem->setPen(QPen(Qt::blue, 2, Qt::DashLine));
                    addItem(arcItem);
                    m_previewItem = arcItem;
                    std::cout << "[TRACE] Arc start set at: (" << pos.x() << "," << pos.y() << ")" << std::endl;
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error in mousePressEvent: " << e.what() << std::endl;
        m_isDrawing = false;
        if (m_isDrawingArc) {
            cleanupArcDrawing();
        }
    }
}

void ContourScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    std::cout << "[TRACE] mouseMoveEvent: m_editMode=" << (int)m_editMode << ", m_drawingMode=" << (int)m_drawingMode << std::endl;
    if (!event) {
        QGraphicsScene::mouseMoveEvent(event);
        return;
    }
    try {
            QPointF currentPos = event->scenePos();
        if (m_snapToGrid) {
            currentPos = snapToGrid(currentPos);
        }
        if (m_editMode == EditMode::Edit && m_editingVertex) {
            // Prevent excessive updates during vertex editing
            if (m_isUpdatingScene || m_isInVertexUpdate || m_vertexUpdateInProgress) {
                return; // Skip if we're already updating
            }
            
            // Throttle updates to prevent recursion - more aggressive
            m_mouseUpdateCounter++;
            if (m_mouseUpdateCounter % 20 != 0) { // Update every 20th mouse move
                return;
            }
            
            // Check if position has changed significantly
            QPointF diff = currentPos - m_lastMousePos;
            if (std::abs(diff.x()) < 10.0 && std::abs(diff.y()) < 10.0) { // Larger threshold
                return; // Skip small movements
            }
            
            m_lastMousePos = currentPos;
            
            // Set vertex update flag to prevent recursion
            m_vertexUpdateInProgress = true;
            
            // Temporarily disable signal emission during vertex editing
            bool wasEmittingSignals = m_isEmittingSignals;
            m_isEmittingSignals = false;
            
            // Update vertex position during editing
            geometry::Point2D newPoint(currentPos.x(), currentPos.y());
            updateSegmentAfterVertexEdit(m_editingVertex->segmentIndex, m_editingVertex->isStartPoint, newPoint);
            updateVertexMarker(m_editingVertex->segmentIndex, m_editingVertex->isStartPoint, currentPos);
            
            // Restore signal emission
            m_isEmittingSignals = wasEmittingSignals;
            m_vertexUpdateInProgress = false;
            
        } else if (m_editMode == EditMode::Draw) {
            if (m_drawingMode == DrawingMode::Line && m_isDrawing && m_previewItem) {
                // Line drawing - update preview line
            QGraphicsLineItem* lineItem = qgraphicsitem_cast<QGraphicsLineItem*>(m_previewItem);
            if (lineItem) {
                    lineItem->setLine(QLineF(m_lastPoint, currentPos));
                }
            } else if (m_drawingMode == DrawingMode::Arc && m_isDrawingArc && m_arcStep == 2 && m_previewItem) {
                // Update preview arc
                QGraphicsPathItem* arcItem = qgraphicsitem_cast<QGraphicsPathItem*>(m_previewItem);
                if (arcItem) {
                    double radius = std::sqrt(std::pow(currentPos.x() - m_arcCenter.x(), 2) + std::pow(currentPos.y() - m_arcCenter.y(), 2));
                    double startAngle = std::atan2(m_arcStartPoint.y() - m_arcCenter.y(), m_arcStartPoint.x() - m_arcCenter.x());
                    double endAngle = std::atan2(currentPos.y() - m_arcCenter.y(), currentPos.x() - m_arcCenter.x());
                    QPainterPath path;
                    QRectF rect(m_arcCenter.x() - radius, m_arcCenter.y() - radius, 2 * radius, 2 * radius);
                    path.moveTo(m_arcStartPoint);
                    path.arcTo(rect, startAngle * 180.0 / M_PI, (endAngle - startAngle) * 180.0 / M_PI);
                    arcItem->setPath(path);
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error in mouseMoveEvent: " << e.what() << std::endl;
    }
}

void ContourScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    std::cout << "[TRACE] mouseReleaseEvent: m_editMode=" << (int)m_editMode << ", m_drawingMode=" << (int)m_drawingMode << std::endl;
    if (!event || event->button() != Qt::LeftButton) {
        QGraphicsScene::mouseReleaseEvent(event);
        return;
    }
    try {
        if (m_editMode == EditMode::Edit && m_editingVertex) {
            // Finish vertex editing
            finishVertexEdit();
            m_isMouseDragging = false;
            m_mouseUpdateCounter = 0;
            
            // Emit signal after vertex editing is complete
            if (m_isEmittingSignals && !m_isUpdatingScene && !m_updateInProgress) {
                std::cout << "[TRACE] emit contourModified" << std::endl;
                emit contourModified();
            }
        } else if (m_editMode == EditMode::Draw) {
            if (m_drawingMode == DrawingMode::Line && m_isDrawing) {
                // Line drawing - finish drawing
                QPointF endPos = event->scenePos();
            
            if (m_snapToGrid) {
                endPos = snapToGrid(endPos);
            }
            
                // Check if we have a valid line
                if (m_lastPoint != endPos) {
            // Add the line segment to the contour
            geometry::Point2D start(m_lastPoint.x(), m_lastPoint.y());
            geometry::Point2D end(endPos.x(), endPos.y());
            
                    addLineSegment(start, end);
                }
                
                cleanupPreviewItem();
            m_isDrawing = false;
                
                std::cout << "Finished drawing line to: (" << endPos.x() << "," << endPos.y() << ")" << std::endl;
            } else if (m_drawingMode == DrawingMode::Arc && m_isDrawingArc && m_arcStep == 2) {
                // Arc drawing - finish drawing
                QPointF endPos = event->scenePos();
                
                if (m_snapToGrid) {
                    endPos = snapToGrid(endPos);
                }
                
                // Check if we have a valid arc
                if (m_arcStartPoint != endPos) {
                    double radius = std::sqrt(std::pow(m_arcStartPoint.x() - m_arcCenter.x(), 2) + std::pow(m_arcStartPoint.y() - m_arcCenter.y(), 2));
                    double startAngle = std::atan2(m_arcStartPoint.y() - m_arcCenter.y(), m_arcStartPoint.x() - m_arcCenter.x());
                    double endAngle = std::atan2(endPos.y() - m_arcCenter.y(), endPos.x() - m_arcCenter.x());
                    bool clockwise = false;
                    double angleDiff = endAngle - startAngle;
                    if (angleDiff > M_PI) {
                        angleDiff -= 2 * M_PI;
                        clockwise = true;
                    } else if (angleDiff < -M_PI) {
                        angleDiff += 2 * M_PI;
                        clockwise = false;
                    } else {
                        clockwise = angleDiff < 0;
                    }
                    geometry::Point2D center(m_arcCenter.x(), m_arcCenter.y());
                    addArcSegment(center, radius, startAngle, endAngle, clockwise);
                }
                
                cleanupPreviewItem();
                cleanupArcDrawing();
                m_isDrawingArc = false;
                m_arcStep = 0;
                
                std::cout << "Finished drawing arc to: (" << endPos.x() << "," << endPos.y() << ")" << std::endl;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error in mouseReleaseEvent: " << e.what() << std::endl;
        cleanupPreviewItem();
        m_isDrawing = false;
        if (m_isDrawingArc) {
            cleanupArcDrawing();
        }
        if (m_editingVertex) {
            finishVertexEdit();
        }
    }
}

void ContourScene::drawBackground(QPainter *painter, const QRectF &rect)
{
    if (!painter) {
        return;
    }
    
    QGraphicsScene::drawBackground(painter, rect);
    
    if (m_showGrid) {
        QPen pen(Qt::lightGray, 0);
        painter->setPen(pen);
        
        qreal left = int(rect.left()) - std::fmod(int(rect.left()), m_gridSize);
        qreal top = int(rect.top()) - std::fmod(int(rect.top()), m_gridSize);
        
        QVarLengthArray<QLineF, 100> lines;
        
        for (qreal x = left; x < rect.right(); x += m_gridSize)
            lines.append(QLineF(x, rect.top(), x, rect.bottom()));
        for (qreal y = top; y < rect.bottom(); y += m_gridSize)
            lines.append(QLineF(rect.left(), y, rect.right(), y));
        
        if (!lines.isEmpty()) {
            painter->drawLines(lines.data(), lines.size());
        }
    }
}

void ContourScene::updateScene()
{
    std::cout << "[TRACE] updateScene called. Flags: isUpdatingScene=" << m_isUpdatingScene
              << ", updateInProgress=" << m_updateInProgress
              << ", isInVertexUpdate=" << m_isInVertexUpdate
              << ", vertexUpdateInProgress=" << m_vertexUpdateInProgress << std::endl;
    if (m_isUpdatingScene || m_updateInProgress) {
        std::cout << "Preventing recursive updateScene call" << std::endl;
            return;
        }
    m_isUpdatingScene = true;
    m_updateInProgress = true;

    QSignalBlocker blocker(this); // Blokir signal Qt internal selama update

    // Hapus semua item secara manual, jangan pakai QGraphicsScene::clear()
    QList<QGraphicsItem*> itemsToRemove = items();
    for (QGraphicsItem* item : itemsToRemove) {
        removeItem(item);
        delete item;
    }
    
    // Reset preview item pointer karena item sudah dihapus
    m_previewItem = nullptr;
    
    // Reset arc drawing markers
    m_arcCenterMarker = nullptr;
    m_arcRadiusMarker = nullptr;
    
    m_segmentVisuals.clear();
    m_vertices.clear(); // Clear vertex markers too

    std::cout << "Updating scene with " << m_contour.size() << " segments" << std::endl;

    // Create items for each segment
    for (size_t i = 0; i < m_contour.size(); ++i) {
            try {
                const auto& segment = m_contour.at(i);
            createSegmentItem(segment, i);
            } catch (const std::exception& e) {
                std::cerr << "Error processing segment " << i << ": " << e.what() << std::endl;
        }
    }

    // Recreate vertex markers if in edit mode
    if (m_editMode == EditMode::Edit) {
        createVertexMarkers();
    }

    std::cout << "Scene updated successfully" << std::endl;
    m_isUpdatingScene = false;
    m_updateInProgress = false;
}

void ContourScene::createSegmentItem(const contour::Segment& segment, size_t segmentIndex)
{
    try {
        auto segmentType = segment.getType();
        QColor color = getSegmentColor(segmentIndex);
        double thickness = getSegmentThickness(segmentIndex);
        
        switch (segmentType) {
            case contour::SegmentType::Line: {
                auto start = segment.getStartPoint();
                auto end = segment.getEndPoint();
                
                // Create line item
                QGraphicsLineItem* lineItem = new QGraphicsLineItem(start.x, start.y, end.x, end.y);
                lineItem->setPen(QPen(color, thickness));
                addItem(lineItem);
                
                // Store visual info
                m_segmentVisuals[segmentIndex] = {lineItem, color, thickness};
                
                std::cout << "Created line segment: (" << start.x << "," << start.y 
                          << ") to (" << end.x << "," << end.y << ") with color " 
                          << color.name().toStdString() << std::endl;
                break;
            }
            case contour::SegmentType::Arc: {
                try {
                    const auto& arcSegment = dynamic_cast<const contour::ArcSegment&>(segment);
                    auto center = arcSegment.getCenter();
                    auto radius = arcSegment.getRadius();
                    auto startAngle = arcSegment.getStartAngle();
                    auto endAngle = arcSegment.getEndAngle();
                    
                    QPainterPath path;
                    QRectF rect(center.x - radius, center.y - radius, 2 * radius, 2 * radius);
                    
                    // Convert angles from radians to degrees
                    double startDeg = startAngle * 180.0 / M_PI;
                    double endDeg = endAngle * 180.0 / M_PI;
                    
                    // Calculate sweep angle
                    double sweepDeg = endDeg - startDeg;
                    if (sweepDeg < 0) sweepDeg += 360.0;
                    
                    path.arcTo(rect, startDeg, sweepDeg);
                    
                    QGraphicsPathItem* pathItem = new QGraphicsPathItem(path);
                    pathItem->setPen(QPen(color, thickness));
                    addItem(pathItem);
                    
                    // Store visual info
                    m_segmentVisuals[segmentIndex] = {pathItem, color, thickness};
                    
                    std::cout << "Created arc segment: center(" << center.x << "," << center.y 
                              << "), radius=" << radius << " with color " 
                              << color.name().toStdString() << std::endl;
                } catch (const std::bad_cast& e) {
                    std::cerr << "Failed to cast to ArcSegment: " << e.what() << std::endl;
                }
                break;
            }
            default:
                std::cerr << "Unknown segment type: " << static_cast<int>(segmentType) << std::endl;
                break;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error creating segment item: " << e.what() << std::endl;
    }
}

QPointF ContourScene::snapToGrid(const QPointF& point) const
{
    if (std::isnan(point.x()) || std::isnan(point.y())) {
        return point;
    }
    
    qreal x = qRound(point.x() / m_gridSize) * m_gridSize;
    qreal y = qRound(point.y() / m_gridSize) * m_gridSize;
    return QPointF(x, y);
}

void ContourScene::createSegmentItemOnly(const contour::Segment& segment, size_t segmentIndex)
{
    try {
        auto segmentType = segment.getType();
        QColor color = getSegmentColor(segmentIndex);
        double thickness = getSegmentThickness(segmentIndex);
        
        switch (segmentType) {
            case contour::SegmentType::Line: {
                auto start = segment.getStartPoint();
                auto end = segment.getEndPoint();
                
                // Create line item
                QGraphicsLineItem* lineItem = new QGraphicsLineItem(start.x, start.y, end.x, end.y);
                lineItem->setPen(QPen(color, thickness));
                addItem(lineItem);
                
                // Store visual info
                m_segmentVisuals[segmentIndex] = {lineItem, color, thickness};
                
                std::cout << "Updated line segment: (" << start.x << "," << start.y 
                          << ") to (" << end.x << "," << end.y << ") with color " 
                          << color.name().toStdString() << std::endl;
                break;
            }
            case contour::SegmentType::Arc: {
                try {
                    const auto& arcSegment = dynamic_cast<const contour::ArcSegment&>(segment);
                    auto center = arcSegment.getCenter();
                    auto radius = arcSegment.getRadius();
                    auto startAngle = arcSegment.getStartAngle();
                    auto endAngle = arcSegment.getEndAngle();
                    
                    QPainterPath path;
                    QRectF rect(center.x - radius, center.y - radius, 2 * radius, 2 * radius);
                    
                    // Convert angles from radians to degrees
                    double startDeg = startAngle * 180.0 / M_PI;
                    double endDeg = endAngle * 180.0 / M_PI;
                    
                    // Calculate sweep angle
                    double sweepDeg = endDeg - startDeg;
                    if (sweepDeg < 0) sweepDeg += 360.0;
                    
                    path.arcTo(rect, startDeg, sweepDeg);
                    
                    QGraphicsPathItem* pathItem = new QGraphicsPathItem(path);
                    pathItem->setPen(QPen(color, thickness));
                    addItem(pathItem);
                    
                    // Store visual info
                    m_segmentVisuals[segmentIndex] = {pathItem, color, thickness};
                    
                    std::cout << "Updated arc segment: center(" << center.x << "," << center.y 
                              << "), radius=" << radius << " with color " 
                              << color.name().toStdString() << std::endl;
                } catch (const std::bad_cast& e) {
                    std::cerr << "Failed to cast to ArcSegment: " << e.what() << std::endl;
                }
                break;
            }
            default:
                std::cerr << "Unknown segment type: " << static_cast<int>(segmentType) << std::endl;
                break;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error creating segment item: " << e.what() << std::endl;
    }
} 