#pragma once

#include <cmath>
#include <limits>

namespace geometry
{
    // Constants
    constexpr double EPSILON = 1e-9;
    constexpr double PI = 3.14159265358979323846;

    // 2D Point class
    class Point2D
    {
    public:
        double x, y;

        Point2D() : x(0.0), y(0.0) {}
        Point2D(double x, double y) : x(x), y(y) {}

        // Copy constructor and assignment
        Point2D(const Point2D&) = default;
        Point2D& operator=(const Point2D&) = default;

        // Move constructor and assignment
        Point2D(Point2D&&) noexcept = default;
        Point2D& operator=(Point2D&&) noexcept = default;

        // Operators
        Point2D operator+(const Point2D& other) const;
        Point2D operator-(const Point2D& other) const;
        Point2D operator*(double scalar) const;
        Point2D& operator+=(const Point2D& other);
        Point2D& operator-=(const Point2D& other);
        Point2D& operator*=(double scalar);

        // Distance and comparison
        double distanceTo(const Point2D& other) const;
        double distanceSquaredTo(const Point2D& other) const;
        bool isEqual(const Point2D& other, double epsilon = EPSILON) const;
        
        // Utility functions
        double magnitude() const;
        Point2D normalized() const;
        double dot(const Point2D& other) const;
    };

    // 2D Vector (alias for Point2D for clarity in usage)
    using Vector2D = Point2D;

    // Utility functions
    bool isEqual(double a, double b, double epsilon = EPSILON);
    double degreesToRadians(double degrees);
    double radiansToDegrees(double radians);
} 