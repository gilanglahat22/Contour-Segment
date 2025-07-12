# Contributing to ContourSegment

## Overview

Thank you for your interest in contributing to ContourSegment! This guide will help you understand how to contribute effectively to the project.

## Getting Started

### Prerequisites
- Visual Studio 2019/2022 with C++ workload
- Qt 6.5.3 with MSVC 2019 64-bit components
- Git for version control
- Understanding of C++20 and Qt

### Development Environment Setup
```cmd
# Clone the repository
git clone https://github.com/your-username/ContourSegment.git
cd ContourSegment

# Setup environment
setup_env.bat

# Build the project
build.bat

# Run tests
test.bat
```

## Development Workflow

### 1. Fork and Clone
1. Fork the repository on GitHub
2. Clone your fork locally
3. Add the original repository as upstream
```cmd
git remote add upstream https://github.com/original/ContourSegment.git
```

### 2. Create Feature Branch
```cmd
# Create and switch to feature branch
git checkout -b feature/your-feature-name

# Or for bug fixes
git checkout -b fix/your-bug-description
```

### 3. Make Changes
- Follow the coding standards
- Write tests for new features
- Update documentation
- Test your changes thoroughly

### 4. Commit Changes
```cmd
# Stage changes
git add .

# Commit with descriptive message
git commit -m "Add new feature: brief description

- Detailed description of changes
- Any important notes
- Related issue number"
```

### 5. Push and Create Pull Request
```cmd
# Push to your fork
git push origin feature/your-feature-name

# Create pull request on GitHub
```

## Coding Standards

### C++ Style Guide

#### Naming Conventions
```cpp
// Classes: PascalCase
class ContourSegment { };

// Methods: camelCase
void addSegment(std::unique_ptr<Segment> segment);

// Variables: camelCase
int segmentCount;
std::vector<Point2D> vertices;

// Constants: UPPER_SNAKE_CASE
const double EPSILON = 1e-6;
const std::string VERSION = "1.0.0";

// Namespaces: lowercase
namespace geometry { }
namespace contour { }
```

#### File Organization
```cpp
// Header files (.h)
#pragma once

#include <vector>
#include <memory>

namespace contour {
    class Contour {
    public:
        // Public interface
    private:
        // Private implementation
    };
}

// Source files (.cpp)
#include "../include/Contour.h"
#include <algorithm>
#include <stdexcept>

namespace contour {
    // Implementation
}
```

#### Code Structure
```cpp
// 1. Include guards
#pragma once

// 2. System includes
#include <vector>
#include <memory>
#include <string>

// 3. Third-party includes
#include <QtWidgets>

// 4. Project includes
#include "Geometry.h"
#include "Segment.h"

// 5. Namespace declarations
namespace contour {

// 6. Class declaration
class Contour {
public:
    // Public interface first
    Contour() = default;
    ~Contour() = default;
    
    // Copy and move semantics
    Contour(const Contour& other);
    Contour(Contour&&) noexcept = default;
    Contour& operator=(const Contour& other);
    Contour& operator=(Contour&&) noexcept = default;
    
    // Public methods
    void addSegment(std::unique_ptr<Segment> segment);
    bool isValid() const;
    
private:
    // Private implementation
    std::vector<std::unique_ptr<Segment>> m_segments;
    mutable std::optional<bool> m_validityCache;
};

} // namespace contour
```

### Qt Style Guide

#### Qt-Specific Conventions
```cpp
// Qt object naming
QGraphicsView *m_graphicsView;  // Member variables with m_ prefix
QAction *m_actionNew;           // Action members

// Signal/slot connections
connect(m_actionNew, &QAction::triggered, this, &MainWindow::on_actionNew_triggered);

// Event handlers
void MainWindow::mousePressEvent(QMouseEvent *event) {
    // Implementation
}

// Qt macros
Q_OBJECT  // For classes with signals/slots
Q_DECLARE_METATYPE(Contour)  // For custom types
```

#### UI Design Principles
- **Consistency**: Follow Qt design patterns
- **Accessibility**: Support keyboard navigation
- **Responsiveness**: Don't block the UI thread
- **Error Handling**: Provide user-friendly error messages

## Testing Guidelines

### Writing Tests
```cpp
void testNewFeature(TestSuite& suite) {
    std::cout << "\n=== Testing New Feature ===" << std::endl;
    
    // Test normal operation
    auto result = createNewFeature();
    suite.runTest("New feature creation", result != nullptr);
    
    // Test edge cases
    try {
        auto invalidResult = createNewFeatureWithInvalidInput();
        suite.runTest("Invalid input should throw", false);
    } catch (const std::invalid_argument& e) {
        suite.runTest("Invalid input exception", true);
    }
    
    // Test performance
    auto start = std::chrono::high_resolution_clock::now();
    // ... test code ...
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    suite.runTest("Performance within limits", duration.count() < 100);
}
```

### Test Categories
1. **Unit Tests**: Test individual functions and classes
2. **Integration Tests**: Test component interactions
3. **Performance Tests**: Test performance characteristics
4. **Error Handling Tests**: Test exception scenarios

### Running Tests
```cmd
# Run all tests
test.bat

# Run specific test categories
ContourSegment.exe --test-only --category=geometry

# Run with verbose output
ContourSegment.exe --test-only --verbose
```

## Documentation Standards

### Code Documentation
```cpp
/**
 * @brief Represents a 2D contour composed of connected segments
 * 
 * The Contour class manages a collection of geometric segments that form
 * a continuous path. It provides validation, iteration, and utility methods.
 * 
 * @example
 * Contour contour;
 * contour.addSegment(createLineSegment(Point2D(0,0), Point2D(1,0)));
 * contour.addSegment(createLineSegment(Point2D(1,0), Point2D(1,1)));
 * 
 * @note Contours are not thread-safe. Use external synchronization for
 *       multi-threaded access.
 */
class Contour {
public:
    /**
     * @brief Adds a segment to the contour
     * 
     * @param segment The segment to add (ownership transferred)
     * @throws std::invalid_argument if segment is null
     * @throws std::runtime_error if segment cannot be connected
     */
    void addSegment(std::unique_ptr<Segment> segment);
    
    /**
     * @brief Validates the contour
     * 
     * Checks that all segments are properly connected and the contour
     * meets all validity requirements.
     * 
     * @param epsilon Tolerance for floating-point comparisons
     * @return true if contour is valid, false otherwise
     */
    bool isValid(double epsilon = geometry::EPSILON) const;
};
```

### API Documentation
- Document all public interfaces
- Include usage examples
- Specify preconditions and postconditions
- Document exception behavior
- Update documentation with code changes

## Git Workflow

### Commit Messages
```cmd
# Good commit messages
git commit -m "Add Bezier curve segment support

- Implement BezierSegment class with cubic curve math
- Add factory function createBezierSegment()
- Include comprehensive unit tests
- Update documentation with usage examples

Fixes #123"
```

### Branch Naming
```cmd
# Feature branches
feature/add-bezier-curves
feature/improve-performance
feature/add-undo-redo

# Bug fix branches
fix/memory-leak-in-contour-validation
fix/crash-when-loading-invalid-file
fix/grid-snap-not-working

# Documentation branches
docs/update-api-reference
docs/add-user-guide
docs/fix-typos
```

### Pull Request Guidelines
1. **Title**: Clear, descriptive title
2. **Description**: Detailed description of changes
3. **Testing**: Evidence that tests pass
4. **Documentation**: Updated documentation
5. **Screenshots**: For UI changes

## Review Process

### Code Review Checklist
- [ ] Code follows style guidelines
- [ ] Tests are included and pass
- [ ] Documentation is updated
- [ ] No performance regressions
- [ ] Error handling is appropriate
- [ ] Security considerations addressed

### Review Comments
```cpp
// Good review comment
// Consider using std::move here to avoid unnecessary copying
auto segment = std::make_unique<LineSegment>(start, end);
contour.addSegment(std::move(segment));

// Better approach
contour.addSegment(std::make_unique<LineSegment>(start, end));
```

## Issue Reporting

### Bug Reports
```markdown
**Bug Description**
Brief description of the bug

**Steps to Reproduce**
1. Open ContourSegment
2. Click "Add Line"
3. Draw a line segment
4. Observe crash

**Expected Behavior**
Application should continue working normally

**Actual Behavior**
Application crashes with segmentation fault

**Environment**
- OS: Windows 10 21H2
- Qt: 6.5.3
- Visual Studio: 2019
- Build: Release x64

**Additional Information**
- Crash occurs only with large contours
- Works fine in debug builds
```

### Feature Requests
```markdown
**Feature Description**
Add support for elliptical arc segments

**Use Case**
Users need to create contours with elliptical curves for CAD applications

**Proposed Implementation**
- Create EllipticalArcSegment class
- Add factory function createEllipticalArcSegment()
- Update UI to support elliptical arc drawing
- Include validation for elliptical arc parameters

**Acceptance Criteria**
- [ ] Can create elliptical arc segments
- [ ] UI supports elliptical arc drawing
- [ ] Validation works correctly
- [ ] Documentation is updated
- [ ] Tests are included
```

## Performance Guidelines

### Optimization Principles
1. **Measure First**: Profile before optimizing
2. **Algorithm Choice**: Use efficient algorithms
3. **Memory Management**: Minimize allocations
4. **Caching**: Cache expensive calculations

### Performance Testing
```cpp
void testPerformance(TestSuite& suite) {
    // Test with large datasets
    std::vector<Point2D> largeContour(10000);
    // ... populate with test data ...
    
    auto start = std::chrono::high_resolution_clock::now();
    auto contour = utilities::createPolylineContour(largeContour);
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    suite.runTest("Large contour creation performance", duration.count() < 1000);
}
```

## Security Guidelines

### Input Validation
```cpp
void validateInput(const std::string& input) {
    if (input.empty()) {
        throw std::invalid_argument("Input cannot be empty");
    }
    
    if (input.length() > MAX_INPUT_LENGTH) {
        throw std::invalid_argument("Input too long");
    }
    
    // Validate content
    for (char c : input) {
        if (!isValidCharacter(c)) {
            throw std::invalid_argument("Invalid character in input");
        }
    }
}
```

### Memory Safety
```cpp
// Use smart pointers
std::unique_ptr<Segment> segment = createSegment();

// Avoid raw pointers
// Bad: Segment* segment = new Segment();
// Good: auto segment = std::make_unique<Segment>();

// Use RAII
class ResourceManager {
public:
    ResourceManager() { acquire(); }
    ~ResourceManager() { release(); }
private:
    void acquire();
    void release();
};
```

## Release Process

### Version Management
```cpp
// Version constants
const std::string VERSION = "1.0.0";
const std::string BUILD_DATE = __DATE__;
const std::string BUILD_TIME = __TIME__;

// Version checking
bool isCompatibleVersion(const std::string& version) {
    // Version compatibility logic
    return true;
}
```

### Release Checklist
- [ ] All tests pass
- [ ] Documentation is updated
- [ ] Version numbers are updated
- [ ] Changelog is updated
- [ ] Release notes are written
- [ ] Installer is tested
- [ ] Performance is acceptable

## Community Guidelines

### Communication
- Be respectful and constructive
- Help other contributors
- Share knowledge and experience
- Report issues promptly

### Recognition
- Contributors are credited in README
- Significant contributions are highlighted
- Regular contributors may become maintainers

## Getting Help

### Resources
1. **Documentation**: Check docs/ folder
2. **Issues**: Search existing issues
3. **Discussions**: Use GitHub Discussions
4. **Code Examples**: Look at existing code

### Contact
- **Issues**: GitHub Issues
- **Discussions**: GitHub Discussions
- **Email**: project-email@example.com

## Conclusion

Thank you for contributing to ContourSegment! Your contributions help make the project better for everyone. Follow these guidelines to ensure smooth collaboration and high-quality contributions.

Remember:
- **Quality over quantity**: Focus on well-tested, documented code
- **Communication**: Keep the community informed of your work
- **Learning**: Contribute to learn and improve your skills
- **Fun**: Enjoy the process of building something useful together 