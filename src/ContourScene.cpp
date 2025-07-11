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
        updateScene();
        emit contourModified();
    } catch (const std::exception& e) {
        std::cerr << "Error setting contour: " << e.what() << std::endl;
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

        auto segment = contour::createLineSegment(start, end);
        m_contour.addSegment(std::move(segment));
        updateScene();
        emit contourModified();
    } catch (const std::exception& e) {
        std::cerr << "Error adding line segment: " << e.what() << std::endl;
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
        m_contour.addSegment(std::move(segment));
        updateScene();
        emit contourModified();
    } catch (const std::exception& e) {
        std::cerr << "Error adding arc segment: " << e.what() << std::endl;
    }
}

void ContourScene::clear()
{
    QGraphicsScene::clear();
    m_contour = contour::Contour();
    emit contourModified();
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
    if (event->button() == Qt::LeftButton) {
        m_isDrawing = true;
        m_lastPoint = event->scenePos();
        m_previewItem = addLine(m_lastPoint.x(), m_lastPoint.y(), 
                               m_lastPoint.x(), m_lastPoint.y(), 
                               QPen(Qt::black));
    }
    QGraphicsScene::mousePressEvent(event);
}

void ContourScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_isDrawing && m_previewItem) {
        QPointF currentPos = event->scenePos();
        if (m_snapToGrid) {
            currentPos = snapToGrid(currentPos);
        }
        
        QLineF line(m_lastPoint, currentPos);
        static_cast<QGraphicsLineItem*>(m_previewItem)->setLine(line);
    }
    QGraphicsScene::mouseMoveEvent(event);
}

void ContourScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && m_isDrawing) {
        QPointF endPos = event->scenePos();
        if (m_snapToGrid) {
            endPos = snapToGrid(endPos);
        }
        
        // Add the line segment to the contour
        geometry::Point2D start(m_lastPoint.x(), m_lastPoint.y());
        geometry::Point2D end(endPos.x(), endPos.y());
        addLineSegment(start, end);
        
        // Clean up preview
        if (m_previewItem) {
            removeItem(m_previewItem);
            delete m_previewItem;
            m_previewItem = nullptr;
        }
        
        m_isDrawing = false;
    }
    QGraphicsScene::mouseReleaseEvent(event);
}

void ContourScene::drawBackground(QPainter *painter, const QRectF &rect)
{
    QGraphicsScene::drawBackground(painter, rect);
    
    if (m_showGrid) {
        QPen pen(Qt::lightGray, 0);
        painter->setPen(pen);
        
        qreal left = int(rect.left()) - (int(rect.left()) % m_gridSize);
        qreal top = int(rect.top()) - (int(rect.top()) % m_gridSize);
        
        QVarLengthArray<QLineF, 100> lines;
        
        for (qreal x = left; x < rect.right(); x += m_gridSize)
            lines.append(QLineF(x, rect.top(), x, rect.bottom()));
        for (qreal y = top; y < rect.bottom(); y += m_gridSize)
            lines.append(QLineF(rect.left(), y, rect.right(), y));
        
        painter->drawLines(lines.data(), lines.size());
    }
}

void ContourScene::updateScene()
{
    clear();
    
    // Create items for each segment
    for (size_t i = 0; i < m_contour.size(); ++i) {
        const auto& segment = m_contour[i];
        createSegmentItem(segment);
    }
}

void ContourScene::createSegmentItem(const contour::Segment& segment)
{
    switch (segment.getType()) {
        case contour::SegmentType::Line: {
            auto start = segment.getStartPoint();
            auto end = segment.getEndPoint();
            addLine(start.x, start.y, end.x, end.y, QPen(Qt::black));
            break;
        }
        case contour::SegmentType::Arc: {
            auto center = segment.getCenter();
            auto radius = segment.getRadius();
            auto startAngle = segment.getStartAngle();
            auto endAngle = segment.getEndAngle();
            
            QPainterPath path;
            QRectF rect(center.x - radius, center.y - radius, 
                       2 * radius, 2 * radius);
            
            // Convert angles from radians to degrees
            double startDeg = startAngle * 180.0 / M_PI;
            double endDeg = endAngle * 180.0 / M_PI;
            
            // Calculate sweep angle
            double sweepDeg = endDeg - startDeg;
            if (sweepDeg < 0) sweepDeg += 360.0;
            
            path.arcTo(rect, startDeg, sweepDeg);
            addPath(path, QPen(Qt::black));
            break;
        }
    }
}

QPointF ContourScene::snapToGrid(const QPointF& point) const
{
    qreal x = qRound(point.x() / m_gridSize) * m_gridSize;
    qreal y = qRound(point.y() / m_gridSize) * m_gridSize;
    return QPointF(x, y);
} 