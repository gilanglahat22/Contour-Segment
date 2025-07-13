#include "../include/Segment.h"
#include <stdexcept>
#include <cmath>
#include <limits> // Added for std::numeric_limits
#include <iostream> // Added for std::cerr

namespace contour
{
    // Base Segment class implementation
    bool Segment::isConnectedTo(const Segment& other, double epsilon) const
    {
        // Validate epsilon
        if (std::isnan(epsilon) || epsilon < 0) {
            return false;
        }
        
        try {
            return getEndPoint().isEqual(other.getStartPoint(), epsilon);
        } catch (const std::exception& e) {
            std::cerr << "Error in isConnectedTo: " << e.what() << std::endl;
            return false;
        }
    }

    // LineSegment implementation
    LineSegment::LineSegment(const geometry::Point2D& start, const geometry::Point2D& end)
        : m_start(start), m_end(end)
    {
        // Validate that start and end are different
        if (start.isEqual(end))
        {
            throw std::invalid_argument("LineSegment: start and end points cannot be the same");
        }
    }

    geometry::Point2D LineSegment::getStartPoint() const
    {
        // Validate point
        if (std::isnan(m_start.x) || std::isnan(m_start.y)) {
            return geometry::Point2D(std::numeric_limits<double>::quiet_NaN(), 
                                   std::numeric_limits<double>::quiet_NaN());
        }
        
        return m_start;
    }

    geometry::Point2D LineSegment::getEndPoint() const
    {
        // Validate point
        if (std::isnan(m_end.x) || std::isnan(m_end.y)) {
            return geometry::Point2D(std::numeric_limits<double>::quiet_NaN(), 
                                   std::numeric_limits<double>::quiet_NaN());
        }
        
        return m_end;
    }

    SegmentType LineSegment::getType() const
    {
        return SegmentType::Line;
    }

    std::unique_ptr<Segment> LineSegment::clone() const
    {
        return std::make_unique<LineSegment>(*this);
    }

    double LineSegment::getLength() const
    {
        // Validate points
        if (std::isnan(m_start.x) || std::isnan(m_start.y) || 
            std::isnan(m_end.x) || std::isnan(m_end.y)) {
            return std::numeric_limits<double>::quiet_NaN();
        }
        
        return m_start.distanceTo(m_end);
    }

    bool LineSegment::isEqual(const Segment& other, double epsilon) const
    {
        if (other.getType() != SegmentType::Line)
        {
            return false;
        }

        try {
            const auto& otherLine = dynamic_cast<const LineSegment&>(other);
            return m_start.isEqual(otherLine.m_start, epsilon) && 
                   m_end.isEqual(otherLine.m_end, epsilon);
        } catch (const std::bad_cast& e) {
            std::cerr << "Failed to cast to LineSegment: " << e.what() << std::endl;
            return false;
        }
    }

    geometry::Vector2D LineSegment::getDirection() const
    {
        // Validate points
        if (std::isnan(m_start.x) || std::isnan(m_start.y) || 
            std::isnan(m_end.x) || std::isnan(m_end.y)) {
            return geometry::Vector2D(std::numeric_limits<double>::quiet_NaN(), 
                                    std::numeric_limits<double>::quiet_NaN());
        }
        
        auto result = (m_end - m_start).normalized();
        
        // Check if normalization failed
        if (std::isnan(result.x) || std::isnan(result.y)) {
            return geometry::Vector2D(0.0, 0.0);
        }
        
        return result;
    }

    geometry::Point2D LineSegment::getPointAt(double t) const
    {
        if (t < 0.0 || t > 1.0)
        {
            throw std::invalid_argument("LineSegment::getPointAt: t must be in range [0,1]");
        }
        
        // Validate points
        if (std::isnan(m_start.x) || std::isnan(m_start.y) || 
            std::isnan(m_end.x) || std::isnan(m_end.y)) {
            return geometry::Point2D(std::numeric_limits<double>::quiet_NaN(), 
                                   std::numeric_limits<double>::quiet_NaN());
        }
        
        return m_start + (m_end - m_start) * t;
    }

    // ArcSegment implementation
    ArcSegment::ArcSegment(const geometry::Point2D& center, double radius, 
                           double startAngle, double endAngle, bool clockwise)
        : m_center(center), m_radius(radius), m_startAngle(startAngle), 
          m_endAngle(endAngle), m_clockwise(clockwise)
    {
        if (radius <= 0.0)
        {
            throw std::invalid_argument("ArcSegment: radius must be positive");
        }
        
        // Normalize angles to [0, 2π)
        m_startAngle = normalizeAngle(m_startAngle);
        m_endAngle = normalizeAngle(m_endAngle);
        
        // Check that we have a valid arc (not a full circle)
        if (geometry::isEqual(m_startAngle, m_endAngle))
        {
            throw std::invalid_argument("ArcSegment: start and end angles cannot be the same");
        }
    }

    geometry::Point2D ArcSegment::getStartPoint() const
    {
        // Validate arc parameters
        if (std::isnan(m_center.x) || std::isnan(m_center.y) || 
            std::isnan(m_radius) || m_radius <= 0 ||
            std::isnan(m_startAngle)) {
            return geometry::Point2D(std::numeric_limits<double>::quiet_NaN(), 
                                   std::numeric_limits<double>::quiet_NaN());
        }
        
        return pointFromAngle(m_startAngle);
    }

    geometry::Point2D ArcSegment::getEndPoint() const
    {
        // Validate arc parameters
        if (std::isnan(m_center.x) || std::isnan(m_center.y) || 
            std::isnan(m_radius) || m_radius <= 0 ||
            std::isnan(m_endAngle)) {
            return geometry::Point2D(std::numeric_limits<double>::quiet_NaN(), 
                                   std::numeric_limits<double>::quiet_NaN());
        }
        
        return pointFromAngle(m_endAngle);
    }

    SegmentType ArcSegment::getType() const
    {
        return SegmentType::Arc;
    }

    std::unique_ptr<Segment> ArcSegment::clone() const
    {
        return std::make_unique<ArcSegment>(*this);
    }

    double ArcSegment::getLength() const
    {
        // Validate radius and angle span
        if (std::isnan(m_radius) || m_radius <= 0) {
            return std::numeric_limits<double>::quiet_NaN();
        }
        
        double angleSpan = getAngleSpan();
        if (std::isnan(angleSpan)) {
            return std::numeric_limits<double>::quiet_NaN();
        }
        
        return m_radius * angleSpan;
    }

    bool ArcSegment::isEqual(const Segment& other, double epsilon) const
    {
        if (other.getType() != SegmentType::Arc)
        {
            return false;
        }

        try {
            const auto& otherArc = dynamic_cast<const ArcSegment&>(other);
            return m_center.isEqual(otherArc.m_center, epsilon) &&
                   geometry::isEqual(m_radius, otherArc.m_radius, epsilon) &&
                   geometry::isEqual(m_startAngle, otherArc.m_startAngle, epsilon) &&
                   geometry::isEqual(m_endAngle, otherArc.m_endAngle, epsilon) &&
                   m_clockwise == otherArc.m_clockwise;
        } catch (const std::bad_cast& e) {
            std::cerr << "Failed to cast to ArcSegment: " << e.what() << std::endl;
            return false;
        }
    }

    double ArcSegment::getAngleSpan() const
    {
        // Validate angles
        if (std::isnan(m_startAngle) || std::isnan(m_endAngle)) {
            return std::numeric_limits<double>::quiet_NaN();
        }
        
        double span;
        if (m_clockwise)
        {
            if (m_startAngle >= m_endAngle)
            {
                span = m_startAngle - m_endAngle;
            }
            else
            {
                span = m_startAngle + (2.0 * geometry::PI - m_endAngle);
            }
        }
        else // counter-clockwise
        {
            if (m_endAngle >= m_startAngle)
            {
                span = m_endAngle - m_startAngle;
            }
            else
            {
                span = m_endAngle + (2.0 * geometry::PI - m_startAngle);
            }
        }
        
        // Check for NaN result
        if (std::isnan(span)) {
            return std::numeric_limits<double>::quiet_NaN();
        }
        
        return span;
    }

    geometry::Point2D ArcSegment::getPointAt(double t) const
    {
        if (t < 0.0 || t > 1.0)
        {
            throw std::invalid_argument("ArcSegment::getPointAt: t must be in range [0,1]");
        }

        // Validate arc parameters
        if (std::isnan(m_center.x) || std::isnan(m_center.y) || 
            std::isnan(m_radius) || m_radius <= 0 ||
            std::isnan(m_startAngle) || std::isnan(m_endAngle)) {
            return geometry::Point2D(std::numeric_limits<double>::quiet_NaN(), 
                                   std::numeric_limits<double>::quiet_NaN());
        }

        double angle;
        if (m_clockwise)
        {
            angle = m_startAngle - t * getAngleSpan();
        }
        else
        {
            angle = m_startAngle + t * getAngleSpan();
        }
        
        return pointFromAngle(normalizeAngle(angle));
    }

    geometry::Point2D ArcSegment::pointFromAngle(double angle) const
    {
        // Validate inputs
        if (std::isnan(angle) || std::isnan(m_center.x) || std::isnan(m_center.y) || 
            std::isnan(m_radius) || m_radius <= 0) {
            return geometry::Point2D(std::numeric_limits<double>::quiet_NaN(), 
                                   std::numeric_limits<double>::quiet_NaN());
        }
        
        double cosAngle = std::cos(angle);
        double sinAngle = std::sin(angle);
        
        // Check for NaN results from trig functions
        if (std::isnan(cosAngle) || std::isnan(sinAngle)) {
            return geometry::Point2D(std::numeric_limits<double>::quiet_NaN(), 
                                   std::numeric_limits<double>::quiet_NaN());
        }
        
        return geometry::Point2D(
            m_center.x + m_radius * cosAngle,
            m_center.y + m_radius * sinAngle
        );
    }

    double ArcSegment::normalizeAngle(double angle) const
    {
        // Check for NaN input
        if (std::isnan(angle)) {
            return std::numeric_limits<double>::quiet_NaN();
        }
        
        while (angle < 0.0)
        {
            angle += 2.0 * geometry::PI;
        }
        while (angle >= 2.0 * geometry::PI)
        {
            angle -= 2.0 * geometry::PI;
        }
        return angle;
    }

    // Factory functions
    std::unique_ptr<Segment> createLineSegment(const geometry::Point2D& start, const geometry::Point2D& end)
    {
        return std::make_unique<LineSegment>(start, end);
    }

    std::unique_ptr<Segment> createArcSegment(const geometry::Point2D& center, double radius, 
                                              double startAngle, double endAngle, bool clockwise)
    {
        return std::make_unique<ArcSegment>(center, radius, startAngle, endAngle, clockwise);
    }
} 