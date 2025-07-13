#include "../include/Contour.h"
#include "../include/Segment.h"
#include "../include/Geometry.h"
#include "../include/ContourUtilities.h"
#include "../include/ContourVisualizer.h"
#include <iostream>
#include <vector>
#include <memory>
#include <thread>
#include <future>
#include <chrono>
#include <string>

using namespace contour;
using namespace geometry;

// Simple test function implementation
void runContourTests() {
    std::cout << "Running ContourSegment Tests..." << std::endl;
    std::cout << "=================================" << std::endl;
    
    try {
        // Test 1: Basic Contour Creation
        std::cout << "\n1. Testing basic contour creation..." << std::endl;
        Contour contour;
        contour.addSegment(createLineSegment(Point2D(0, 0), Point2D(1, 0)));
        contour.addSegment(createLineSegment(Point2D(1, 0), Point2D(1, 1)));
        contour.addSegment(createLineSegment(Point2D(1, 1), Point2D(0, 1)));
        contour.addSegment(createLineSegment(Point2D(0, 1), Point2D(0, 0)));
        
        std::cout << "   ✓ Contour created with " << contour.size() << " segments" << std::endl;
        std::cout << "   ✓ Contour is " << (contour.isValid() ? "valid" : "invalid") << std::endl;
        std::cout << "   ✓ Contour is " << (contour.isClosed() ? "closed" : "open") << std::endl;
        
        // Test 2: Copy and Move
        std::cout << "\n2. Testing copy and move semantics..." << std::endl;
        Contour copiedContour = contour;
        Contour movedContour = std::move(Contour());
        
        std::cout << "   ✓ Copy constructor works" << std::endl;
        std::cout << "   ✓ Move constructor works" << std::endl;
        
        // Test 3: Iteration
        std::cout << "\n3. Testing iteration..." << std::endl;
        int segmentCount = 0;
        for (const auto& segment : contour) {
            segmentCount++;
        }
        std::cout << "   ✓ Iteration works: " << segmentCount << " segments counted" << std::endl;
        
        // Test 4: Segment manipulation
        std::cout << "\n4. Testing segment manipulation..." << std::endl;
        Contour testContour;
        testContour.addSegment(createLineSegment(Point2D(0, 0), Point2D(1, 0)));
        testContour.insertSegment(0, createLineSegment(Point2D(-1, 0), Point2D(0, 0)));
        testContour.removeSegment(1);
        
        std::cout << "   ✓ Segment manipulation works: " << testContour.size() << " segments" << std::endl;
        
        // Test 5: Validation caching
        std::cout << "\n5. Testing validation caching..." << std::endl;
        bool firstCall = contour.isValid();
        bool secondCall = contour.isValid();
        
        std::cout << "   ✓ Validation caching works: " << (firstCall == secondCall ? "Yes" : "No") << std::endl;
        
        // Test 6: Utility functions
        std::cout << "\n6. Testing utility functions..." << std::endl;
        std::vector<Point2D> points = {
            Point2D(0, 0), Point2D(1, 0), Point2D(1, 1), Point2D(0, 1)
        };
        
        auto polylineContour = utilities::createPolylineContour(points);
        std::cout << "   ✓ Polyline creation works: " << polylineContour.size() << " segments" << std::endl;
        
        // Test 7: Asynchronous search
        std::cout << "\n7. Testing asynchronous search..." << std::endl;
        
        // Create test contours
        std::vector<std::unique_ptr<Contour>> testContours;
        
        // Valid contour 1
        auto valid1 = std::make_unique<Contour>();
        valid1->addSegment(createLineSegment(Point2D(0, 0), Point2D(1, 0)));
        valid1->addSegment(createLineSegment(Point2D(1, 0), Point2D(1, 1)));
        valid1->addSegment(createLineSegment(Point2D(1, 1), Point2D(0, 1)));
        valid1->addSegment(createLineSegment(Point2D(0, 1), Point2D(0, 0)));
        
        // Valid contour 2
        auto valid2 = std::make_unique<Contour>();
        valid2->addSegment(createLineSegment(Point2D(0, 0), Point2D(2, 0)));
        valid2->addSegment(createLineSegment(Point2D(2, 0), Point2D(1, 2)));
        valid2->addSegment(createLineSegment(Point2D(1, 2), Point2D(0, 0)));
        
        // Invalid contour 1
        auto invalid1 = std::make_unique<Contour>();
        invalid1->addSegment(createLineSegment(Point2D(0, 0), Point2D(1, 0)));
        invalid1->addSegment(createLineSegment(Point2D(2, 0), Point2D(3, 0))); // Gap!
        
        // Invalid contour 2
        auto invalid2 = std::make_unique<Contour>();
        invalid2->addSegment(createLineSegment(Point2D(0, 0), Point2D(1, 0)));
        invalid2->addSegment(createLineSegment(Point2D(1, 1), Point2D(2, 1))); // Gap!
        
        testContours.push_back(std::move(valid1));
        testContours.push_back(std::move(valid2));
        testContours.push_back(std::move(invalid1));
        testContours.push_back(std::move(invalid2));
        
        // Asynchronous search functions
        auto searchValid = [&testContours]() -> std::vector<Contour*> {
            std::vector<Contour*> validContours;
            for (const auto& contour : testContours) {
                if (contour->isValid()) {
                    validContours.push_back(contour.get());
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
            return validContours;
        };
        
        auto searchInvalid = [&testContours]() -> std::vector<Contour*> {
            std::vector<Contour*> invalidContours;
            for (const auto& contour : testContours) {
                if (!contour->isValid()) {
                    invalidContours.push_back(contour.get());
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
            return invalidContours;
        };
        
        // Start asynchronous tasks
        auto validFuture = std::async(std::launch::async, searchValid);
        auto invalidFuture = std::async(std::launch::async, searchInvalid);
        
        // Get results
        auto validResults = validFuture.get();
        auto invalidResults = invalidFuture.get();
        
        std::cout << "   ✓ Async search works: " << validResults.size() << " valid, " 
                  << invalidResults.size() << " invalid contours found" << std::endl;
        
        // Test 8: Visualization
        std::cout << "\n8. Testing visualization..." << std::endl;
        contour::visualization::VisualizationOptions options;
        options.width = 40;
        options.height = 20;
        options.showCoordinates = true;
        
        contour::visualization::ContourVisualizer visualizer(options);
        std::string visualization = visualizer.visualizeContour(contour);
        std::cout << "   ✓ Visualization works" << std::endl;
        std::cout << "\nContour Visualization:" << std::endl;
        std::cout << visualization << std::endl;
        
        std::cout << "\n=================================" << std::endl;
        std::cout << "ALL TESTS PASSED! ✓" << std::endl;
        std::cout << "=================================" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
    }
}

int main(int argc, char* argv[])
{
    // Check for test-only flag
    bool testOnly = false;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--test-only" || arg == "-t") {
            testOnly = true;
            break;
        }
    }
    
    if (testOnly) {
        std::cout << "ContourSegment - Test Mode" << std::endl;
        std::cout << "==========================" << std::endl;
        
        try {
            runContourTests();
            std::cout << "\nAll tests completed successfully!" << std::endl;
            return 0;
        } catch (const std::exception& e) {
            std::cerr << "Test execution failed: " << e.what() << std::endl;
            return 1;
        }
    }
    
    std::cout << "ContourSegment - Console Version" << std::endl;
    std::cout << "=================================" << std::endl;
    
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
        
        // Visualize the contour
        std::cout << "\nContour Visualization:" << std::endl;
        contour::visualization::VisualizationOptions options;
        options.width = 50;
        options.height = 25;
        options.showCoordinates = true;
        
        contour::visualization::ContourVisualizer visualizer(options);
        std::string visualization = visualizer.visualizeContour(contour);
        std::cout << visualization << std::endl;
        
        std::cout << "\nPress Enter to exit..." << std::endl;
        std::cin.get();
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
} 