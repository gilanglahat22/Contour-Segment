# ContourSegment Testing Guide

## Overview

This guide covers testing strategies, test execution, and best practices for the ContourSegment application. The project includes comprehensive unit tests and integration tests.

## Test Structure

### Test Files
```
tests/
└── ContourTests.cpp    # Main test suite
```

### Test Categories
- **Unit Tests**: Individual class and method testing
- **Integration Tests**: Component interaction testing
- **Performance Tests**: Performance and memory testing
- **Error Handling Tests**: Exception and error condition testing

## Running Tests

### Command Line Execution
```cmd
# Run all tests
test.bat

# Run specific test categories
ContourSegment.exe --test-only

# Run with verbose output
ContourSegment.exe --test-only --verbose
```

### Visual Studio Testing
1. Open `ContourSegment.sln` in Visual Studio
2. Build the solution
3. Set `ContourTests.cpp` as startup file
4. Run with debugging (F5)

### Qt Creator Testing
1. Open `ContourSegment.pro` in Qt Creator
2. Configure test run settings
3. Run tests from Qt Creator

## Test Framework

### TestSuite Class
```cpp
class TestSuite {
public:
    void runTest(const std::string& name, bool condition);
    void runTest(const std::string& name, const std::string& expected, const std::string& actual);
    void runTest(const std::string& name, double expected, double actual, double epsilon = 1e-6);
    
    int getPassedCount() const;
    int getFailedCount() const;
    int getTotalCount() const;
    void printSummary() const;
};
```

### Test Categories

#### Geometry Tests
```cpp
void testPoint2D(TestSuite& suite) {
    std::cout << "\n=== Testing Point2D ===" << std::endl;
    
    // Basic construction
    geometry::Point2D p1(1.0, 2.0);
    suite.runTest("Point2D construction", p1.x == 1.0 && p1.y == 2.0);
    
    // Distance calculation
    geometry::Point2D p2(4.0, 6.0);
    double distance = p1.distanceTo(p2);
    suite.runTest("Point2D distance", distance, 5.0);
    
    // Vector operations
    geometry::Point2D sum = p1 + p2;
    suite.runTest("Point2D addition", sum.x == 5.0 && sum.y == 8.0);
    
    // Equality testing
    geometry::Point2D p3(1.0, 2.0);
    suite.runTest("Point2D equality", p1.isEqual(p3));
}
```

#### Segment Tests
```cpp
void testLineSegment(TestSuite& suite) {
    std::cout << "\n=== Testing LineSegment ===" << std::endl;
    
    // Valid line segment
    auto line = createLineSegment(
        geometry::Point2D(0.0, 0.0),
        geometry::Point2D(3.0, 4.0)
    );
    suite.runTest("LineSegment creation", line != nullptr);
    suite.runTest("LineSegment type", line->getType() == contour::SegmentType::Line);
    suite.runTest("LineSegment length", line->getLength(), 5.0);
    
    // Invalid line segment (same points)
    try {
        auto invalidLine = createLineSegment(
            geometry::Point2D(1.0, 1.0),
            geometry::Point2D(1.0, 1.0)
        );
        suite.runTest("Invalid LineSegment should throw", false);
    } catch (const std::invalid_argument& e) {
        suite.runTest("Invalid LineSegment exception", true);
    }
}
```

#### Contour Tests
```cpp
void testContour(TestSuite& suite) {
    std::cout << "\n=== Testing Contour ===" << std::endl;
    
    // Create valid contour
    contour::Contour contour;
    contour.addSegment(createLineSegment(
        geometry::Point2D(0.0, 0.0),
        geometry::Point2D(1.0, 0.0)
    ));
    contour.addSegment(createLineSegment(
        geometry::Point2D(1.0, 0.0),
        geometry::Point2D(1.0, 1.0)
    ));
    contour.addSegment(createLineSegment(
        geometry::Point2D(1.0, 1.0),
        geometry::Point2D(0.0, 1.0)
    ));
    contour.addSegment(createLineSegment(
        geometry::Point2D(0.0, 1.0),
        geometry::Point2D(0.0, 0.0)
    ));
    
    suite.runTest("Contour size", contour.size() == 4);
    suite.runTest("Contour validity", contour.isValid());
    suite.runTest("Contour closure", contour.isClosed());
    suite.runTest("Contour total length", contour.getTotalLength(), 4.0);
}
```

#### Utility Tests
```cpp
void testUtilities(TestSuite& suite) {
    std::cout << "\n=== Testing Utilities ===" << std::endl;
    
    // Polyline creation
    std::vector<geometry::Point2D> points = {
        geometry::Point2D(0.0, 0.0),
        geometry::Point2D(1.0, 0.0),
        geometry::Point2D(1.0, 1.0),
        geometry::Point2D(0.0, 1.0)
    };
    
    auto polyline = utilities::createPolylineContour(points);
    suite.runTest("Polyline creation", polyline.size() == 3);
    suite.runTest("Polyline validity", polyline.isValid());
    
    // Closed polyline
    utilities::PolylineOptions options;
    options.closePath = true;
    auto closedPolyline = utilities::createPolylineContour(points, options);
    suite.runTest("Closed polyline", closedPolyline.isClosed());
}
```

## Test Execution

### Running Individual Tests
```cpp
// In main function
TestSuite suite;

// Run specific test categories
testGeometry(suite);
testSegments(suite);
testContours(suite);
testUtilities(suite);
testVisualization(suite);
testErrorHandling(suite);
testPerformance(suite);

// Print results
suite.printSummary();
```

### Test Output Format
```
=== Testing Geometry ===
✓ Point2D construction: PASSED
✓ Point2D distance: PASSED
✓ Point2D addition: PASSED
✓ Point2D equality: PASSED

=== Testing Segments ===
✓ LineSegment creation: PASSED
✓ LineSegment type: PASSED
✓ LineSegment length: PASSED
✓ Invalid LineSegment exception: PASSED

Test Summary:
Total: 12, Passed: 12, Failed: 0
```

## Test Categories

### 1. Geometry Tests
- **Point2D**: Construction, operations, comparison
- **Vector2D**: Construction, operations, normalization
- **Coordinate Systems**: World/scene coordinate conversion

### 2. Segment Tests
- **LineSegment**: Creation, validation, operations
- **ArcSegment**: Creation, validation, operations
- **Segment Interface**: Polymorphic behavior
- **Factory Functions**: Segment creation utilities

### 3. Contour Tests
- **Contour Management**: Add, remove, insert segments
- **Validation**: Connectivity, closure, validity
- **Iteration**: Range-based loops, STL compatibility
- **Copy/Move Semantics**: Proper object behavior

### 4. Utility Tests
- **Polyline Creation**: Point sequence to contour conversion
- **Options Handling**: Advanced polyline options
- **Validation**: Input validation and error handling
- **Optimization**: Collinear point removal

### 5. Visualization Tests
- **ContourVisualizer**: ASCII art generation
- **Options**: Customization and configuration
- **Output Formats**: Various visualization formats
- **Performance**: Large contour rendering

### 6. Error Handling Tests
- **Invalid Input**: Malformed data handling
- **Edge Cases**: Boundary condition testing
- **Exception Safety**: Proper exception propagation
- **Resource Management**: Memory leak prevention

### 7. Performance Tests
- **Large Contours**: Performance with many segments
- **Memory Usage**: Memory consumption analysis
- **Caching**: Validation result caching
- **Optimization**: Algorithm efficiency

## Test Data

### Sample Contours
```cpp
// Simple square
std::vector<geometry::Point2D> squarePoints = {
    geometry::Point2D(0.0, 0.0),
    geometry::Point2D(1.0, 0.0),
    geometry::Point2D(1.0, 1.0),
    geometry::Point2D(0.0, 1.0)
};

// Triangle
std::vector<geometry::Point2D> trianglePoints = {
    geometry::Point2D(0.0, 0.0),
    geometry::Point2D(1.0, 0.0),
    geometry::Point2D(0.5, 1.0)
};

// L-shape
std::vector<geometry::Point2D> lShapePoints = {
    geometry::Point2D(0.0, 0.0),
    geometry::Point2D(2.0, 0.0),
    geometry::Point2D(2.0, 1.0),
    geometry::Point2D(1.0, 1.0),
    geometry::Point2D(1.0, 2.0),
    geometry::Point2D(0.0, 2.0)
};
```

### Test Utilities
```cpp
// Helper function to create test contours
contour::Contour createTestContour(const std::vector<geometry::Point2D>& points) {
    contour::Contour contour;
    for (size_t i = 0; i < points.size() - 1; ++i) {
        contour.addSegment(createLineSegment(points[i], points[i + 1]));
    }
    return contour;
}

// Helper function to validate test results
bool validateContourProperties(const contour::Contour& contour, 
                             size_t expectedSegments,
                             bool expectedValid,
                             bool expectedClosed) {
    return contour.size() == expectedSegments &&
           contour.isValid() == expectedValid &&
           contour.isClosed() == expectedClosed;
}
```

## Continuous Integration

### Automated Testing
```cmd
# Run tests in CI environment
msbuild ContourSegment.sln /p:Configuration=Release
ContourSegment.exe --test-only --ci-mode
```

### Test Reporting
```cpp
// Generate test report
void generateTestReport(const TestSuite& suite, const std::string& filename) {
    std::ofstream report(filename);
    report << "Test Report\n";
    report << "===========\n";
    report << "Total Tests: " << suite.getTotalCount() << "\n";
    report << "Passed: " << suite.getPassedCount() << "\n";
    report << "Failed: " << suite.getFailedCount() << "\n";
    report << "Success Rate: " << (suite.getPassedCount() * 100.0 / suite.getTotalCount()) << "%\n";
}
```

## Best Practices

### Test Design
1. **Single Responsibility**: Each test should test one thing
2. **Independence**: Tests should not depend on each other
3. **Readability**: Clear test names and structure
4. **Maintainability**: Easy to update when code changes

### Test Data
1. **Representative**: Use realistic test data
2. **Edge Cases**: Include boundary conditions
3. **Error Conditions**: Test invalid inputs
4. **Performance**: Test with large datasets

### Test Execution
1. **Fast**: Tests should run quickly
2. **Reliable**: Tests should be deterministic
3. **Comprehensive**: Cover all code paths
4. **Automated**: Can run without manual intervention

### Test Maintenance
1. **Update Tests**: Keep tests current with code changes
2. **Review Coverage**: Ensure adequate test coverage
3. **Refactor Tests**: Keep tests clean and maintainable
4. **Document Tests**: Explain complex test scenarios

## Debugging Tests

### Common Issues
1. **Floating Point Precision**: Use epsilon for comparisons
2. **Memory Leaks**: Check for proper cleanup
3. **Thread Safety**: Ensure thread-safe test execution
4. **Platform Differences**: Handle platform-specific behavior

### Debugging Tools
```cpp
// Enable debug output
#define DEBUG_TESTS
#ifdef DEBUG_TESTS
    std::cout << "Debug: " << variable << std::endl;
#endif

// Use assertions for debugging
assert(condition && "Debug assertion failed");

// Profile test performance
auto start = std::chrono::high_resolution_clock::now();
// ... test code ...
auto end = std::chrono::high_resolution_clock::now();
auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
std::cout << "Test took " << duration.count() << "ms" << std::endl;
```

## Conclusion

The testing framework provides comprehensive coverage of the ContourSegment library. Regular test execution ensures code quality and prevents regressions. Follow the best practices outlined in this guide to maintain high-quality tests.

For additional testing needs, consider:
- **Property-based testing**: Using frameworks like QuickCheck
- **Fuzzing**: Random input testing for robustness
- **Integration testing**: Testing with external systems
- **Performance benchmarking**: Continuous performance monitoring 