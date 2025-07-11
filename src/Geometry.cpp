#include "../include/Geometry.h"
#include <cstdlib>

namespace geometry
{
    // Point2D operators
    Point2D Point2D::operator+(const Point2D& other) const
    {
        return Point2D(x + other.x, y + other.y);
    }

    Point2D Point2D::operator-(const Point2D& other) const
    {
        return Point2D(x - other.x, y - other.y);
    }

    Point2D Point2D::operator*(double scalar) const
    {
        return Point2D(x * scalar, y * scalar);
    }

    Point2D& Point2D::operator+=(const Point2D& other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }

    Point2D& Point2D::operator-=(const Point2D& other)
    {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    Point2D& Point2D::operator*=(double scalar)
    {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    // Distance and comparison functions
    double Point2D::distanceTo(const Point2D& other) const
    {
        return std::sqrt(distanceSquaredTo(other));
    }

    double Point2D::distanceSquaredTo(const Point2D& other) const
    {
        const double dx = x - other.x;
        const double dy = y - other.y;
        return dx * dx + dy * dy;
    }

    bool Point2D::isEqual(const Point2D& other, double epsilon) const
    {
        return geometry::isEqual(x, other.x, epsilon) && geometry::isEqual(y, other.y, epsilon);
    }

    // Utility functions
    double Point2D::magnitude() const
    {
        return std::sqrt(x * x + y * y);
    }

    Point2D Point2D::normalized() const
    {
        const double mag = magnitude();
        if (geometry::isEqual(mag, 0.0))
        {
            return Point2D(0.0, 0.0);
        }
        return Point2D(x / mag, y / mag);
    }

    double Point2D::dot(const Point2D& other) const
    {
        return x * other.x + y * other.y;
    }

    // Global utility functions
    bool isEqual(double a, double b, double epsilon)
    {
        return std::abs(a - b) < epsilon;
    }

    double degreesToRadians(double degrees)
    {
        return degrees * PI / 180.0;
    }

    double radiansToDegrees(double radians)
    {
        return radians * 180.0 / PI;
    }
} 