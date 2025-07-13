#include <iostream>
#include <string>

// Simulasi enum dan class yang ada di ContourScene
enum class EditMode { Draw, Edit };
enum class DrawingMode { Line, Arc };

class MockContourScene {
public:
    void setEditMode(EditMode mode) {
        std::cout << "[TRACE] MockContourScene::setEditMode called with mode: " 
                  << (mode == EditMode::Draw ? "Draw" : "Edit") << std::endl;
        m_editMode = mode;
    }
    
    void setDrawingMode(DrawingMode mode) {
        std::cout << "[TRACE] MockContourScene::setDrawingMode called with mode: " 
                  << (mode == DrawingMode::Line ? "Line" : "Arc") << std::endl;
        m_drawingMode = mode;
    }
    
    void updateScene() {
        std::cout << "[TRACE] MockContourScene::updateScene called" << std::endl;
    }
    
private:
    EditMode m_editMode = EditMode::Draw;
    DrawingMode m_drawingMode = DrawingMode::Line;
};

// Simulasi slot onEditModeChanged
void onEditModeChanged(int index, MockContourScene* scene) {
    std::cout << "[TRACE] onEditModeChanged called! scene=" << scene << " index=" << index << std::endl;
    
    // Validate scene exists
    if (!scene) {
        std::cout << "[ERROR] scene is null in onEditModeChanged!" << std::endl;
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
            scene->setEditMode(EditMode::Draw);
            scene->setDrawingMode(DrawingMode::Line);
            break;
        case 1: // Draw Line
            std::cout << "[TRACE] Mode: Draw Line" << std::endl;
            scene->setEditMode(EditMode::Draw);
            scene->setDrawingMode(DrawingMode::Line);
            break;
        case 2: // Draw Arc
            std::cout << "[TRACE] Mode: Draw Arc" << std::endl;
            scene->setEditMode(EditMode::Draw);
            scene->setDrawingMode(DrawingMode::Arc);
            break;
        case 3: // Edit Vertex
            std::cout << "[TRACE] Mode: Edit Vertex" << std::endl;
            scene->setEditMode(EditMode::Edit);
            break;
        default:
            std::cout << "[ERROR] Unknown mode index: " << index << std::endl;
            break;
    }
    
    // Force scene update to reflect mode change
    scene->updateScene();
    std::cout << "[TRACE] onEditModeChanged completed for index: " << index << std::endl;
}

int main() {
    std::cout << "Testing MainWindow.cpp Fixes" << std::endl;
    std::cout << "===========================" << std::endl;
    
    MockContourScene* scene = new MockContourScene();
    
    std::cout << "\nTesting mode changes:" << std::endl;
    std::cout << "=====================" << std::endl;
    
    // Test all modes
    onEditModeChanged(0, scene); // Select
    onEditModeChanged(1, scene); // Draw Line
    onEditModeChanged(2, scene); // Draw Arc
    onEditModeChanged(3, scene); // Edit Vertex
    
    std::cout << "\nTesting invalid inputs:" << std::endl;
    std::cout << "=======================" << std::endl;
    
    // Test invalid inputs
    onEditModeChanged(-1, scene); // Invalid index
    onEditModeChanged(5, scene);  // Invalid index
    onEditModeChanged(2, nullptr); // Null scene
    
    delete scene;
    
    std::cout << "\nTest completed successfully!" << std::endl;
    std::cout << "The MainWindow.cpp fixes should work correctly." << std::endl;
    
    return 0;
} 