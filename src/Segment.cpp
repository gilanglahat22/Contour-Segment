#include "../include/Segment.h"
#include <stdexcept>
#include <cmath>

namespace contour
{
    // Base Segment class implementation
    bool Segment::isConnectedTo(const Segment& other, double epsilon) const
    {
        return getEndPoint().isEqual(other.getStartPoint(), epsilon);
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
        return m_start;
    }

    geometry::Point2D LineSegment::getEndPoint() const
    {
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
        return m_start.distanceTo(m_end);
    }

    bool LineSegment::isEqual(const Segment& other, double epsilon) const
    {
        if (other.getType() != SegmentType::Line)
        {
            return false;
        }

        const auto& otherLine = static_cast<const LineSegment&>(other);
        return m_start.isEqual(otherLine.m_start, epsilon) && 
               m_end.isEqual(otherLine.m_end, epsilon);
    }

    geometry::Vector2D LineSegment::getDirection() const
    {
        return (m_end - m_start).normalized();
    }

    geometry::Point2D LineSegment::getPointAt(double t) const
    {
        if (t < 0.0 || t > 1.0)
        {
            throw std::invalid_argument("LineSegment::getPointAt: t must be in range [0,1]");
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
        return pointFromAngle(m_startAngle);
    }

    geometry::Point2D ArcSegment::getEndPoint() const
    {
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
        return m_radius * getAngleSpan();
    }

    bool ArcSegment::isEqual(const Segment& other, double epsilon) const
    {
        if (other.getType() != SegmentType::Arc)
        {
            return false;
        }

        const auto& otherArc = static_cast<const ArcSegment&>(other);
        return m_center.isEqual(otherArc.m_center, epsilon) &&
               geometry::isEqual(m_radius, otherArc.m_radius, epsilon) &&
               geometry::isEqual(m_startAngle, otherArc.m_startAngle, epsilon) &&
               geometry::isEqual(m_endAngle, otherArc.m_endAngle, epsilon) &&
               m_clockwise == otherArc.m_clockwise;
    }

    double ArcSegment::getAngleSpan() const
    {
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
        return span;
    }

    geometry::Point2D ArcSegment::getPointAt(double t) const
    {
        if (t < 0.0 || t > 1.0)
        {
            throw std::invalid_argument("ArcSegment::getPointAt: t must be in range [0,1]");
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
        return geometry::Point2D(
            m_center.x + m_radius * std::cos(angle),
            m_center.y + m_radius * std::sin(angle)
        );
    }

    double ArcSegment::normalizeAngle(double angle) const
    {
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