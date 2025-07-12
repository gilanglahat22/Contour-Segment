#pragma once

#include "ContourScene.h"
#include <QMainWindow>
#include <QGraphicsView>
#include <QToolBar>
#include <QAction>
#include <QMenuBar>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void onNewContour();
    void onOpenContour();
    void onSaveContour();
    void onClearContour();
    void onToggleGrid();
    void onToggleSnap();
    void onAbout();
    void onContourModified();

private:
    void setupUI();
    void setupMenuBar();
    void setupToolBar();
    void setupStatusBar();
    void createActions();
    void connectSignals();

    ContourScene* m_scene;
    QGraphicsView* m_view;
    QToolBar* m_toolBar;
    QStatusBar* m_statusBar;
    
    // Actions
    QAction* m_newAction;
    QAction* m_openAction;
    QAction* m_saveAction;
    QAction* m_clearAction;
    QAction* m_gridAction;
    QAction* m_snapAction;
    QAction* m_aboutAction;
}; 