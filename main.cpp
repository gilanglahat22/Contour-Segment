#include "include/Contour.h"
#include "include/ContourUtilities.h"
#include "include/ContourVisualizer.h"
#include "include/Geometry.h"
#include "include/MainWindow.h" // Uncommented Qt header
#include <QApplication>
#include <QMainWindow>
#include <iostream>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv); // Enabled Qt application
    
    std::cout << "ContourSegment - GUI Version" << std::endl;
    std::cout << "================================" << std::endl;
    
    try {
        // Create a simple contour for demonstration
        contour::Contour contour;
        
        // Add some line segments to create a simple shape
        contour.addSegment(contour::createLineSegment(geometry::Point2D(0, 0), geometry::Point2D(10, 0)));
        contour.addSegment(contour::createLineSegment(geometry::Point2D(10, 0), geometry::Point2D(10, 10)));
        contour.addSegment(contour::createLineSegment(geometry::Point2D(10, 10), geometry::Point2D(0, 10)));
        contour.addSegment(contour::createLineSegment(geometry::Point2D(0, 10), geometry::Point2D(0, 0)));
        
        std::cout << "Created contour with " << contour.size() << " segments" << std::endl;
        std::cout << "Contour is " << (contour.isValid() ? "valid" : "invalid") << std::endl;
        std::cout << "Contour is " << (contour.isClosed() ? "closed" : "open") << std::endl;
        
        // Calculate contour properties
        double perimeter = contour.getTotalLength();
        auto vertices = contour.getVertices();
        double area = contour::utilities::calculatePolygonArea(vertices);
        
        std::cout << "Perimeter: " << perimeter << std::endl;
        std::cout << "Area: " << area << std::endl;
        
        // Create and show the main window
        MainWindow window;
        window.show();
        
        return app.exec(); // Enable Qt event loop
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
} 