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
    void onEditModeChanged(int index);
    void onColorButtonClicked();
    void onThicknessChanged(int value);
    void onSegmentSelected();
    void onApplySettingsClicked(); // Tambahan slot untuk tombol Apply

private:
    void setupUI();
    void setupMenuBar();
    void setupToolBar();
    void setupStatusBar();
    void setupEditPanel();
    void createActions();
    void connectSignals();
    void updateEditPanel();

    ContourScene* m_scene;
    QGraphicsView* m_view;
    QToolBar* m_toolBar;
    QStatusBar* m_statusBar;
    QWidget* m_editPanel;
    
    // Edit controls
    QComboBox* m_editModeCombo;
    QComboBox* m_segmentCombo;
    QPushButton* m_colorButton;
    QSlider* m_thicknessSlider;
    QSpinBox* m_thicknessSpinBox;
    QLabel* m_colorLabel;
    QLabel* m_thicknessLabel;
    QPushButton* m_applyButton; // Tambahan tombol Apply
    QColor m_pendingColor; // Tambahan untuk simpan warna sementara
    int m_pendingThickness; // Tambahan untuk simpan thickness sementara
    
    // Actions
    QAction* m_newAction;
    QAction* m_openAction;
    QAction* m_saveAction;
    QAction* m_clearAction;
    QAction* m_gridAction;
    QAction* m_snapAction;
    QAction* m_aboutAction;
    
    // Current selection
    size_t m_currentSegmentIndex;

    // Proteksi recursion signal/slot
    bool m_isHandlingContourModified = false;
    bool m_isUserEditingPanel = false;
}; 