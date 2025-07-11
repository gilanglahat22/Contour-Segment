#pragma once

#include "ContourScene.h"
#include <QGraphicsView>
#include <QWheelEvent>
#include <QKeyEvent>

class ContourView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit ContourView(ContourScene* scene, QWidget* parent = nullptr);
    ~ContourView();

protected:
    void wheelEvent(QWheelEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    void zoomIn();
    void zoomOut();
    void zoomFit();
    void panView(const QPointF& delta);

    ContourScene* m_scene;
    bool m_panning;
    QPointF m_lastPanPoint;
    double m_zoomFactor;
}; 