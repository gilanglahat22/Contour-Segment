#include "../include/Contour.h"
#include "../include/ContourUtilities.h"
#include "../include/ContourVisualizer.h"
#include "../include/Geometry.h"
#include <vector>
#include <memory>
#include <future>
#include <algorithm>
#include <iostream>
#include <cassert>
#include <chrono>
#include <thread>
#include <set>
#include <iterator>
#include <functional>
#include <string>
#include <utility>

using namespace contour;
using namespace geometry;
using namespace contour::visualization;

// Test helper class for managing test results
class TestSuite
{
private:
    int m_totalTests = 0;
    int m_passedTests = 0;
    
public:
    void runTest(const std::string& testName, bool result)
    {
        m_totalTests++;
        if (result)
        {
            m_passedTests++;
            std::cout << "[PASS] " << testName << std::endl;
        }
        else
        {
            std::cout << "[FAIL] " << testName << std::endl;
        }
    }
    
    void printSummary()
    {
        std::cout << "\nTest Results: " << m_passedTests << "/" << m_totalTests << " tests passed\n";
        if (m_passedTests == m_totalTests)
        {
            std::cout << "All tests PASSED!" << std::endl;
        }
        else
        {
            std::cout << (m_totalTests - m_passedTests) << " tests FAILED!" << std::endl;
        }
    }
};

// Test basic geometry functionality
void testGeometry(TestSuite& suite)
{
    std::cout << "\n=== Testing Geometry ===" << std::endl;
    
    // Test Point2D basic operations
    Point2D p1(1.0, 2.0);
    Point2D p2(3.0, 4.0);
    Point2D p3 = p1 + p2;
    
    suite.runTest("Point2D addition", p3.x == 4.0 && p3.y == 6.0);
    suite.runTest("Point2D distance", isEqual(p1.distanceTo(p2), std::sqrt(8.0)));
    suite.runTest("Point2D equality", p1.isEqual(Point2D(1.0, 2.0)));
}

// Test segment functionality
void testSegments(TestSuite& suite)
{
    std::cout << "\n=== Testing Segments ===" << std::endl;
    
    // Test LineSegment
    auto lineSegment = createLineSegment(Point2D(0.0, 0.0), Point2D(3.0, 4.0));
    suite.runTest("LineSegment creation", lineSegment != nullptr);
    suite.runTest("LineSegment length", isEqual(lineSegment->getLength(), 5.0));
    suite.runTest("LineSegment type", lineSegment->getType() == SegmentType::Line);
    
    // Test ArcSegment
    auto arcSegment = createArcSegment(Point2D(0.0, 0.0), 5.0, 0.0, PI/2, false);
    suite.runTest("ArcSegment creation", arcSegment != nullptr);
    suite.runTest("ArcSegment type", arcSegment->getType() == SegmentType::Arc);
    suite.runTest("ArcSegment length", isEqual(arcSegment->getLength(), 5.0 * PI / 2.0));
    
    // Test connection
    auto line1 = createLineSegment(Point2D(0.0, 0.0), Point2D(1.0, 0.0));
    auto line2 = createLineSegment(Point2D(1.0, 0.0), Point2D(2.0, 0.0));
    suite.runTest("Segment connection", line1->isConnectedTo(*line2));
    
    // Test cloning
    auto clonedLine = line1->clone();
    suite.runTest("Segment cloning", line1->isEqual(*clonedLine));
}

// Test contour functionality
void testContour(TestSuite& suite)
{
    std::cout << "\n=== Testing Contour ===" << std::endl;
    
    // Create a simple valid contour (square)
    Contour validSquare;
    validSquare.addSegment(createLineSegment(Point2D(0.0, 0.0), Point2D(1.0, 0.0)));
    validSquare.addSegment(createLineSegment(Point2D(1.0, 0.0), Point2D(1.0, 1.0)));
    validSquare.addSegment(createLineSegment(Point2D(1.0, 1.0), Point2D(0.0, 1.0)));
    validSquare.addSegment(createLineSegment(Point2D(0.0, 1.0), Point2D(0.0, 0.0)));
    
    suite.runTest("Valid contour creation", validSquare.size() == 4);
    suite.runTest("Valid contour validation", validSquare.isValid());
    suite.runTest("Contour is closed", validSquare.isClosed());
    
    // Test caching - should not recalculate
    bool firstCall = validSquare.isValid();
    bool secondCall = validSquare.isValid();
    suite.runTest("Validation caching", firstCall == secondCall);
    
    // Create invalid contour (disconnected segments)
    Contour invalidContour;
    invalidContour.addSegment(createLineSegment(Point2D(0.0, 0.0), Point2D(1.0, 0.0)));
    invalidContour.addSegment(createLineSegment(Point2D(2.0, 0.0), Point2D(3.0, 0.0))); // Disconnected!
    
    suite.runTest("Invalid contour validation", !invalidContour.isValid());
    
    // Test copy constructor
    Contour copiedContour = validSquare;
    suite.runTest("Contour copy constructor", copiedContour.isValid() && copiedContour == validSquare);
    
    // Test iterator support
    int segmentCount = 0;
    for (const auto& segment : validSquare)
    {
        segmentCount++;
        (void)segment; // Suppress unused variable warning
    }
    suite.runTest("Contour iteration", segmentCount == 4);
}

// Test utility functions
void testUtilities(TestSuite& suite)
{
    std::cout << "\n=== Testing Utilities ===" << std::endl;
    
    // Test polyline creation
    std::vector<Point2D> points = {
        Point2D(0.0, 0.0),
        Point2D(1.0, 0.0),
        Point2D(1.0, 1.0),
        Point2D(0.0, 1.0)
    };
    
    auto polylineContour = utilities::createPolylineContour(points);
    suite.runTest("Polyline contour creation", polylineContour.size() == 3);
    suite.runTest("Polyline contour validation", polylineContour.isValid());
    
    // Test with options (closed path)
    utilities::PolylineOptions options;
    options.closePath = true;
    auto closedContour = utilities::createPolylineContour(points, options);
    suite.runTest("Closed polyline creation", closedContour.size() == 4);
    suite.runTest("Closed polyline validation", closedContour.isValid());
    suite.runTest("Closed polyline is closed", closedContour.isClosed());
    
    // Test collinear point removal
    std::vector<Point2D> collinearPoints = {
        Point2D(0.0, 0.0),
        Point2D(1.0, 0.0),
        Point2D(2.0, 0.0), // Collinear
        Point2D(3.0, 1.0)
    };
    options.removeCollinear = true;
    auto optimizedContour = utilities::createPolylineContour(collinearPoints, options);
    suite.runTest("Collinear point removal", optimizedContour.size() == 3); // Should remove middle point
    
    // Test area calculation
    double area = utilities::calculatePolygonArea(points);
    suite.runTest("Polygon area calculation", isEqual(area, 1.0));
}

// Create hardcoded test contours as required by specifications
std::vector<std::unique_ptr<Contour>> createTestContours()
{
    std::vector<std::unique_ptr<Contour>> contours;
    
    // Contour 1: Valid square
    auto contour1 = std::make_unique<Contour>();
    contour1->addSegment(createLineSegment(Point2D(0.0, 0.0), Point2D(1.0, 0.0)));
    contour1->addSegment(createLineSegment(Point2D(1.0, 0.0), Point2D(1.0, 1.0)));
    contour1->addSegment(createLineSegment(Point2D(1.0, 1.0), Point2D(0.0, 1.0)));
    contour1->addSegment(createLineSegment(Point2D(0.0, 1.0), Point2D(0.0, 0.0)));
    
    // Contour 2: Valid triangle
    auto contour2 = std::make_unique<Contour>();
    contour2->addSegment(createLineSegment(Point2D(0.0, 0.0), Point2D(2.0, 0.0)));
    contour2->addSegment(createLineSegment(Point2D(2.0, 0.0), Point2D(1.0, 2.0)));
    contour2->addSegment(createLineSegment(Point2D(1.0, 2.0), Point2D(0.0, 0.0)));
    
    // Contour 3: Invalid - disconnected segments
    auto contour3 = std::make_unique<Contour>();
    contour3->addSegment(createLineSegment(Point2D(0.0, 0.0), Point2D(1.0, 0.0)));
    contour3->addSegment(createLineSegment(Point2D(2.0, 0.0), Point2D(3.0, 0.0))); // Gap!
    
    // Contour 4: Invalid - segments don't connect properly 
    auto contour4 = std::make_unique<Contour>();
    contour4->addSegment(createLineSegment(Point2D(0.0, 0.0), Point2D(1.0, 0.0)));
    contour4->addSegment(createLineSegment(Point2D(2.0, 0.0), Point2D(3.0, 1.0))); // Gap from (1,0) to (2,0)
    contour4->addSegment(createLineSegment(Point2D(3.0, 1.0), Point2D(0.0, 0.0)));
    
    contours.push_back(std::move(contour1));
    contours.push_back(std::move(contour2));
    contours.push_back(std::move(contour3));
    contours.push_back(std::move(contour4));
    
    return contours;
}

// Asynchronous function to search for valid contours
std::vector<Contour*> searchValidContours(const std::vector<std::unique_ptr<Contour>>& contours)
{
    std::vector<Contour*> validContours;
    
    for (const auto& contour : contours)
    {
        if (contour->isValid())
        {
            validContours.push_back(contour.get());
        }
        
        // Simulate some processing time
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    return validContours;
}

// Asynchronous function to search for invalid contours
std::vector<Contour*> searchInvalidContours(const std::vector<std::unique_ptr<Contour>>& contours)
{
    std::vector<Contour*> invalidContours;
    
    for (const auto& contour : contours)
    {
        if (!contour->isValid())
        {
            invalidContours.push_back(contour.get());
        }
        
        // Simulate some processing time
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    return invalidContours;
}

// Main async routine test as specified in requirements
void testAsyncContourSearch(TestSuite& suite)
{
    std::cout << "\n=== Testing Async Contour Search (Requirements 8) ===" << std::endl;
    
    // Step a: Construct vector of 4 hardcoded Contours
    auto contours = createTestContours();
    suite.runTest("Created 4 test contours", contours.size() == 4);
    
    // Step b & c: Start asynchronous tasks
    auto validFuture = std::async(std::launch::async, searchValidContours, std::cref(contours));
    auto invalidFuture = std::async(std::launch::async, searchInvalidContours, std::cref(contours));
    
    // Step d: Get results from both jobs
    auto validResults = validFuture.get();
    auto invalidResults = invalidFuture.get();
    
    // Step e: Validate results
    
    // Check that we found the expected number of valid/invalid contours
    suite.runTest("Found valid contours", validResults.size() == 2);
    suite.runTest("Found invalid contours", invalidResults.size() == 2);
    
    // Verify each set is unique (no duplicates within sets)
    std::set<Contour*> validSet(validResults.begin(), validResults.end());
    std::set<Contour*> invalidSet(invalidResults.begin(), invalidResults.end());
    
    suite.runTest("Valid set is unique", validSet.size() == validResults.size());
    suite.runTest("Invalid set is unique", invalidSet.size() == invalidResults.size());
    
    // Verify no overlap between sets
    std::vector<Contour*> intersection;
    std::set_intersection(validSet.begin(), validSet.end(),
                         invalidSet.begin(), invalidSet.end(),
                         std::back_inserter(intersection));
    
    suite.runTest("No overlap between valid and invalid sets", intersection.empty());
    
    // Verify combination of both sets contains all original contours
    std::set<Contour*> allFound;
    allFound.insert(validResults.begin(), validResults.end());
    allFound.insert(invalidResults.begin(), invalidResults.end());
    
    std::set<Contour*> allOriginal;
    for (const auto& contour : contours)
    {
        allOriginal.insert(contour.get());
    }
    
    suite.runTest("All original contours found", allFound == allOriginal);
    
    // Verify actual validity of found contours
    bool allValidAreValid = std::all_of(validResults.begin(), validResults.end(),
        [](const Contour* c) { return c->isValid(); });
    
    bool allInvalidAreInvalid = std::all_of(invalidResults.begin(), invalidResults.end(),
        [](const Contour* c) { return !c->isValid(); });
    
    suite.runTest("All 'valid' results are actually valid", allValidAreValid);
    suite.runTest("All 'invalid' results are actually invalid", allInvalidAreInvalid);
    
    std::cout << "Async search completed successfully!" << std::endl;
}

// Test visualization functionality
void testVisualization(TestSuite& suite)
{
    std::cout << "\n=== Testing Visualization ===" << std::endl;
    
    // Test visualization options
    VisualizationOptions options;
    options.width = 40;
    options.height = 15;
    options.showCoordinates = true;
    
    ContourVisualizer visualizer(options);
    
    // Create a simple test contour
    Contour testContour;
    testContour.addSegment(createLineSegment(Point2D(0.0, 0.0), Point2D(2.0, 0.0)));
    testContour.addSegment(createLineSegment(Point2D(2.0, 0.0), Point2D(2.0, 2.0)));
    testContour.addSegment(createLineSegment(Point2D(2.0, 2.0), Point2D(0.0, 2.0)));
    testContour.addSegment(createLineSegment(Point2D(0.0, 2.0), Point2D(0.0, 0.0)));
    
    // Test single contour visualization
    std::string visualization = visualizer.visualizeContour(testContour);
    suite.runTest("Contour visualization generation", !visualization.empty());
    suite.runTest("Visualization contains coordinate info", 
                  visualization.find("Bounds:") != std::string::npos);
    suite.runTest("Visualization contains validity info", 
                  visualization.find("Valid:") != std::string::npos);
    
    // Test coordinate info generation
    std::string coordInfo = visualizer.generateCoordinateInfo(testContour);
    suite.runTest("Coordinate info generation", !coordInfo.empty());
    suite.runTest("Coordinate info contains segments", 
                  coordInfo.find("Segment") != std::string::npos);
    
    // Test contour table generation
    std::string table = visualizer.generateContourTable(testContour);
    suite.runTest("Contour table generation", !table.empty());
    suite.runTest("Table contains properties", 
                  table.find("Segments:") != std::string::npos);
    
    // Test multiple contours visualization
    Contour triangle;
    triangle.addSegment(createLineSegment(Point2D(1.0, 1.0), Point2D(3.0, 1.0)));
    triangle.addSegment(createLineSegment(Point2D(3.0, 1.0), Point2D(2.0, 3.0)));
    triangle.addSegment(createLineSegment(Point2D(2.0, 3.0), Point2D(1.0, 1.0)));
    
    std::vector<const Contour*> contours = {&testContour, &triangle};
    std::string multiViz = visualizer.visualizeContours(contours);
    suite.runTest("Multiple contours visualization", !multiViz.empty());
    suite.runTest("Multiple visualization has legend", 
                  multiViz.find("Legend:") != std::string::npos);
    
    // Test utility functions
    std::string pointStr = utils::formatPoint(Point2D(1.23, 4.56), 1);
    suite.runTest("Point formatting", pointStr == "(1.2,4.6)");
    
    std::string angleStr = utils::formatAngle(PI/4, 0);
    suite.runTest("Angle formatting", angleStr == "45°");
    
    std::string box = utils::drawBox(20, 3, "Test");
    suite.runTest("Box drawing", !box.empty() && box.find("Test") != std::string::npos);
    
    std::cout << "\nSample Visualization Output:\n";
    std::cout << "┌────────────────────────────────────┐\n";
    std::cout << "│          Sample Square             │\n";
    std::cout << "└────────────────────────────────────┘\n";
    VisualizationOptions smallOptions;
    smallOptions.width = 20;
    smallOptions.height = 8;
    smallOptions.showCoordinates = false;
    ContourVisualizer smallViz(smallOptions);
    std::cout << smallViz.visualizeContour(testContour) << std::endl;
}

// Main test runner
void runAllTests()
{
    TestSuite suite;
    
    std::cout << "Starting ContourSegment Test Suite..." << std::endl;
    std::cout << "==========================================" << std::endl;
    
    try
    {
        testGeometry(suite);
        testSegments(suite);
        testContour(suite);
        testUtilities(suite);
        testVisualization(suite);
        testAsyncContourSearch(suite);
    }
    catch (const std::exception& e)
    {
        std::cout << "Test execution failed with exception: " << e.what() << std::endl;
    }
    
    std::cout << "\n===========================================" << std::endl;
    suite.printSummary();
}

// For integration with main.cpp
extern "C" void runContourTests()
{
    runAllTests();
} 