#pragma once

#include "Contour.h"
#include "Geometry.h"
#include <QGraphicsScene>
#include <QWidget>
#include <memory>

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

signals:
    void contourModified();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void drawBackground(QPainter* painter, const QRectF& rect) override;

private:
    void updateScene();
    void createSegmentItem(const contour::Segment& segment);
    QPointF snapToGrid(const QPointF& point) const;

    contour::Contour m_contour;
    bool m_showGrid;
    bool m_snapToGrid;
    double m_gridSize;
    double m_scale;
    bool m_isDrawing;
    QGraphicsItem* m_previewItem;
    QPointF m_lastPoint;
}; 