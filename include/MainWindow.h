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
#include <QComboBox>
#include <QColorDialog>
#include <QSlider>
#include <QLabel>
#include <QSpinBox>
#include <QPushButton>
#include <QGroupBox>
#include <QLineEdit>
#include <QCheckBox>

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
    // Semua operasi edit hanya dari Segment Management
    void onAddSegmentClicked();
    void onInsertSegmentClicked();
    void onRemoveSegmentClicked();
    void onUpdateSegmentClicked();
    void onSegmentTypeChanged(int index);
    void onInsertPositionChanged(int value);
    void onSegmentSelectChanged(int value);

private:
    void setupUI();
    void setupMenuBar();
    void setupToolBar();
    void setupStatusBar();
    void setupSegmentManagementPanel();
    void createActions();
    void connectSignals();
    void updateSegmentManagementPanel();
    void updateValidationStatus();
    void fillFormWithSegment(int index);
    void updateButtonStates();

    ContourScene* m_scene;
    QGraphicsView* m_view;
    QToolBar* m_toolBar;
    QStatusBar* m_statusBar;
    QWidget* m_segmentManagementPanel;
    
    // Segment management controls
    QComboBox* m_segmentTypeCombo;
    QSpinBox* m_insertPositionSpinBox;
    QPushButton* m_addSegmentButton;
    QPushButton* m_insertSegmentButton;
    QPushButton* m_removeSegmentButton;
    QPushButton* m_updateSegmentButton;
    QLabel* m_validationStatusLabel;
    QLabel* m_segmentCountLabel;
    QComboBox* m_segmentSelectCombo;
    
    // Line segment inputs
    QLineEdit* m_lineStartXEdit;
    QLineEdit* m_lineStartYEdit;
    QLineEdit* m_lineEndXEdit;
    QLineEdit* m_lineEndYEdit;
    
    // Arc segment inputs
    QLineEdit* m_arcCenterXEdit;
    QLineEdit* m_arcCenterYEdit;
    QLineEdit* m_arcRadiusEdit;
    QLineEdit* m_arcStartAngleEdit;
    QLineEdit* m_arcEndAngleEdit;
    QCheckBox* m_arcClockwiseCheck;
    
    // Actions
    QAction* m_newAction;
    QAction* m_openAction;
    QAction* m_saveAction;
    QAction* m_clearAction;
    QAction* m_gridAction;
    QAction* m_snapAction;
    QAction* m_aboutAction;
}; 