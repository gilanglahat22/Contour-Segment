#include "../include/ContourView.h"
#include <QWheelEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QApplication>
#include <QScrollBar>
#include <QTransform>
#include <cmath>

ContourView::ContourView(ContourScene* scene, QWidget* parent)
    : QGraphicsView(scene, parent)
    , m_scene(scene)
    , m_panning(false)
    , m_lastPanPoint(0, 0)
    , m_zoomFactor(1.0)
{
    setRenderHint(QPainter::Antialiasing);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setDragMode(QGraphicsView::RubberBandDrag);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);
    
    // Set initial view
    setSceneRect(-500, -500, 1000, 1000);
    centerOn(0, 0);
}

ContourView::~ContourView()
{
}

void ContourView::wheelEvent(QWheelEvent* event)
{
    if (event->modifiers() & Qt::ControlModifier) {
        // Zoom with Ctrl+Wheel
        if (event->angleDelta().y() > 0) {
            zoomIn();
        } else {
            zoomOut();
        }
        event->accept();
    } else {
        // Normal scrolling
        QGraphicsView::wheelEvent(event);
    }
}

void ContourView::keyPressEvent(QKeyEvent* event)
{
    switch (event->key()) {
        case Qt::Key_Plus:
        case Qt::Key_Equal:
            if (event->modifiers() & Qt::ControlModifier) {
                zoomIn();
                event->accept();
            }
            break;
            
        case Qt::Key_Minus:
            if (event->modifiers() & Qt::ControlModifier) {
                zoomOut();
                event->accept();
            }
            break;
            
        case Qt::Key_0:
            if (event->modifiers() & Qt::ControlModifier) {
                zoomFit();
                event->accept();
            }
            break;
            
        case Qt::Key_Space:
            // Toggle pan mode
            if (dragMode() == QGraphicsView::RubberBandDrag) {
                setDragMode(QGraphicsView::ScrollHandDrag);
            } else {
                setDragMode(QGraphicsView::RubberBandDrag);
            }
            event->accept();
            break;
            
        default:
            QGraphicsView::keyPressEvent(event);
            break;
    }
}

void ContourView::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::MiddleButton || 
        (event->button() == Qt::LeftButton && event->modifiers() & Qt::AltModifier)) {
        // Start panning
        m_panning = true;
        m_lastPanPoint = event->pos();
        setCursor(Qt::ClosedHandCursor);
        event->accept();
    } else {
        QGraphicsView::mousePressEvent(event);
    }
}

void ContourView::mouseMoveEvent(QMouseEvent* event)
{
    if (m_panning) {
        QPointF delta = event->pos() - m_lastPanPoint;
        panView(delta);
        m_lastPanPoint = event->pos();
        event->accept();
    } else {
        QGraphicsView::mouseMoveEvent(event);
    }
}

void ContourView::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::MiddleButton || 
        (event->button() == Qt::LeftButton && event->modifiers() & Qt::AltModifier)) {
        // Stop panning
        m_panning = false;
        setCursor(Qt::ArrowCursor);
        event->accept();
    } else {
        QGraphicsView::mouseReleaseEvent(event);
    }
}

void ContourView::zoomIn()
{
    m_zoomFactor *= 1.2;
    scale(1.2, 1.2);
}

void ContourView::zoomOut()
{
    m_zoomFactor /= 1.2;
    scale(1.0 / 1.2, 1.0 / 1.2);
}

void ContourView::zoomFit()
{
    // Reset zoom
    resetTransform();
    m_zoomFactor = 1.0;
    
    // Fit view to scene
    fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
    
    // Update zoom factor
    QTransform transform = this->transform();
    m_zoomFactor = transform.m11(); // Use horizontal scale as zoom factor
}

void ContourView::panView(const QPointF& delta)
{
    QScrollBar* hBar = horizontalScrollBar();
    QScrollBar* vBar = verticalScrollBar();
    
    hBar->setValue(hBar->value() - delta.x());
    vBar->setValue(vBar->value() - delta.y());
} 