#include "include/Contour.h"
#include "include/ContourUtilities.h"
#include "include/Geometry.h"
#include <iostream>
#include <vector>
#include <memory>
#include <future>
#include <algorithm>
#include <set>
#include <chrono>
#include <thread>

using namespace contour;
using namespace geometry;

// Test function untuk memverifikasi semua persyaratan
void testAllRequirements() {
    std::cout << "Testing ContourSegment Requirements" << std::endl;
    std::cout << "===================================" << std::endl;
    
    bool allTestsPassed = true;
    
    // Test 1: Membuat Contour dengan serangkaian segment
    std::cout << "\n1. Testing Contour creation with segments..." << std::endl;
    Contour contour;
    contour.addSegment(createLineSegment(Point2D(0, 0), Point2D(1, 0)));
    contour.addSegment(createLineSegment(Point2D(1, 0), Point2D(1, 1)));
    contour.addSegment(createLineSegment(Point2D(1, 1), Point2D(0, 1)));
    contour.addSegment(createLineSegment(Point2D(0, 1), Point2D(0, 0)));
    
    if (contour.size() == 4) {
        std::cout << "   ✓ Contour created with 4 segments" << std::endl;
    } else {
        std::cout << "   ✗ Contour creation failed" << std::endl;
        allTestsPassed = false;
    }
    
    // Test 2: Copyable dan moveable
    std::cout << "\n2. Testing copy and move semantics..." << std::endl;
    Contour copiedContour = contour; // Copy constructor
    Contour movedContour = std::move(Contour()); // Move constructor
    
    if (copiedContour.size() == contour.size() && copiedContour.isValid()) {
        std::cout << "   ✓ Copy constructor works" << std::endl;
    } else {
        std::cout << "   ✗ Copy constructor failed" << std::endl;
        allTestsPassed = false;
    }
    
    // Test 3: Iterasi over segments
    std::cout << "\n3. Testing iteration over segments..." << std::endl;
    int segmentCount = 0;
    for (const auto& segment : contour) {
        segmentCount++;
        (void)segment; // Suppress warning
    }
    
    if (segmentCount == 4) {
        std::cout << "   ✓ Iteration works correctly" << std::endl;
    } else {
        std::cout << "   ✗ Iteration failed" << std::endl;
        allTestsPassed = false;
    }
    
    // Test 4: Add, insert, remove segments
    std::cout << "\n4. Testing segment manipulation..." << std::endl;
    Contour testContour;
    testContour.addSegment(createLineSegment(Point2D(0, 0), Point2D(1, 0)));
    testContour.insertSegment(0, createLineSegment(Point2D(-1, 0), Point2D(0, 0)));
    testContour.removeSegment(1);
    
    if (testContour.size() == 1) {
        std::cout << "   ✓ Segment manipulation works" << std::endl;
    } else {
        std::cout << "   ✗ Segment manipulation failed" << std::endl;
        allTestsPassed = false;
    }
    
    // Test 5: isValid() dengan caching
    std::cout << "\n5. Testing isValid() with caching..." << std::endl;
    bool firstCall = contour.isValid();
    bool secondCall = contour.isValid();
    
    if (firstCall && secondCall && firstCall == secondCall) {
        std::cout << "   ✓ isValid() with caching works" << std::endl;
    } else {
        std::cout << "   ✗ isValid() failed" << std::endl;
        allTestsPassed = false;
    }
    
    // Test 6: Utility function untuk membuat Contour dari points
    std::cout << "\n6. Testing utility function for polyline creation..." << std::endl;
    std::vector<Point2D> points = {
        Point2D(0, 0), Point2D(1, 0), Point2D(1, 1), Point2D(0, 1)
    };
    
    auto polylineContour = utilities::createPolylineContour(points);
    if (polylineContour.size() == 3 && polylineContour.isValid()) {
        std::cout << "   ✓ Polyline creation works" << std::endl;
    } else {
        std::cout << "   ✗ Polyline creation failed" << std::endl;
        allTestsPassed = false;
    }
    
    // Test 7: Asynchronous search untuk valid/invalid contours
    std::cout << "\n7. Testing asynchronous contour search..." << std::endl;
    
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
    
    // Validate results
    bool asyncTestPassed = true;
    
    if (validResults.size() == 2 && invalidResults.size() == 2) {
        std::cout << "   ✓ Found correct number of valid/invalid contours" << std::endl;
    } else {
        std::cout << "   ✗ Wrong number of valid/invalid contours found" << std::endl;
        asyncTestPassed = false;
    }
    
    // Check uniqueness
    std::set<Contour*> validSet(validResults.begin(), validResults.end());
    std::set<Contour*> invalidSet(invalidResults.begin(), invalidResults.end());
    
    if (validSet.size() == validResults.size() && invalidSet.size() == invalidResults.size()) {
        std::cout << "   ✓ Sets are unique" << std::endl;
    } else {
        std::cout << "   ✗ Sets contain duplicates" << std::endl;
        asyncTestPassed = false;
    }
    
    // Check no overlap
    std::vector<Contour*> intersection;
    std::set_intersection(validSet.begin(), validSet.end(),
                         invalidSet.begin(), invalidSet.end(),
                         std::back_inserter(intersection));
    
    if (intersection.empty()) {
        std::cout << "   ✓ No overlap between valid and invalid sets" << std::endl;
    } else {
        std::cout << "   ✗ Overlap found between sets" << std::endl;
        asyncTestPassed = false;
    }
    
    // Check all original contours are found
    std::set<Contour*> allFound;
    allFound.insert(validResults.begin(), validResults.end());
    allFound.insert(invalidResults.begin(), invalidResults.end());
    
    std::set<Contour*> allOriginal;
    for (const auto& contour : testContours) {
        allOriginal.insert(contour.get());
    }
    
    if (allFound == allOriginal) {
        std::cout << "   ✓ All original contours found" << std::endl;
    } else {
        std::cout << "   ✗ Not all original contours found" << std::endl;
        asyncTestPassed = false;
    }
    
    if (asyncTestPassed) {
        std::cout << "   ✓ Asynchronous search works correctly" << std::endl;
    } else {
        std::cout << "   ✗ Asynchronous search failed" << std::endl;
        allTestsPassed = false;
    }
    
    // Final result
    std::cout << "\n===================================" << std::endl;
    if (allTestsPassed) {
        std::cout << "ALL REQUIREMENTS PASSED! ✓" << std::endl;
    } else {
        std::cout << "SOME REQUIREMENTS FAILED! ✗" << std::endl;
    }
    std::cout << "===================================" << std::endl;
}

int main() {
    try {
        testAllRequirements();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
} 