#pragma once

#include "Segment.h"
#include "Geometry.h"
#include <vector>
#include <memory>
#include <optional>

namespace contour
{
    class Contour
    {
    public:
        // Type aliases for clarity
        using SegmentContainer = std::vector<std::unique_ptr<Segment>>;
        using iterator = SegmentContainer::iterator;
        using const_iterator = SegmentContainer::const_iterator;
        using size_type = SegmentContainer::size_type;

        // Constructors and destructor
        Contour() = default;
        explicit Contour(SegmentContainer segments);
        ~Contour() = default;

        // Copy constructor and assignment (deep copy)
        Contour(const Contour& other);
        Contour& operator=(const Contour& other);

        // Move constructor and assignment
        Contour(Contour&&) noexcept = default;
        Contour& operator=(Contour&&) noexcept = default;

        // Segment management
        void addSegment(std::unique_ptr<Segment> segment);
        void insertSegment(size_type index, std::unique_ptr<Segment> segment);
        void removeSegment(size_type index);
        void clear();

        // Access methods
        size_type size() const { return m_segments.size(); }
        bool empty() const { return m_segments.empty(); }
        
        Segment& operator[](size_type index);
        const Segment& operator[](size_type index) const;
        
        Segment& at(size_type index);
        const Segment& at(size_type index) const;

        // Iterator support for range-based loops and STL algorithms
        iterator begin() { invalidateCache(); return m_segments.begin(); }
        const_iterator begin() const { return m_segments.begin(); }
        const_iterator cbegin() const { return m_segments.begin(); }
        
        iterator end() { invalidateCache(); return m_segments.end(); }
        const_iterator end() const { return m_segments.end(); }
        const_iterator cend() const { return m_segments.end(); }

        // Validation with caching
        bool isValid(double epsilon = geometry::EPSILON) const;

        // Utility methods
        double getTotalLength() const;
        std::vector<geometry::Point2D> getVertices() const;
        bool isClosed(double epsilon = geometry::EPSILON) const;

        // Comparison
        bool isEqual(const Contour& other, double epsilon = geometry::EPSILON) const;

    private:
        SegmentContainer m_segments;
        
        // Caching for isValid() - mutable to allow modification in const methods
        mutable std::optional<bool> m_validityCache;
        mutable double m_lastEpsilon = -1.0; // Track epsilon used for caching
        
        // Helper methods
        void invalidateCache() const;
        bool calculateValidity(double epsilon) const;
        void deepCopyFrom(const Contour& other);
    };

    // Global operators
    bool operator==(const Contour& lhs, const Contour& rhs);
    bool operator!=(const Contour& lhs, const Contour& rhs);

    // Contour builder for convenience
    class ContourBuilder
    {
    private:
        Contour m_contour;

    public:
        ContourBuilder& addLine(const geometry::Point2D& start, const geometry::Point2D& end);
        ContourBuilder& addArc(const geometry::Point2D& center, double radius, 
                              double startAngle, double endAngle, bool clockwise = false);
        
        Contour build() && { return std::move(m_contour); }
        const Contour& get() const { return m_contour; }
    };
} 