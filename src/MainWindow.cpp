#include "../include/MainWindow.h"
#include "../include/ContourScene.h"
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
#include <QColorDialog>
#include <QSignalBlocker>
#include <QGroupBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QFormLayout>
#include <iostream>
#include <cmath>
#define _USE_MATH_DEFINES
#include <math.h>
#include "Segment.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_scene(nullptr)
    , m_view(nullptr)
    , m_toolBar(nullptr)
    , m_statusBar(nullptr)
    , m_segmentManagementPanel(nullptr)
    , m_segmentTypeCombo(nullptr)
    , m_insertPositionSpinBox(nullptr)
    , m_addSegmentButton(nullptr)
    , m_insertSegmentButton(nullptr)
    , m_removeSegmentButton(nullptr)
    , m_updateSegmentButton(nullptr)
    , m_validationStatusLabel(nullptr)
    , m_segmentCountLabel(nullptr)
    , m_segmentSelectCombo(nullptr)
    , m_lineStartXEdit(nullptr)
    , m_lineStartYEdit(nullptr)
    , m_lineEndXEdit(nullptr)
    , m_lineEndYEdit(nullptr)
    , m_arcCenterXEdit(nullptr)
    , m_arcCenterYEdit(nullptr)
    , m_arcRadiusEdit(nullptr)
    , m_arcStartAngleEdit(nullptr)
    , m_arcEndAngleEdit(nullptr)
    , m_arcClockwiseCheck(nullptr)
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
    setupSegmentManagementPanel();
    
    // Verify QComboBox is properly created before connecting signals
    if (m_segmentSelectCombo) {
        std::cout << "[TRACE] QComboBox is valid, proceeding with signal connection..." << std::endl;
        std::cout << "[TRACE] QComboBox current index: " << m_segmentSelectCombo->currentIndex() << std::endl;
        std::cout << "[TRACE] QComboBox item count: " << m_segmentSelectCombo->count() << std::endl;
    } else {
        std::cout << "[ERROR] QComboBox is null after setupSegmentManagementPanel!" << std::endl;
    }
    
    connectSignals();
    std::cout << "[TRACE] m_segmentSelectCombo address (after connectSignals): " << m_segmentSelectCombo << std::endl;
    
    setWindowTitle("ContourSegment - Contour Editor with Segment Management");
    resize(1200, 800);

    // Tambahkan dark style agar menu, panel, dan konten kontras
    QString darkStyle = R"(
        QMainWindow, QWidget, QMenuBar, QMenu, QToolBar, QStatusBar {
            background-color:rgb(2, 72, 143);
            color: #f0f0f0;
        }
        QMenuBar, QMenu {
            background-color: #232629;
            color: #f0f0f0;
        }
        QMenuBar::item:selected, QMenu::item:selected {
            background: #444;
        }
        QToolBar, QStatusBar {
            background-color: #232629;
            color: #f0f0f0;
        }
        QLabel, QComboBox, QSpinBox, QSlider, QPushButton, QLineEdit, QCheckBox {
            background-color: #232629;
            color: #f0f0f0;
            border: 1px solid #444;
        }
        QComboBox QAbstractItemView {
            background: #232629;
            color: #f0f0f0;
            selection-background-color: #444;
            selection-color: #fff;
        }
        QPushButton {
            background-color: #444;
            color: #f0f0f0;
            border-radius: 4px;
            padding: 4px 8px;
        }
        QPushButton:disabled {
            background-color: #333;
            color: #888;
        }
        QSlider::groove:horizontal {
            background: #444;
            height: 6px;
            border-radius: 3px;
        }
        QSlider::handle:horizontal {
            background: #f0f0f0;
            border: 1px solid #888;
            width: 14px;
            margin: -4px 0;
            border-radius: 7px;
        }
        QGroupBox {
            font-weight: bold;
            border: 2px solid #444;
            border-radius: 5px;
            margin-top: 1ex;
            padding-top: 10px;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 5px 0 5px;
        }
    )";
    qApp->setStyleSheet(darkStyle);
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    // Create central widget
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    // Create main layout
    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);
    
    // Create left panel for scene
    QVBoxLayout* leftLayout = new QVBoxLayout();
    
    // Create scene and view
    m_scene = new ContourScene(this);
    m_view = new QGraphicsView(m_scene);
    m_view->setRenderHint(QPainter::Antialiasing);
    m_view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_view->setDragMode(QGraphicsView::RubberBandDrag);
    
    leftLayout->addWidget(m_view);
    
    // Create right panels for controls
    setupSegmentManagementPanel();
    
    // Add panels to main layout
    mainLayout->addLayout(leftLayout, 2); // Scene takes most space
    mainLayout->addWidget(m_segmentManagementPanel, 0); // Segment management panel takes minimum space
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
    QAction* exitAction = new QAction("E&xit", this);
    exitAction->setShortcut(QKeySequence::Quit);
    exitAction->setStatusTip("Exit the application");
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
    fileMenu->addAction(exitAction);
    
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

void MainWindow::setupSegmentManagementPanel()
{
    m_segmentManagementPanel = new QGroupBox("Segment Management", this);
    m_segmentManagementPanel->setMinimumWidth(300);
    m_segmentManagementPanel->setMaximumWidth(300);
    m_segmentManagementPanel->setStyleSheet(R"(
        QGroupBox {
            font-weight: bold;
            border: 2px solid #444;
            border-radius: 5px;
            margin-top: 1ex;
            padding-top: 10px;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 5px 0 5px;
        }
    )");

    QVBoxLayout* segmentManagementLayout = new QVBoxLayout(m_segmentManagementPanel);
    segmentManagementLayout->setAlignment(Qt::AlignTop);
    segmentManagementLayout->setContentsMargins(10, 10, 10, 10);

    // Segment Type Combo Box
    m_segmentTypeCombo = new QComboBox(m_segmentManagementPanel);
    m_segmentTypeCombo->addItem("Line");
    m_segmentTypeCombo->addItem("Arc");
    segmentManagementLayout->addWidget(m_segmentTypeCombo);

    // Insert Position Spin Box
    m_insertPositionSpinBox = new QSpinBox(m_segmentManagementPanel);
    m_insertPositionSpinBox->setMinimum(0);
    m_insertPositionSpinBox->setMaximum(100); // Arbitrary large number for now
    segmentManagementLayout->addWidget(m_insertPositionSpinBox);

    // Add Segment Button
    m_addSegmentButton = new QPushButton("Add Segment", m_segmentManagementPanel);
    segmentManagementLayout->addWidget(m_addSegmentButton);

    // Insert Segment Button
    m_insertSegmentButton = new QPushButton("Insert Segment", m_segmentManagementPanel);
    segmentManagementLayout->addWidget(m_insertSegmentButton);

    // Remove Segment Button
    m_removeSegmentButton = new QPushButton("Remove Segment", m_segmentManagementPanel);
    segmentManagementLayout->addWidget(m_removeSegmentButton);

    // Validation Status Label
    m_validationStatusLabel = new QLabel("Validation Status: N/A", m_segmentManagementPanel);
    segmentManagementLayout->addWidget(m_validationStatusLabel);

    // Segment Count Label
    m_segmentCountLabel = new QLabel("Total Segments: 0", m_segmentManagementPanel);
    segmentManagementLayout->addWidget(m_segmentCountLabel);

    // Line Segment Form
    QGroupBox* lineSegmentGroup = new QGroupBox("Line Segment", m_segmentManagementPanel);
    QFormLayout* lineSegmentLayout = new QFormLayout(lineSegmentGroup);
    lineSegmentLayout->setContentsMargins(10, 5, 10, 5);

    m_lineStartXEdit = new QLineEdit(lineSegmentGroup);
    m_lineStartXEdit->setPlaceholderText("X");
    lineSegmentLayout->addRow("Start X:", m_lineStartXEdit);

    m_lineStartYEdit = new QLineEdit(lineSegmentGroup);
    m_lineStartYEdit->setPlaceholderText("Y");
    lineSegmentLayout->addRow("Start Y:", m_lineStartYEdit);

    m_lineEndXEdit = new QLineEdit(lineSegmentGroup);
    m_lineEndXEdit->setPlaceholderText("X");
    lineSegmentLayout->addRow("End X:", m_lineEndXEdit);

    m_lineEndYEdit = new QLineEdit(lineSegmentGroup);
    m_lineEndYEdit->setPlaceholderText("Y");
    lineSegmentLayout->addRow("End Y:", m_lineEndYEdit);

    segmentManagementLayout->addWidget(lineSegmentGroup);

    // Arc Segment Form
    QGroupBox* arcSegmentGroup = new QGroupBox("Arc Segment", m_segmentManagementPanel);
    QFormLayout* arcSegmentLayout = new QFormLayout(arcSegmentGroup);
    arcSegmentLayout->setContentsMargins(10, 5, 10, 5);

    m_arcCenterXEdit = new QLineEdit(arcSegmentGroup);
    m_arcCenterXEdit->setPlaceholderText("X");
    arcSegmentLayout->addRow("Center X:", m_arcCenterXEdit);

    m_arcCenterYEdit = new QLineEdit(arcSegmentGroup);
    m_arcCenterYEdit->setPlaceholderText("Y");
    arcSegmentLayout->addRow("Center Y:", m_arcCenterYEdit);

    m_arcRadiusEdit = new QLineEdit(arcSegmentGroup);
    m_arcRadiusEdit->setPlaceholderText("Radius");
    arcSegmentLayout->addRow("Radius:", m_arcRadiusEdit);

    m_arcStartAngleEdit = new QLineEdit(arcSegmentGroup);
    m_arcStartAngleEdit->setPlaceholderText("Start Angle (deg)");
    arcSegmentLayout->addRow("Start Angle:", m_arcStartAngleEdit);

    m_arcEndAngleEdit = new QLineEdit(arcSegmentGroup);
    m_arcEndAngleEdit->setPlaceholderText("End Angle (deg)");
    arcSegmentLayout->addRow("End Angle:", m_arcEndAngleEdit);

    m_arcClockwiseCheck = new QCheckBox("Clockwise", arcSegmentGroup);
    arcSegmentLayout->addRow("Clockwise:", m_arcClockwiseCheck);

    segmentManagementLayout->addWidget(arcSegmentGroup);

    // Combo box pilih segmen
    m_segmentSelectCombo = new QComboBox(m_segmentManagementPanel);
    segmentManagementLayout->addWidget(m_segmentSelectCombo);
    // Tombol update segmen
    m_updateSegmentButton = new QPushButton("Update Segment", m_segmentManagementPanel);
    segmentManagementLayout->addWidget(m_updateSegmentButton);
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
    
    // Connect segment management panel signals
    connect(m_addSegmentButton, &QPushButton::clicked, this, &MainWindow::onAddSegmentClicked);
    connect(m_insertSegmentButton, &QPushButton::clicked, this, &MainWindow::onInsertSegmentClicked);
    connect(m_removeSegmentButton, &QPushButton::clicked, this, &MainWindow::onRemoveSegmentClicked);
    connect(m_segmentTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int idx){
        // Enable/disable form sesuai tipe
        bool isLine = (idx == 0);
        m_lineStartXEdit->setEnabled(isLine);
        m_lineStartYEdit->setEnabled(isLine);
        m_lineEndXEdit->setEnabled(isLine);
        m_lineEndYEdit->setEnabled(isLine);
        m_arcCenterXEdit->setEnabled(!isLine);
        m_arcCenterYEdit->setEnabled(!isLine);
        m_arcRadiusEdit->setEnabled(!isLine);
        m_arcStartAngleEdit->setEnabled(!isLine);
        m_arcEndAngleEdit->setEnabled(!isLine);
        m_arcClockwiseCheck->setEnabled(!isLine);
    });
    connect(m_insertPositionSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &MainWindow::onInsertPositionChanged);
    connect(m_segmentSelectCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onSegmentSelectChanged);
    connect(m_updateSegmentButton, &QPushButton::clicked, this, &MainWindow::onUpdateSegmentClicked);
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
        .arg(m_scene->getSegmentCount())
        .arg(m_scene->isValid() ? "Yes" : "No")
        .arg(m_scene->isClosed() ? "Yes" : "No");
    
    m_statusBar->showMessage(status);
    updateSegmentManagementPanel();
}

void MainWindow::onSegmentTypeChanged(int index)
{
    // Update the insert position maximum based on current segment count
    size_t segmentCount = m_scene->getSegmentCount();
    m_insertPositionSpinBox->setMaximum(static_cast<int>(segmentCount));
    
    // Update validation status
    updateValidationStatus();
}

void MainWindow::onInsertPositionChanged(int value)
{
    // This slot can be used for additional validation or UI updates
    // For now, just update the validation status
    updateValidationStatus();
}

void MainWindow::updateSegmentManagementPanel()
{
    size_t segmentCount = m_scene->getSegmentCount();
    m_segmentCountLabel->setText(QString("Total Segments: %1").arg(segmentCount));
    m_insertPositionSpinBox->setMaximum(static_cast<int>(segmentCount));
    m_segmentSelectCombo->clear();
    for (size_t i = 0; i < segmentCount; ++i) {
        m_segmentSelectCombo->addItem(QString("Segment %1").arg(i+1));
    }
    if (segmentCount > 0) fillFormWithSegment(0);
    updateValidationStatus();
    
    // Update edit panel as well
    // updateEditPanel(); // This line is removed as per the new_code
}

void MainWindow::updateValidationStatus()
{
    if (!m_scene) {
        m_validationStatusLabel->setText("Validation Status: N/A");
        return;
    }
    
    bool isValid = m_scene->isValid();
    bool isClosed = m_scene->isClosed();
    
    QString status;
    if (isValid) {
        if (isClosed) {
            status = "Valid & Closed";
            m_validationStatusLabel->setStyleSheet("QLabel { color: #00ff00; }"); // Green
        } else {
            status = "Valid (Open)";
            m_validationStatusLabel->setStyleSheet("QLabel { color: #ffff00; }"); // Yellow
        }
    } else {
        status = "Invalid";
        m_validationStatusLabel->setStyleSheet("QLabel { color: #ff0000; }"); // Red
    }
    
    m_validationStatusLabel->setText(QString("Validation Status: %1").arg(status));
} 

void MainWindow::onSegmentSelectChanged(int index) {
    fillFormWithSegment(index);
}

void MainWindow::onUpdateSegmentClicked() {
    int idx = m_segmentSelectCombo->currentIndex();
    if (idx < 0 || idx >= (int)m_scene->getSegmentCount()) return;
    // Cek tipe segmen yang dipilih
    auto& contour = m_scene->getContour();
    auto& seg = contour[idx];
    if (seg.getType() == contour::SegmentType::Line && m_segmentTypeCombo->currentIndex() == 0) {
        bool ok1, ok2, ok3, ok4;
        double startX = m_lineStartXEdit->text().toDouble(&ok1);
        double startY = m_lineStartYEdit->text().toDouble(&ok2);
        double endX = m_lineEndXEdit->text().toDouble(&ok3);
        double endY = m_lineEndYEdit->text().toDouble(&ok4);
        if (!ok1 || !ok2 || !ok3 || !ok4) return;
        auto newSeg = contour::createLineSegment({startX, startY}, {endX, endY});
        contour.replaceSegment(idx, std::move(newSeg));
    } else if (seg.getType() == contour::SegmentType::Arc && m_segmentTypeCombo->currentIndex() == 1) {
        bool ok1, ok2, ok3, ok4, ok5;
        double cx = m_arcCenterXEdit->text().toDouble(&ok1);
        double cy = m_arcCenterYEdit->text().toDouble(&ok2);
        double r = m_arcRadiusEdit->text().toDouble(&ok3);
        double sa = m_arcStartAngleEdit->text().toDouble(&ok4);
        double ea = m_arcEndAngleEdit->text().toDouble(&ok5);
        if (!ok1 || !ok2 || !ok3 || !ok4 || !ok5) return;
        double saRad = sa * M_PI / 180.0;
        double eaRad = ea * M_PI / 180.0;
        bool cw = m_arcClockwiseCheck->isChecked();
        auto newSeg = contour::createArcSegment({cx, cy}, r, saRad, eaRad, cw);
        contour.replaceSegment(idx, std::move(newSeg));
    }
    m_scene->updateScene();
    updateSegmentManagementPanel();
    m_segmentSelectCombo->setCurrentIndex(idx);
}

void MainWindow::fillFormWithSegment(int index) {
    if (index < 0 || index >= (int)m_scene->getSegmentCount()) {
        m_lineStartXEdit->clear(); m_lineStartYEdit->clear(); m_lineEndXEdit->clear(); m_lineEndYEdit->clear();
        m_arcCenterXEdit->clear(); m_arcCenterYEdit->clear(); m_arcRadiusEdit->clear(); m_arcStartAngleEdit->clear(); m_arcEndAngleEdit->clear();
        m_arcClockwiseCheck->setChecked(false);
        return;
    }
    const auto& seg = m_scene->getContour()[index];
    if (seg.getType() == contour::SegmentType::Line) {
        m_segmentTypeCombo->setCurrentIndex(0);
        const auto& line = static_cast<const contour::LineSegment&>(seg);
        m_lineStartXEdit->setText(QString::number(line.getStartPoint().x));
        m_lineStartYEdit->setText(QString::number(line.getStartPoint().y));
        m_lineEndXEdit->setText(QString::number(line.getEndPoint().x));
        m_lineEndYEdit->setText(QString::number(line.getEndPoint().y));
    } else if (seg.getType() == contour::SegmentType::Arc) {
        m_segmentTypeCombo->setCurrentIndex(1);
        const auto& arc = static_cast<const contour::ArcSegment&>(seg);
        m_arcCenterXEdit->setText(QString::number(arc.getCenter().x));
        m_arcCenterYEdit->setText(QString::number(arc.getCenter().y));
        m_arcRadiusEdit->setText(QString::number(arc.getRadius()));
        m_arcStartAngleEdit->setText(QString::number(arc.getStartAngle() * 180.0 / M_PI));
        m_arcEndAngleEdit->setText(QString::number(arc.getEndAngle() * 180.0 / M_PI));
        m_arcClockwiseCheck->setChecked(arc.isClockwise());
    }
} 

void MainWindow::onAddSegmentClicked() {
    if (m_segmentTypeCombo->currentIndex() == 0) { // Line
        bool ok1, ok2, ok3, ok4;
        double x1 = m_lineStartXEdit->text().toDouble(&ok1);
        double y1 = m_lineStartYEdit->text().toDouble(&ok2);
        double x2 = m_lineEndXEdit->text().toDouble(&ok3);
        double y2 = m_lineEndYEdit->text().toDouble(&ok4);
        if (!ok1 || !ok2 || !ok3 || !ok4) return;
        if (geometry::Point2D(x1, y1).isEqual(geometry::Point2D(x2, y2), 1e-6)) return;
        m_scene->addLineSegment({x1, y1}, {x2, y2});
    } else { // Arc
        bool ok1, ok2, ok3, ok4, ok5;
        double cx = m_arcCenterXEdit->text().toDouble(&ok1);
        double cy = m_arcCenterYEdit->text().toDouble(&ok2);
        double r = m_arcRadiusEdit->text().toDouble(&ok3);
        double sa = m_arcStartAngleEdit->text().toDouble(&ok4);
        double ea = m_arcEndAngleEdit->text().toDouble(&ok5);
        if (!ok1 || !ok2 || !ok3 || !ok4 || !ok5 || r <= 0) return;
        double saRad = sa * M_PI / 180.0;
        double eaRad = ea * M_PI / 180.0;
        m_scene->addArcSegment({cx, cy}, r, saRad, eaRad, m_arcClockwiseCheck->isChecked());
    }
    m_scene->updateScene();
    updateSegmentManagementPanel();
    // Pilih segmen terakhir
    int lastIdx = (int)m_scene->getSegmentCount() - 1;
    if (lastIdx >= 0) m_segmentSelectCombo->setCurrentIndex(lastIdx);
}

void MainWindow::onInsertSegmentClicked() {
    int pos = m_insertPositionSpinBox->value();
    size_t count = m_scene->getSegmentCount();
    if (pos < 0 || pos > (int)count) return;
    if (m_segmentTypeCombo->currentIndex() == 0) { // Line
        bool ok1, ok2, ok3, ok4;
        double x1 = m_lineStartXEdit->text().toDouble(&ok1);
        double y1 = m_lineStartYEdit->text().toDouble(&ok2);
        double x2 = m_lineEndXEdit->text().toDouble(&ok3);
        double y2 = m_lineEndYEdit->text().toDouble(&ok4);
        if (!ok1 || !ok2 || !ok3 || !ok4) return;
        if (geometry::Point2D(x1, y1).isEqual(geometry::Point2D(x2, y2), 1e-6)) return;
        auto seg = contour::createLineSegment({x1, y1}, {x2, y2});
        m_scene->getContour().insertSegment(pos, std::move(seg));
    } else { // Arc
        bool ok1, ok2, ok3, ok4, ok5;
        double cx = m_arcCenterXEdit->text().toDouble(&ok1);
        double cy = m_arcCenterYEdit->text().toDouble(&ok2);
        double r = m_arcRadiusEdit->text().toDouble(&ok3);
        double sa = m_arcStartAngleEdit->text().toDouble(&ok4);
        double ea = m_arcEndAngleEdit->text().toDouble(&ok5);
        if (!ok1 || !ok2 || !ok3 || !ok4 || !ok5 || r <= 0) return;
        double saRad = sa * M_PI / 180.0;
        double eaRad = ea * M_PI / 180.0;
        auto seg = contour::createArcSegment({cx, cy}, r, saRad, eaRad, m_arcClockwiseCheck->isChecked());
        m_scene->getContour().insertSegment(pos, std::move(seg));
    }
    m_scene->updateScene();
    updateSegmentManagementPanel();
    // Pilih segmen yang baru diinsert
    m_segmentSelectCombo->setCurrentIndex(pos);
}

void MainWindow::onRemoveSegmentClicked() {
    int pos = m_insertPositionSpinBox->value();
    size_t count = m_scene->getSegmentCount();
    if (count == 0 || pos < 0 || pos >= (int)count) return;
    m_scene->getContour().removeSegment(pos);
    m_scene->updateScene();
    updateSegmentManagementPanel();
    // Pilih segmen terdekat
    int sel = pos;
    if (sel >= (int)m_scene->getSegmentCount()) sel = (int)m_scene->getSegmentCount() - 1;
    if (sel >= 0) m_segmentSelectCombo->setCurrentIndex(sel);
} 