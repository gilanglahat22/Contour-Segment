#pragma once

#include "Contour.h"
#include "Geometry.h"
#include <vector>
#include <initializer_list>

namespace contour
{
    namespace utilities
    {
        // Main utility function - flexible with multiple overloads
        template<typename Container>
        Contour createPolylineContour(const Container& points);
        
        // Overloads for different input types
        Contour createPolylineContour(std::initializer_list<geometry::Point2D> points);
        Contour createPolylineContour(const std::vector<geometry::Point2D>& points);
        
        // C-style array support
        Contour createPolylineContour(const geometry::Point2D* points, size_t count);
        
        // Iterator-based version for maximum flexibility
        template<typename Iterator>
        Contour createPolylineContour(Iterator begin, Iterator end);

        // Advanced utilities
        struct PolylineOptions
        {
            bool closePath = false;           // Whether to close the contour
            bool removeCollinear = false;     // Remove collinear points for efficiency
            double collinearThreshold = geometry::EPSILON; // Threshold for collinear detection
            bool validateInput = true;        // Validate input points (no duplicates, etc.)
        };

        // Advanced polyline creation with options
        template<typename Container>
        Contour createPolylineContour(const Container& points, const PolylineOptions& options);
        
        Contour createPolylineContour(const std::vector<geometry::Point2D>& points, 
                                     const PolylineOptions& options);

        // Utility functions for contour analysis and manipulation
        std::vector<geometry::Point2D> extractPoints(const Contour& contour);
        bool arePointsCollinear(const geometry::Point2D& p1, 
                               const geometry::Point2D& p2, 
                               const geometry::Point2D& p3, 
                               double threshold = geometry::EPSILON);
        
        // Validation helpers
        bool validatePointSequence(const std::vector<geometry::Point2D>& points, 
                                  bool allowDuplicates = false);
        std::vector<geometry::Point2D> removeConsecutiveDuplicates(
            const std::vector<geometry::Point2D>& points, 
            double epsilon = geometry::EPSILON);
        
        std::vector<geometry::Point2D> removeCollinearPoints(
            const std::vector<geometry::Point2D>& points, 
            double threshold = geometry::EPSILON);

        // Geometric utilities
        double calculatePolygonArea(const std::vector<geometry::Point2D>& points);
        geometry::Point2D calculateCentroid(const std::vector<geometry::Point2D>& points);
        bool isClockwise(const std::vector<geometry::Point2D>& points);
    }
}

// Template implementations (must be in header for template instantiation)
namespace contour::utilities
{
    template<typename Container>
    Contour createPolylineContour(const Container& points)
    {
        return createPolylineContour(std::begin(points), std::end(points));
    }

    template<typename Iterator>
    Contour createPolylineContour(Iterator begin, Iterator end)
    {
        std::vector<geometry::Point2D> pointVector(begin, end);
        return createPolylineContour(pointVector);
    }

    template<typename Container>
    Contour createPolylineContour(const Container& points, const PolylineOptions& options)
    {
        std::vector<geometry::Point2D> pointVector(std::begin(points), std::end(points));
        return createPolylineContour(pointVector, options);
    }
} 