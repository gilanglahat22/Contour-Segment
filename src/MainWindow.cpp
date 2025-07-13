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
#include <iostream>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_scene(nullptr)
    , m_view(nullptr)
    , m_toolBar(nullptr)
    , m_statusBar(nullptr)
    , m_editPanel(nullptr)
    , m_editModeCombo(nullptr)
    , m_segmentCombo(nullptr)
    , m_colorButton(nullptr)
    , m_thicknessSlider(nullptr)
    , m_thicknessSpinBox(nullptr)
    , m_colorLabel(nullptr)
    , m_thicknessLabel(nullptr)
    , m_newAction(nullptr)
    , m_openAction(nullptr)
    , m_saveAction(nullptr)
    , m_clearAction(nullptr)
    , m_gridAction(nullptr)
    , m_snapAction(nullptr)
    , m_aboutAction(nullptr)
    , m_currentSegmentIndex(0)
    , m_isHandlingContourModified(false)
    , m_isUserEditingPanel(false)
{
    setupUI();
    createActions();
    setupMenuBar();
    setupToolBar();
    setupStatusBar();
    setupEditPanel();
    std::cout << "[TRACE] m_editModeCombo address (after setupEditPanel): " << m_editModeCombo << std::endl;
    
    // Verify QComboBox is properly created before connecting signals
    if (m_editModeCombo) {
        std::cout << "[TRACE] QComboBox is valid, proceeding with signal connection..." << std::endl;
        std::cout << "[TRACE] QComboBox current index: " << m_editModeCombo->currentIndex() << std::endl;
        std::cout << "[TRACE] QComboBox item count: " << m_editModeCombo->count() << std::endl;
    } else {
        std::cout << "[ERROR] QComboBox is null after setupEditPanel!" << std::endl;
    }
    
    connectSignals();
    std::cout << "[TRACE] m_editModeCombo address (after connectSignals): " << m_editModeCombo << std::endl;
    
    setWindowTitle("ContourSegment - Contour Editor with Vertex Editing");
    resize(1000, 700);

    // Tambahkan dark style agar menu, panel, dan konten kontras
    QString darkStyle = R"(
        QMainWindow, QWidget, QMenuBar, QMenu, QToolBar, QStatusBar {
            background-color: #232629;
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
        QLabel, QComboBox, QSpinBox, QSlider, QPushButton {
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
    
    // Create right panel for edit controls
    setupEditPanel();
    
    // Add panels to main layout
    mainLayout->addLayout(leftLayout, 1); // Scene takes most space
    mainLayout->addWidget(m_editPanel, 0); // Edit panel takes minimum space
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

void MainWindow::setupEditPanel()
{
    // Create edit panel
    m_editPanel = new QWidget(this);
    m_editPanel->setMinimumWidth(200);
    m_editPanel->setMaximumWidth(200);
    // Style khusus panel kanan agar background gelap dan font putih
    m_editPanel->setStyleSheet(R"(
        QWidget {
            background-color: #232629;
            color: #f0f0f0;
        }
        QLabel, QComboBox, QSpinBox, QSlider, QPushButton {
            background-color: #232629;
            color: #f0f0f0;
            border: 1px solid #444;
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
    )");

    // Create layout for edit panel
    QVBoxLayout* editPanelLayout = new QVBoxLayout(m_editPanel);
    editPanelLayout->setAlignment(Qt::AlignTop);
    editPanelLayout->setContentsMargins(5, 5, 5, 5);

    // Edit Mode Combo Box - FIXED: Ensure proper creation and setup
    m_editModeCombo = new QComboBox(m_editPanel);
    std::cout << "[TRACE] setupEditPanel: Created m_editModeCombo at address: " << m_editModeCombo << std::endl;
    
    // Add items with clear labels
    m_editModeCombo->addItem("Select");
    m_editModeCombo->addItem("Draw Line");
    m_editModeCombo->addItem("Draw Arc");
    m_editModeCombo->addItem("Edit Vertex");
    
    // Set initial index
    m_editModeCombo->setCurrentIndex(0);
    
    // Make sure it's visible and enabled
    m_editModeCombo->setVisible(true);
    m_editModeCombo->setEnabled(true);
    
    // Add to layout
    editPanelLayout->addWidget(m_editModeCombo);
    
    // Test if we can access the combo box
    std::cout << "[TRACE] setupEditPanel: m_editModeCombo current index: " << m_editModeCombo->currentIndex() << std::endl;
    std::cout << "[TRACE] setupEditPanel: m_editModeCombo item count: " << m_editModeCombo->count() << std::endl;

    // Segment Combo Box
    m_segmentCombo = new QComboBox(m_editPanel);
    editPanelLayout->addWidget(m_segmentCombo);

    // Color Button
    m_colorButton = new QPushButton("Color", m_editPanel);
    m_colorButton->setStyleSheet("QPushButton { background-color: #e0e0e0; }");
    editPanelLayout->addWidget(m_colorButton);

    // Thickness Slider
    m_thicknessSlider = new QSlider(Qt::Horizontal, m_editPanel);
    m_thicknessSlider->setMinimum(1);
    m_thicknessSlider->setMaximum(10);
    m_thicknessSlider->setValue(5);
    editPanelLayout->addWidget(m_thicknessSlider);

    // Thickness Spin Box
    m_thicknessSpinBox = new QSpinBox(m_editPanel);
    m_thicknessSpinBox->setMinimum(1);
    m_thicknessSpinBox->setMaximum(10);
    m_thicknessSpinBox->setValue(5);
    editPanelLayout->addWidget(m_thicknessSpinBox);

    // Color Label
    m_colorLabel = new QLabel("Color:", m_editPanel);
    editPanelLayout->addWidget(m_colorLabel);

    // Thickness Label
    m_thicknessLabel = new QLabel("Thickness:", m_editPanel);
    editPanelLayout->addWidget(m_thicknessLabel);

    // Tambah tombol Apply/Submit
    m_applyButton = new QPushButton("Apply", m_editPanel);
    m_applyButton->setStyleSheet("QPushButton { background-color: #2d8cff; color: #fff; border-radius: 4px; padding: 4px 8px; } QPushButton:disabled { background-color: #333; color: #888; }");
    editPanelLayout->addWidget(m_applyButton);

    // Add edit panel to the main layout
    // layout->addWidget(m_editPanel); // This line is removed as per the new_code
    m_pendingColor = QColor(Qt::white);
    m_pendingThickness = 1;
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
    
    // Connect edit controls - FIXED: Use direct signal connection
    if (m_editModeCombo) {
        bool ok = connect(m_editModeCombo, SIGNAL(currentIndexChanged(int)), 
                         this, SLOT(onEditModeChanged(int)));
        std::cout << "[TRACE] connect m_editModeCombo to onEditModeChanged: " << (ok ? "OK" : "FAIL") << " address: " << m_editModeCombo << std::endl;
        
        // Also connect to activated signal as backup
        bool ok2 = connect(m_editModeCombo, SIGNAL(activated(int)), 
                          this, SLOT(onEditModeChanged(int)));
        std::cout << "[TRACE] connect m_editModeCombo activated signal: " << (ok2 ? "OK" : "FAIL") << std::endl;
        
        // Test the connection by manually calling the slot
        std::cout << "[TRACE] Testing slot connection..." << std::endl;
        onEditModeChanged(m_editModeCombo->currentIndex());
    } else {
        std::cout << "[ERROR] m_editModeCombo is null in connectSignals!" << std::endl;
    }
    
    connect(m_segmentCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onSegmentSelected);
    connect(m_colorButton, &QPushButton::clicked, this, &MainWindow::onColorButtonClicked);
    connect(m_thicknessSlider, &QSlider::valueChanged, this, &MainWindow::onThicknessChanged);
    connect(m_thicknessSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &MainWindow::onThicknessChanged);
    connect(m_applyButton, &QPushButton::clicked, this, &MainWindow::onApplySettingsClicked);
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
    if (m_isHandlingContourModified) return;
    m_isHandlingContourModified = true;

    QString status = QString("Segments: %1 | Valid: %2 | Closed: %3")
        .arg(m_scene->getSegmentCount())
        .arg(m_scene->isValid() ? "Yes" : "No")
        .arg(m_scene->isClosed() ? "Yes" : "No");
    
    m_statusBar->showMessage(status);
    updateEditPanel();

    m_isHandlingContourModified = false;
}

void MainWindow::onEditModeChanged(int index)
{
    std::cout << "[TRACE] onEditModeChanged called! this=" << this << " m_editModeCombo=" << m_editModeCombo << " index=" << index << std::endl;
    
    // Validate scene exists
    if (!m_scene) {
        std::cout << "[ERROR] m_scene is null in onEditModeChanged!" << std::endl;
        return;
    }
    
    // Validate index
    if (index < 0 || index > 3) {
        std::cout << "[ERROR] Invalid index in onEditModeChanged: " << index << std::endl;
        return;
    }
    
    switch (index) {
        case 0: // Select
            std::cout << "[TRACE] Mode: Select (Draw/Line)" << std::endl;
            m_scene->setEditMode(EditMode::Draw);
            m_scene->setDrawingMode(DrawingMode::Line);
            m_statusBar->showMessage("Select mode - click to select segments", 2000);
            break;
        case 1: // Draw Line
            std::cout << "[TRACE] Mode: Draw Line" << std::endl;
            m_scene->setEditMode(EditMode::Draw);
            m_scene->setDrawingMode(DrawingMode::Line);
            m_statusBar->showMessage("Draw mode - click and drag to draw lines", 2000);
            break;
        case 2: // Draw Arc
            std::cout << "[TRACE] Mode: Draw Arc" << std::endl;
            m_scene->setEditMode(EditMode::Draw);
            m_scene->setDrawingMode(DrawingMode::Arc);
            m_statusBar->showMessage("Arc mode - click center, lalu start, lalu drag ke end", 2000);
            break;
        case 3: // Edit Vertex
            std::cout << "[TRACE] Mode: Edit Vertex" << std::endl;
            m_scene->setEditMode(EditMode::Edit);
            m_statusBar->showMessage("Edit mode - drag vertex markers to edit", 2000);
            break;
        default:
            std::cout << "[ERROR] Unknown mode index: " << index << std::endl;
            break;
    }
    
    // Mode change completed - scene will update automatically when needed
    std::cout << "[TRACE] onEditModeChanged completed for index: " << index << std::endl;
}

void MainWindow::onColorButtonClicked()
{
    QColor currentColor = m_pendingColor;
    QColor newColor = QColorDialog::getColor(currentColor, this, "Select Color");
    if (newColor.isValid()) {
        m_pendingColor = newColor;
        m_colorButton->setStyleSheet(QString("QPushButton { background-color: %1; } QPushButton:disabled { background-color: #333; color: #888; } ").arg(newColor.name()));
        m_statusBar->showMessage(QString("Selected color %1").arg(newColor.name()), 2000);
    }
}

void MainWindow::onThicknessChanged(int value)
{
    m_pendingThickness = value;
    m_thicknessSlider->blockSignals(true);
    m_thicknessSlider->setValue(value);
    m_thicknessSlider->blockSignals(false);
    m_thicknessSpinBox->blockSignals(true);
    m_thicknessSpinBox->setValue(value);
    m_thicknessSpinBox->blockSignals(false);
    m_statusBar->showMessage(QString("Selected thickness %1").arg(value), 2000);
}

void MainWindow::updateEditPanel()
{
    static bool isUpdatingPanel = false;
    if (isUpdatingPanel) return;
    isUpdatingPanel = true;
    m_isUserEditingPanel = false;

    QSignalBlocker blocker(m_segmentCombo);
    m_segmentCombo->clear();
    size_t segmentCount = m_scene->getSegmentCount();
    for (size_t i = 0; i < segmentCount; ++i) {
        m_segmentCombo->addItem(QString("Segment %1").arg(i + 1));
    }
    int modeIndex = m_editModeCombo->currentIndex();
    bool alwaysEnable = (modeIndex == 2 || modeIndex == 3); // Draw Arc, Edit Vertex

    // FIX: Jangan disable kontrol apapun pada mode Draw Arc/Edit Vertex
    if (alwaysEnable) {
        m_colorButton->setEnabled(true);
        m_thicknessSlider->setEnabled(true);
        m_thicknessSpinBox->setEnabled(true);
        m_applyButton->setEnabled(true);
    } else {
        bool enableControls = (segmentCount > 0);
        m_colorButton->setEnabled(enableControls);
        m_thicknessSlider->setEnabled(enableControls);
        m_thicknessSpinBox->setEnabled(enableControls);
        m_applyButton->setEnabled(enableControls);
    }

    if (segmentCount > 0) {
        m_segmentCombo->setCurrentIndex(static_cast<int>(m_currentSegmentIndex));
        if (!m_isUserEditingPanel) {
            QColor currentColor = m_scene->getSegmentColor(m_currentSegmentIndex);
            m_pendingColor = currentColor;
            m_colorButton->setStyleSheet(QString("QPushButton { background-color: %1; } QPushButton:disabled { background-color: #333; color: #888; } ").arg(currentColor.name()));
            double thickness = m_scene->getSegmentThickness(m_currentSegmentIndex);
            m_pendingThickness = static_cast<int>(thickness);
            m_thicknessSlider->setValue(m_pendingThickness);
            m_thicknessSpinBox->setValue(m_pendingThickness);
        }
    } else {
        m_currentSegmentIndex = 0;
    }
    m_isUserEditingPanel = true;
    isUpdatingPanel = false;
}

void MainWindow::onSegmentSelected()
{
    if (m_isHandlingContourModified) {
        std::cout << "[TRACE] onSegmentSelected: Skipping due to contour modification in progress" << std::endl;
        return;
    }
    std::cout << "[TRACE] onSegmentSelected: Segment index changed to " << m_segmentCombo->currentIndex() << std::endl;
    int index = m_segmentCombo->currentIndex();
    if (index >= 0 && index < static_cast<int>(m_scene->getSegmentCount())) {
        m_currentSegmentIndex = static_cast<size_t>(index);
        // Update pending value dari segmen
        QColor currentColor = m_scene->getSegmentColor(m_currentSegmentIndex);
        m_pendingColor = currentColor;
        m_colorButton->setStyleSheet(QString("QPushButton { background-color: %1; } QPushButton:disabled { background-color: #333; color: #888; } ").arg(currentColor.name()));
        double thickness = m_scene->getSegmentThickness(m_currentSegmentIndex);
        m_pendingThickness = static_cast<int>(thickness);
        m_thicknessSlider->setValue(m_pendingThickness);
        m_thicknessSpinBox->setValue(m_pendingThickness);
    }
}

void MainWindow::onApplySettingsClicked()
{
    if (m_scene->getSegmentCount() == 0) {
        m_statusBar->showMessage("No segment to apply settings.", 2000);
        return;
    }
    if (m_currentSegmentIndex >= m_scene->getSegmentCount()) {
        m_statusBar->showMessage("No segment selected to apply settings.", 2000);
        return;
    }
    m_scene->setSegmentColor(m_currentSegmentIndex, m_pendingColor);
    m_scene->setSegmentThickness(m_currentSegmentIndex, m_pendingThickness);
    m_statusBar->showMessage(QString("Applied color %1 and thickness %2 to segment %3").arg(m_pendingColor.name()).arg(m_pendingThickness).arg(m_currentSegmentIndex), 2000);
    // Setelah apply, update panel dari segmen (reset flag)
    m_isUserEditingPanel = false;
    updateEditPanel();
} 