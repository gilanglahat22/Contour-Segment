#include "../include/MainWindow.h"
#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QGraphicsView>
#include <QAction>
#include <QIcon>
#include <QStyle>
#include <QStandardPaths>
#include <QDir>
#include <iostream>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_scene(nullptr)
    , m_view(nullptr)
    , m_toolBar(nullptr)
    , m_statusBar(nullptr)
    , m_newAction(nullptr)
    , m_openAction(nullptr)
    , m_saveAction(nullptr)
    , m_clearAction(nullptr)
    , m_gridAction(nullptr)
    , m_snapAction(nullptr)
    , m_aboutAction(nullptr)
{
    setupUI();
    createActions();
    setupMenuBar();
    setupToolBar();
    setupStatusBar();
    connectSignals();
    
    setWindowTitle("ContourSegment - Contour Editor");
    resize(800, 600);
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    // Create central widget
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    // Create layout
    QVBoxLayout* layout = new QVBoxLayout(centralWidget);
    
    // Create scene and view
    m_scene = new ContourScene(this);
    m_view = new QGraphicsView(m_scene);
    m_view->setRenderHint(QPainter::Antialiasing);
    m_view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_view->setDragMode(QGraphicsView::RubberBandDrag);
    
    layout->addWidget(m_view);
}

void MainWindow::createActions()
{
    // File actions
    m_newAction = new QAction("&New", this);
    m_newAction->setShortcut(QKeySequence::New);
    m_newAction->setStatusTip("Create a new contour");
    
    m_openAction = new QAction("&Open...", this);
    m_openAction->setShortcut(QKeySequence::Open);
    m_openAction->setStatusTip("Open an existing contour file");
    
    m_saveAction = new QAction("&Save", this);
    m_saveAction->setShortcut(QKeySequence::Save);
    m_saveAction->setStatusTip("Save the current contour");
    
    // Edit actions
    m_clearAction = new QAction("&Clear", this);
    m_clearAction->setStatusTip("Clear all segments");
    
    // View actions
    m_gridAction = new QAction("Show &Grid", this);
    m_gridAction->setCheckable(true);
    m_gridAction->setChecked(true);
    m_gridAction->setStatusTip("Show/hide grid");
    
    m_snapAction = new QAction("&Snap to Grid", this);
    m_snapAction->setCheckable(true);
    m_snapAction->setChecked(true);
    m_snapAction->setStatusTip("Snap drawing to grid");
    
    // Help actions
    m_aboutAction = new QAction("&About", this);
    m_aboutAction->setStatusTip("About ContourSegment");
}

void MainWindow::setupMenuBar()
{
    // File menu
    QMenu* fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction(m_newAction);
    fileMenu->addAction(m_openAction);
    fileMenu->addAction(m_saveAction);
    fileMenu->addSeparator();
    fileMenu->addAction("E&xit", this, &QWidget::close, QKeySequence::Quit);
    
    // Edit menu
    QMenu* editMenu = menuBar()->addMenu("&Edit");
    editMenu->addAction(m_clearAction);
    
    // View menu
    QMenu* viewMenu = menuBar()->addMenu("&View");
    viewMenu->addAction(m_gridAction);
    viewMenu->addAction(m_snapAction);
    
    // Help menu
    QMenu* helpMenu = menuBar()->addMenu("&Help");
    helpMenu->addAction(m_aboutAction);
}

void MainWindow::setupToolBar()
{
    m_toolBar = addToolBar("Main Toolbar");
    m_toolBar->addAction(m_newAction);
    m_toolBar->addAction(m_openAction);
    m_toolBar->addAction(m_saveAction);
    m_toolBar->addSeparator();
    m_toolBar->addAction(m_clearAction);
    m_toolBar->addSeparator();
    m_toolBar->addAction(m_gridAction);
    m_toolBar->addAction(m_snapAction);
}

void MainWindow::setupStatusBar()
{
    m_statusBar = statusBar();
    m_statusBar->showMessage("Ready");
}

void MainWindow::connectSignals()
{
    // Connect actions
    connect(m_newAction, &QAction::triggered, this, &MainWindow::onNewContour);
    connect(m_openAction, &QAction::triggered, this, &MainWindow::onOpenContour);
    connect(m_saveAction, &QAction::triggered, this, &MainWindow::onSaveContour);
    connect(m_clearAction, &QAction::triggered, this, &MainWindow::onClearContour);
    connect(m_gridAction, &QAction::triggered, this, &MainWindow::onToggleGrid);
    connect(m_snapAction, &QAction::triggered, this, &MainWindow::onToggleSnap);
    connect(m_aboutAction, &QAction::triggered, this, &MainWindow::onAbout);
    
    // Connect scene signals
    connect(m_scene, &ContourScene::contourModified, this, &MainWindow::onContourModified);
}

void MainWindow::onNewContour()
{
    if (m_scene->isValid() && !m_scene->isClosed()) {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "New Contour", 
            "Current contour is not closed. Do you want to create a new one?",
            QMessageBox::Yes | QMessageBox::No);
        
        if (reply == QMessageBox::No) {
            return;
        }
    }
    
    m_scene->clear();
    m_statusBar->showMessage("New contour created", 2000);
}

void MainWindow::onOpenContour()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open Contour File", 
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
        "Contour Files (*.cnt);;All Files (*)");
    
    if (!fileName.isEmpty()) {
        // TODO: Implement file loading
        m_statusBar->showMessage("File opened: " + fileName, 2000);
    }
}

void MainWindow::onSaveContour()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save Contour File", 
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
        "Contour Files (*.cnt);;All Files (*)");
    
    if (!fileName.isEmpty()) {
        // TODO: Implement file saving
        m_statusBar->showMessage("File saved: " + fileName, 2000);
    }
}

void MainWindow::onClearContour()
{
    m_scene->clear();
    m_statusBar->showMessage("Contour cleared", 2000);
}

void MainWindow::onToggleGrid()
{
    m_scene->setShowGrid(m_gridAction->isChecked());
    m_statusBar->showMessage(m_gridAction->isChecked() ? "Grid shown" : "Grid hidden", 1000);
}

void MainWindow::onToggleSnap()
{
    m_scene->setSnapToGrid(m_snapAction->isChecked());
    m_statusBar->showMessage(m_snapAction->isChecked() ? "Snap to grid enabled" : "Snap to grid disabled", 1000);
}

void MainWindow::onAbout()
{
    QMessageBox::about(this, "About ContourSegment", 
        "ContourSegment v1.0\n\n"
        "A contour editor for creating and manipulating geometric contours.\n\n"
        "Features:\n"
        "- Draw line segments\n"
        "- Draw arc segments\n"
        "- Grid snapping\n"
        "- Contour validation\n\n"
        "Click and drag to draw segments.");
}

void MainWindow::onContourModified()
{
    QString status = QString("Segments: %1 | Valid: %2 | Closed: %3")
        .arg(m_scene->isValid() ? "Yes" : "No")
        .arg(m_scene->isClosed() ? "Yes" : "No");
    
    m_statusBar->showMessage(status);
} 