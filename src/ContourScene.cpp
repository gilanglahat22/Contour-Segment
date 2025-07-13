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

ContourScene::ContourScene(QWidget* parent)
    : QGraphicsScene(parent)
    , m_showGrid(true)
    , m_snapToGrid(true)
    , m_gridSize(20.0)
    , m_scale(1.0)
    , m_isDrawing(false)
    , m_previewItem(nullptr)
{
    setSceneRect(-500, -500, 1000, 1000);
    std::cout << "ContourScene initialized" << std::endl;
}

void ContourScene::setContour(const contour::Contour& contour)
{
    try {
        m_contour = contour;
        
        // Update scene without emitting signal to prevent recursive calls
        updateScene();
        emit contourModified();
    } catch (const std::exception& e) {
        std::cerr << "Error setting contour: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown error setting contour" << std::endl;
    }
}

void ContourScene::addLineSegment(const geometry::Point2D& start, const geometry::Point2D& end)
{
    try {
        // Check if points are valid
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

        // Check for reasonable bounds
        if (std::abs(start.x) > 1e6 || std::abs(start.y) > 1e6 ||
            std::abs(end.x) > 1e6 || std::abs(end.y) > 1e6) {
            std::cerr << "Line segment points out of reasonable bounds" << std::endl;
            return;
        }

        auto segment = contour::createLineSegment(start, end);
        if (!segment) {
            std::cerr << "Failed to create line segment" << std::endl;
            return;
        }
        
        m_contour.addSegment(std::move(segment));
        
        // Update scene without emitting signal to prevent recursive calls
        updateScene();
        emit contourModified();
    } catch (const std::exception& e) {
        std::cerr << "Error adding line segment: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown error adding line segment" << std::endl;
    }
}

void ContourScene::addArcSegment(const geometry::Point2D& center, double radius, 
                                double startAngle, double endAngle, bool clockwise)
{
    try {
        // Validate parameters
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
        
        // Check for reasonable bounds
        if (std::abs(center.x) > 1e6 || std::abs(center.y) > 1e6 || radius > 1e6) {
            std::cerr << "Arc segment parameters out of reasonable bounds" << std::endl;
            return;
        }
        
        // Normalize angles to [0, 2π)
        startAngle = std::fmod(startAngle, 2 * M_PI);
        if (startAngle < 0) startAngle += 2 * M_PI;
        
        endAngle = std::fmod(endAngle, 2 * M_PI);
        if (endAngle < 0) endAngle += 2 * M_PI;
        
        // Check if angles are too close
        if (std::abs(startAngle - endAngle) < 1e-6) {
            std::cerr << "Start and end angles are too close" << std::endl;
            return;
        }

        auto segment = contour::createArcSegment(center, radius, startAngle, endAngle, clockwise);
        if (!segment) {
            std::cerr << "Failed to create arc segment" << std::endl;
            return;
        }
        
        m_contour.addSegment(std::move(segment));
        
        // Update scene without emitting signal to prevent recursive calls
        updateScene();
        emit contourModified();
    } catch (const std::exception& e) {
        std::cerr << "Error adding arc segment: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown error adding arc segment" << std::endl;
    }
}

void ContourScene::clear()
{
    try {
        QGraphicsScene::clear();
        m_contour = contour::Contour();
        emit contourModified();
    } catch (const std::exception& e) {
        std::cerr << "Error clearing scene: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown error clearing scene" << std::endl;
    }
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

void ContourScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (!event) {
        return;
    }
    
    try {
        if (event->button() == Qt::LeftButton) {
            m_isDrawing = true;
            m_lastPoint = event->scenePos();
            
            // Validate point coordinates
            if (std::isnan(m_lastPoint.x()) || std::isnan(m_lastPoint.y())) {
                std::cerr << "Invalid point coordinates in mousePressEvent" << std::endl;
                m_isDrawing = false;
                return;
            }
            
            // Create preview line item safely
            QGraphicsLineItem* lineItem = new QGraphicsLineItem(m_lastPoint.x(), m_lastPoint.y(), 
                                                               m_lastPoint.x(), m_lastPoint.y());
            if (lineItem) {
                lineItem->setPen(QPen(Qt::black));
                addItem(lineItem);
                m_previewItem = lineItem;
            }
        }
        QGraphicsScene::mousePressEvent(event);
    } catch (const std::exception& e) {
        std::cerr << "Error in mousePressEvent: " << e.what() << std::endl;
        m_isDrawing = false;
    } catch (...) {
        std::cerr << "Unknown error in mousePressEvent" << std::endl;
        m_isDrawing = false;
    }
}

void ContourScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (!event) {
        return;
    }
    
    try {
        if (m_isDrawing && m_previewItem) {
            QPointF currentPos = event->scenePos();
            
            // Validate point coordinates
            if (std::isnan(currentPos.x()) || std::isnan(currentPos.y())) {
                return;
            }
            
            if (m_snapToGrid) {
                currentPos = snapToGrid(currentPos);
            }
            
            QGraphicsLineItem* lineItem = qgraphicsitem_cast<QGraphicsLineItem*>(m_previewItem);
            if (lineItem) {
                QLineF line(m_lastPoint, currentPos);
                lineItem->setLine(line);
            }
        }
        QGraphicsScene::mouseMoveEvent(event);
    } catch (const std::exception& e) {
        std::cerr << "Error in mouseMoveEvent: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown error in mouseMoveEvent" << std::endl;
    }
}

void ContourScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (!event) {
        return;
    }
    
    try {
        if (event->button() == Qt::LeftButton && m_isDrawing) {
            QPointF endPos = event->scenePos();
            
            // Validate point coordinates
            if (std::isnan(endPos.x()) || std::isnan(endPos.y())) {
                std::cerr << "Invalid end point coordinates in mouseReleaseEvent" << std::endl;
                // Clean up preview even if points are invalid
                if (m_previewItem) {
                    removeItem(m_previewItem);
                    delete m_previewItem;
                    m_previewItem = nullptr;
                }
                m_isDrawing = false;
                return;
            }
            
            if (m_snapToGrid) {
                endPos = snapToGrid(endPos);
            }
            
            // Validate points before creating segment
            if (std::isnan(m_lastPoint.x()) || std::isnan(m_lastPoint.y()) ||
                std::isnan(endPos.x()) || std::isnan(endPos.y())) {
                std::cerr << "Invalid point coordinates in mouseReleaseEvent" << std::endl;
                // Clean up preview even if points are invalid
                if (m_previewItem) {
                    removeItem(m_previewItem);
                    delete m_previewItem;
                    m_previewItem = nullptr;
                }
                m_isDrawing = false;
                return;
            }
            
            // Add the line segment to the contour
            geometry::Point2D start(m_lastPoint.x(), m_lastPoint.y());
            geometry::Point2D end(endPos.x(), endPos.y());
            
            // Clean up preview before adding segment to avoid conflicts
            if (m_previewItem) {
                removeItem(m_previewItem);
                delete m_previewItem;
                m_previewItem = nullptr;
            }
            
            addLineSegment(start, end);
            m_isDrawing = false;
        }
        QGraphicsScene::mouseReleaseEvent(event);
    } catch (const std::exception& e) {
        std::cerr << "Error in mouseReleaseEvent: " << e.what() << std::endl;
        // Clean up on error
        if (m_previewItem) {
            removeItem(m_previewItem);
            delete m_previewItem;
            m_previewItem = nullptr;
        }
        m_isDrawing = false;
    } catch (...) {
        std::cerr << "Unknown error in mouseReleaseEvent" << std::endl;
        // Clean up on error
        if (m_previewItem) {
            removeItem(m_previewItem);
            delete m_previewItem;
            m_previewItem = nullptr;
        }
        m_isDrawing = false;
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
    try {
        // Prevent recursive calls
        static bool isUpdating = false;
        if (isUpdating) {
            return;
        }
        isUpdating = true;
        
        // Clear existing items first
        QGraphicsScene::clear();
        
        // Create items for each segment with bounds checking
        size_t contourSize = m_contour.size();
        for (size_t i = 0; i < contourSize; ++i) {
            try {
                const auto& segment = m_contour.at(i);
                createSegmentItem(segment);
            } catch (const std::exception& e) {
                std::cerr << "Error processing segment " << i << ": " << e.what() << std::endl;
            } catch (...) {
                std::cerr << "Unknown error processing segment " << i << std::endl;
            }
        }
        
        isUpdating = false;
    } catch (const std::exception& e) {
        std::cerr << "Error updating scene: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown error updating scene" << std::endl;
    }
}

void ContourScene::createSegmentItem(const contour::Segment& segment)
{
    try {
        // Validate segment type first
        auto segmentType = segment.getType();
        
        switch (segmentType) {
            case contour::SegmentType::Line: {
                auto start = segment.getStartPoint();
                auto end = segment.getEndPoint();
                
                // Validate points
                if (std::isnan(start.x) || std::isnan(start.y) || 
                    std::isnan(end.x) || std::isnan(end.y)) {
                    std::cerr << "Invalid line segment points" << std::endl;
                    return;
                }
                
                // Check for reasonable bounds
                if (std::abs(start.x) > 1e6 || std::abs(start.y) > 1e6 ||
                    std::abs(end.x) > 1e6 || std::abs(end.y) > 1e6) {
                    std::cerr << "Line segment points out of reasonable bounds" << std::endl;
                    return;
                }
                
                addLine(start.x, start.y, end.x, end.y, QPen(Qt::black));
                break;
            }
            case contour::SegmentType::Arc: {
                try {
                    const auto& arcSegment = dynamic_cast<const contour::ArcSegment&>(segment);
                    auto center = arcSegment.getCenter();
                    auto radius = arcSegment.getRadius();
                    auto startAngle = arcSegment.getStartAngle();
                    auto endAngle = arcSegment.getEndAngle();
                    
                    // Validate arc parameters
                    if (std::isnan(center.x) || std::isnan(center.y) || 
                        std::isnan(radius) || radius <= 0 ||
                        std::isnan(startAngle) || std::isnan(endAngle)) {
                        std::cerr << "Invalid arc segment parameters" << std::endl;
                        return;
                    }
                    
                    // Check for reasonable bounds
                    if (std::abs(center.x) > 1e6 || std::abs(center.y) > 1e6 || radius > 1e6) {
                        std::cerr << "Arc segment parameters out of reasonable bounds" << std::endl;
                        return;
                    }
                    
                    QPainterPath path;
                    QRectF rect(center.x - radius, center.y - radius, 
                               2 * radius, 2 * radius);
                    
                    // Convert angles from radians to degrees
                    double startDeg = startAngle * 180.0 / M_PI;
                    double endDeg = endAngle * 180.0 / M_PI;
                    
                    // Calculate sweep angle
                    double sweepDeg = endDeg - startDeg;
                    if (sweepDeg < 0) sweepDeg += 360.0;
                    
                    // Validate sweep angle
                    if (std::isnan(sweepDeg) || std::abs(sweepDeg) > 360.0) {
                        std::cerr << "Invalid sweep angle for arc" << std::endl;
                        return;
                    }
                    
                    path.arcTo(rect, startDeg, sweepDeg);
                    addPath(path, QPen(Qt::black));
                } catch (const std::bad_cast& e) {
                    std::cerr << "Failed to cast to ArcSegment: " << e.what() << std::endl;
                } catch (const std::exception& e) {
                    std::cerr << "Error creating arc segment: " << e.what() << std::endl;
                }
                break;
            }
            default:
                std::cerr << "Unknown segment type: " << static_cast<int>(segmentType) << std::endl;
                break;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error creating segment item: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown error creating segment item" << std::endl;
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