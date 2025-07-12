#pragma once

#include "Geometry.h"
#include <memory>
#include <variant>

namespace contour
{
    // Forward declarations
    class Segment;
    class LineSegment;
    class ArcSegment;

    // Segment types enum for extensibility
    enum class SegmentType
    {
        Line,
        Arc
        // Future segment types can be added here
    };

    // Base abstract class for all segments
    class Segment
    {
    public:
        virtual ~Segment() = default;

        // Pure virtual functions that must be implemented by derived classes
        virtual geometry::Point2D getStartPoint() const = 0;
        virtual geometry::Point2D getEndPoint() const = 0;
        virtual SegmentType getType() const = 0;
        virtual std::unique_ptr<Segment> clone() const = 0;
        virtual double getLength() const = 0;

        // Common functionality
        bool isConnectedTo(const Segment& other, double epsilon = geometry::EPSILON) const;
        virtual bool isEqual(const Segment& other, double epsilon = geometry::EPSILON) const = 0;

    protected:
        // Protected constructor to prevent direct instantiation
        Segment() = default;
        Segment(const Segment&) = default;
        Segment& operator=(const Segment&) = default;
        Segment(Segment&&) noexcept = default;
        Segment& operator=(Segment&&) noexcept = default;
    };

    // Line segment class
    class LineSegment : public Segment
    {
    private:
        geometry::Point2D m_start;
        geometry::Point2D m_end;

    public:
        LineSegment(const geometry::Point2D& start, const geometry::Point2D& end);
        
        // Copy constructor and assignment
        LineSegment(const LineSegment&) = default;
        LineSegment& operator=(const LineSegment&) = default;

        // Move constructor and assignment
        LineSegment(LineSegment&&) noexcept = default;
        LineSegment& operator=(LineSegment&&) noexcept = default;

        // Implement pure virtual functions
        geometry::Point2D getStartPoint() const override;
        geometry::Point2D getEndPoint() const override;
        SegmentType getType() const override;
        std::unique_ptr<Segment> clone() const override;
        double getLength() const override;
        bool isEqual(const Segment& other, double epsilon = geometry::EPSILON) const override;

        // Line-specific methods
        geometry::Vector2D getDirection() const;
        geometry::Point2D getPointAt(double t) const; // t in [0,1]
    };

    // Arc segment class
    class ArcSegment : public Segment
    {
    private:
        geometry::Point2D m_center;
        double m_radius;
        double m_startAngle; // in radians
        double m_endAngle;   // in radians
        bool m_clockwise;

    public:
        ArcSegment(const geometry::Point2D& center, double radius, 
                   double startAngle, double endAngle, bool clockwise = false);
        
        // Copy constructor and assignment
        ArcSegment(const ArcSegment&) = default;
        ArcSegment& operator=(const ArcSegment&) = default;

        // Move constructor and assignment
        ArcSegment(ArcSegment&&) noexcept = default;
        ArcSegment& operator=(ArcSegment&&) noexcept = default;

        // Implement pure virtual functions
        geometry::Point2D getStartPoint() const override;
        geometry::Point2D getEndPoint() const override;
        SegmentType getType() const override;
        std::unique_ptr<Segment> clone() const override;
        double getLength() const override;
        bool isEqual(const Segment& other, double epsilon = geometry::EPSILON) const override;

        // Arc-specific methods
        geometry::Point2D getCenter() const { return m_center; }
        double getRadius() const { return m_radius; }
        double getStartAngle() const { return m_startAngle; }
        double getEndAngle() const { return m_endAngle; }
        bool isClockwise() const { return m_clockwise; }
        double getAngleSpan() const;
        geometry::Point2D getPointAt(double t) const; // t in [0,1]

    private:
        geometry::Point2D pointFromAngle(double angle) const;
        double normalizeAngle(double angle) const;
    };

    // Type-safe variant for holding different segment types
    using SegmentVariant = std::variant<LineSegment, ArcSegment>;

    // Factory functions for creating segments
    std::unique_ptr<Segment> createLineSegment(const geometry::Point2D& start, const geometry::Point2D& end);
    std::unique_ptr<Segment> createArcSegment(const geometry::Point2D& center, double radius, 
                                              double startAngle, double endAngle, bool clockwise = false);
} 