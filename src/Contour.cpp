#include "../include/Contour.h"
#include <stdexcept>
#include <algorithm>
#include <utility>

namespace contour
{
    // Constructor with segments
    Contour::Contour(SegmentContainer segments) : m_segments(std::move(segments))
    {
        // No need to validate here, let isValid() handle validation when called
    }

    // Copy constructor - deep copy
    Contour::Contour(const Contour& other)
    {
        deepCopyFrom(other);
    }

    // Copy assignment operator - deep copy
    Contour& Contour::operator=(const Contour& other)
    {
        if (this != &other)
        {
            deepCopyFrom(other);
        }
        return *this;
    }

    // Segment management
    void Contour::addSegment(std::unique_ptr<Segment> segment)
    {
        if (!segment)
        {
            throw std::invalid_argument("Contour::addSegment: segment cannot be null");
        }
        m_segments.push_back(std::move(segment));
        invalidateCache();
    }

    void Contour::insertSegment(size_type index, std::unique_ptr<Segment> segment)
    {
        if (!segment)
        {
            throw std::invalid_argument("Contour::insertSegment: segment cannot be null");
        }
        if (index > m_segments.size())
        {
            throw std::out_of_range("Contour::insertSegment: index out of range");
        }
        m_segments.insert(m_segments.begin() + index, std::move(segment));
        invalidateCache();
    }

    void Contour::removeSegment(size_type index)
    {
        if (index >= m_segments.size())
        {
            throw std::out_of_range("Contour::removeSegment: index out of range");
        }
        m_segments.erase(m_segments.begin() + index);
        invalidateCache();
    }

    void Contour::clear()
    {
        m_segments.clear();
        invalidateCache();
    }

    // Access methods
    Segment& Contour::operator[](size_type index)
    {
        invalidateCache(); // Non-const access might modify the segment
        return *m_segments[index];
    }

    const Segment& Contour::operator[](size_type index) const
    {
        return *m_segments[index];
    }

    Segment& Contour::at(size_type index)
    {
        if (index >= m_segments.size())
        {
            throw std::out_of_range("Contour::at: index out of range");
        }
        invalidateCache(); // Non-const access might modify the segment
        return *m_segments[index];
    }

    const Segment& Contour::at(size_type index) const
    {
        if (index >= m_segments.size())
        {
            throw std::out_of_range("Contour::at: index out of range");
        }
        return *m_segments[index];
    }

    // Validation with caching
    bool Contour::isValid(double epsilon) const
    {
        // Check if cache is valid for this epsilon
        if (m_validityCache.has_value() && geometry::isEqual(m_lastEpsilon, epsilon))
        {
            return m_validityCache.value();
        }

        // Calculate validity and cache result
        bool isValid = calculateValidity(epsilon);
        m_validityCache = isValid;
        m_lastEpsilon = epsilon;
        
        return isValid;
    }

    // Utility methods
    double Contour::getTotalLength() const
    {
        double totalLength = 0.0;
        for (const auto& segment : m_segments)
        {
            totalLength += segment->getLength();
        }
        return totalLength;
    }

    std::vector<geometry::Point2D> Contour::getVertices() const
    {
        std::vector<geometry::Point2D> vertices;
        if (m_segments.empty())
        {
            return vertices;
        }

        // Add start point of first segment
        vertices.push_back(m_segments[0]->getStartPoint());

        // Add end points of all segments
        for (const auto& segment : m_segments)
        {
            vertices.push_back(segment->getEndPoint());
        }

        return vertices;
    }

    bool Contour::isClosed(double epsilon) const
    {
        if (m_segments.empty())
        {
            return false;
        }

        const auto& firstStart = m_segments[0]->getStartPoint();
        const auto& lastEnd = m_segments.back()->getEndPoint();
        
        return firstStart.isEqual(lastEnd, epsilon);
    }

    bool Contour::isEqual(const Contour& other, double epsilon) const
    {
        if (m_segments.size() != other.m_segments.size())
        {
            return false;
        }

        for (size_type i = 0; i < m_segments.size(); ++i)
        {
            if (!m_segments[i]->isEqual(*other.m_segments[i], epsilon))
            {
                return false;
            }
        }

        return true;
    }

    // Private helper methods
    void Contour::invalidateCache() const
    {
        m_validityCache.reset();
        m_lastEpsilon = -1.0;
    }

    bool Contour::calculateValidity(double epsilon) const
    {
        // Empty contour is considered invalid
        if (m_segments.empty())
        {
            return false;
        }

        // Single segment is always valid
        if (m_segments.size() == 1)
        {
            return true;
        }

        // Check that all segments are sequentially connected end-to-begin
        for (size_type i = 0; i < m_segments.size() - 1; ++i)
        {
            if (!m_segments[i]->isConnectedTo(*m_segments[i + 1], epsilon))
            {
                return false;
            }
        }

        return true;
    }

    void Contour::deepCopyFrom(const Contour& other)
    {
        m_segments.clear();
        m_segments.reserve(other.m_segments.size());
        
        for (const auto& segment : other.m_segments)
        {
            m_segments.push_back(segment->clone());
        }

        // Don't copy cache - let it be recalculated when needed
        invalidateCache();
    }

    // Global operators
    bool operator==(const Contour& lhs, const Contour& rhs)
    {
        return lhs.isEqual(rhs);
    }

    bool operator!=(const Contour& lhs, const Contour& rhs)
    {
        return !lhs.isEqual(rhs);
    }

    // ContourBuilder implementation
    ContourBuilder& ContourBuilder::addLine(const geometry::Point2D& start, const geometry::Point2D& end)
    {
        m_contour.addSegment(createLineSegment(start, end));
        return *this;
    }

    ContourBuilder& ContourBuilder::addArc(const geometry::Point2D& center, double radius, 
                                          double startAngle, double endAngle, bool clockwise)
    {
        m_contour.addSegment(createArcSegment(center, radius, startAngle, endAngle, clockwise));
        return *this;
    }
} 