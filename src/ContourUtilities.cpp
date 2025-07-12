#include "../include/ContourUtilities.h"
#include <stdexcept>
#include <algorithm>
#include <cmath>
#include <span>
#include <cstdlib>

namespace contour::utilities
{
    // Basic polyline creation functions
    Contour createPolylineContour(std::initializer_list<geometry::Point2D> points)
    {
        return createPolylineContour(std::vector<geometry::Point2D>(points));
    }

    Contour createPolylineContour(const std::vector<geometry::Point2D>& points)
    {
        if (points.size() < 2)
        {
            throw std::invalid_argument("createPolylineContour: need at least 2 points");
        }

        Contour contour;
        for (size_t i = 0; i < points.size() - 1; ++i)
        {
            // Skip zero-length segments
            if (!points[i].isEqual(points[i + 1]))
            {
                contour.addSegment(createLineSegment(points[i], points[i + 1]));
            }
        }

        if (contour.empty())
        {
            throw std::invalid_argument("createPolylineContour: all consecutive points are identical");
        }

        return contour;
    }

    Contour createPolylineContour(std::span<const geometry::Point2D> points)
    {
        return createPolylineContour(std::vector<geometry::Point2D>(points.begin(), points.end()));
    }

    Contour createPolylineContour(const geometry::Point2D* points, size_t count)
    {
        if (!points)
        {
            throw std::invalid_argument("createPolylineContour: points array cannot be null");
        }
        return createPolylineContour(std::vector<geometry::Point2D>(points, points + count));
    }

    // Advanced polyline creation with options
    Contour createPolylineContour(const std::vector<geometry::Point2D>& points, 
                                 const PolylineOptions& options)
    {
        if (points.size() < 2)
        {
            throw std::invalid_argument("createPolylineContour: need at least 2 points");
        }

        auto processedPoints = points;

        // Validate input if requested
        if (options.validateInput)
        {
            if (!validatePointSequence(processedPoints, false))
            {
                throw std::invalid_argument("createPolylineContour: invalid point sequence");
            }
        }

        // Remove consecutive duplicates
        processedPoints = removeConsecutiveDuplicates(processedPoints);

        // Remove collinear points if requested
        if (options.removeCollinear)
        {
            processedPoints = removeCollinearPoints(processedPoints, options.collinearThreshold);
        }

        // Check we still have enough points
        if (processedPoints.size() < 2)
        {
            throw std::invalid_argument("createPolylineContour: insufficient points after processing");
        }

        // Create the contour
        Contour contour;
        size_t endIndex = processedPoints.size();
        
        // If closing path, add one more segment
        if (options.closePath && !processedPoints.front().isEqual(processedPoints.back()))
        {
            endIndex = processedPoints.size(); // Will create segment back to first point
        }
        else if (options.closePath)
        {
            endIndex = processedPoints.size() - 1; // Last point same as first, don't duplicate
        }
        else
        {
            endIndex = processedPoints.size() - 1; // Normal open path
        }

        for (size_t i = 0; i < endIndex; ++i)
        {
            size_t nextIndex = (i + 1) % processedPoints.size();
            
            if (!processedPoints[i].isEqual(processedPoints[nextIndex]))
            {
                contour.addSegment(createLineSegment(processedPoints[i], processedPoints[nextIndex]));
            }
        }

        if (contour.empty())
        {
            throw std::invalid_argument("createPolylineContour: no valid segments created");
        }

        return contour;
    }

    // Utility functions for contour analysis
    std::vector<geometry::Point2D> extractPoints(const Contour& contour)
    {
        std::vector<geometry::Point2D> points;
        if (contour.empty())
        {
            return points;
        }

        // Add start point of first segment
        points.push_back(contour[0].getStartPoint());

        // Add end points of all segments
        for (size_t i = 0; i < contour.size(); ++i)
        {
            points.push_back(contour[i].getEndPoint());
        }

        return points;
    }

    bool arePointsCollinear(const geometry::Point2D& p1, 
                           const geometry::Point2D& p2, 
                           const geometry::Point2D& p3, 
                           double threshold)
    {
        // Calculate cross product of vectors (p2-p1) and (p3-p1)
        geometry::Vector2D v1 = p2 - p1;
        geometry::Vector2D v2 = p3 - p1;
        
        // Cross product in 2D: v1.x * v2.y - v1.y * v2.x
        double crossProduct = v1.x * v2.y - v1.y * v2.x;
        
        return std::abs(crossProduct) < threshold;
    }

    // Validation helpers
    bool validatePointSequence(const std::vector<geometry::Point2D>& points, bool allowDuplicates)
    {
        if (points.size() < 2)
        {
            return false;
        }

        if (!allowDuplicates)
        {
            // Check for consecutive duplicates
            for (size_t i = 0; i < points.size() - 1; ++i)
            {
                if (points[i].isEqual(points[i + 1]))
                {
                    return false;
                }
            }
        }

        return true;
    }

    std::vector<geometry::Point2D> removeConsecutiveDuplicates(
        const std::vector<geometry::Point2D>& points, double epsilon)
    {
        if (points.empty())
        {
            return points;
        }

        std::vector<geometry::Point2D> result;
        result.reserve(points.size());
        result.push_back(points[0]);

        for (size_t i = 1; i < points.size(); ++i)
        {
            if (!points[i].isEqual(points[i - 1], epsilon))
            {
                result.push_back(points[i]);
            }
        }

        return result;
    }

    std::vector<geometry::Point2D> removeCollinearPoints(
        const std::vector<geometry::Point2D>& points, double threshold)
    {
        if (points.size() < 3)
        {
            return points;
        }

        std::vector<geometry::Point2D> result;
        result.reserve(points.size());
        result.push_back(points[0]);

        for (size_t i = 1; i < points.size() - 1; ++i)
        {
            if (!arePointsCollinear(points[i - 1], points[i], points[i + 1], threshold))
            {
                result.push_back(points[i]);
            }
        }

        result.push_back(points.back());
        return result;
    }

    // Geometric utilities
    double calculatePolygonArea(const std::vector<geometry::Point2D>& points)
    {
        if (points.size() < 3)
        {
            return 0.0;
        }

        double area = 0.0;
        size_t n = points.size();

        for (size_t i = 0; i < n; ++i)
        {
            size_t j = (i + 1) % n;
            area += points[i].x * points[j].y;
            area -= points[j].x * points[i].y;
        }

        return std::abs(area) / 2.0;
    }

    geometry::Point2D calculateCentroid(const std::vector<geometry::Point2D>& points)
    {
        if (points.empty())
        {
            return geometry::Point2D(0.0, 0.0);
        }

        double totalX = 0.0;
        double totalY = 0.0;

        for (const auto& point : points)
        {
            totalX += point.x;
            totalY += point.y;
        }

        return geometry::Point2D(totalX / points.size(), totalY / points.size());
    }

    bool isClockwise(const std::vector<geometry::Point2D>& points)
    {
        if (points.size() < 3)
        {
            return false; // Not enough points to determine orientation
        }

        double sum = 0.0;
        size_t n = points.size();

        for (size_t i = 0; i < n; ++i)
        {
            size_t j = (i + 1) % n;
            sum += (points[j].x - points[i].x) * (points[j].y + points[i].y);
        }

        return sum > 0.0;
    }
} 