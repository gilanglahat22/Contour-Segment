#include "../include/ContourVisualizer.h"
#include "../include/Segment.h"
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <limits>
#include <cstdlib>

namespace contour::visualization
{
    // ASCIICanvas implementation
    ASCIICanvas::ASCIICanvas(int width, int height, char backgroundChar)
        : m_width(width), m_height(height), m_backgroundChar(backgroundChar)
    {
        m_canvas.resize(m_height, std::vector<char>(m_width, backgroundChar));
    }

    void ASCIICanvas::setBounds(const geometry::Point2D& minBounds, const geometry::Point2D& maxBounds)
    {
        m_minBounds = minBounds;
        m_maxBounds = maxBounds;
    }

    void ASCIICanvas::clear()
    {
        for (auto& row : m_canvas)
        {
            std::fill(row.begin(), row.end(), m_backgroundChar);
        }
    }

    std::pair<int, int> ASCIICanvas::worldToCanvas(const geometry::Point2D& worldPoint) const
    {
        double rangeX = m_maxBounds.x - m_minBounds.x;
        double rangeY = m_maxBounds.y - m_minBounds.y;
        
        if (geometry::isEqual(rangeX, 0.0) || geometry::isEqual(rangeY, 0.0))
        {
            return {m_width / 2, m_height / 2};
        }
        
        double normalizedX = (worldPoint.x - m_minBounds.x) / rangeX;
        double normalizedY = (worldPoint.y - m_minBounds.y) / rangeY;
        
        int canvasX = static_cast<int>(normalizedX * (m_width - 1));
        int canvasY = static_cast<int>((1.0 - normalizedY) * (m_height - 1)); // Flip Y axis
        
        return {canvasX, canvasY};
    }

    void ASCIICanvas::setPixel(int x, int y, char character)
    {
        if (x >= 0 && x < m_width && y >= 0 && y < m_height)
        {
            m_canvas[y][x] = character;
        }
    }

    void ASCIICanvas::setPixel(const geometry::Point2D& worldPoint, char character)
    {
        auto [x, y] = worldToCanvas(worldPoint);
        setPixel(x, y, character);
    }

    void ASCIICanvas::drawLine(const geometry::Point2D& start, const geometry::Point2D& end, char character)
    {
        auto [x1, y1] = worldToCanvas(start);
        auto [x2, y2] = worldToCanvas(end);
        
        // Bresenham's line algorithm
        int dx = std::abs(x2 - x1);
        int dy = std::abs(y2 - y1);
        int sx = (x1 < x2) ? 1 : -1;
        int sy = (y1 < y2) ? 1 : -1;
        int err = dx - dy;
        
        int x = x1, y = y1;
        
        while (true)
        {
            setPixel(x, y, character);
            
            if (x == x2 && y == y2) break;
            
            int e2 = 2 * err;
            if (e2 > -dy)
            {
                err -= dy;
                x += sx;
            }
            if (e2 < dx)
            {
                err += dx;
                y += sy;
            }
        }
    }

    void ASCIICanvas::drawArc(const geometry::Point2D& center, double radius, 
                             double startAngle, double endAngle, bool clockwise, char character)
    {
        // Sample points along the arc
        double angleSpan;
        if (clockwise)
        {
            angleSpan = (startAngle >= endAngle) ? 
                (startAngle - endAngle) : (startAngle + 2.0 * geometry::PI - endAngle);
        }
        else
        {
            angleSpan = (endAngle >= startAngle) ? 
                (endAngle - startAngle) : (endAngle + 2.0 * geometry::PI - startAngle);
        }
        
        int numPoints = std::max(10, static_cast<int>(angleSpan * radius * 5)); // Adaptive sampling
        
        for (int i = 0; i <= numPoints; ++i)
        {
            double t = static_cast<double>(i) / numPoints;
            double angle;
            
            if (clockwise)
            {
                angle = startAngle - t * angleSpan;
            }
            else
            {
                angle = startAngle + t * angleSpan;
            }
            
            geometry::Point2D point(
                center.x + radius * std::cos(angle),
                center.y + radius * std::sin(angle)
            );
            
            setPixel(point, character);
        }
    }

    void ASCIICanvas::drawPoint(const geometry::Point2D& point, char character)
    {
        setPixel(point, character);
    }

    std::string ASCIICanvas::render() const
    {
        std::ostringstream oss;
        for (const auto& row : m_canvas)
        {
            for (char c : row)
            {
                oss << c;
            }
            oss << '\n';
        }
        return oss.str();
    }

    // ContourVisualizer implementation
    ContourVisualizer::ContourVisualizer(const VisualizationOptions& options)
        : m_options(options)
    {
    }

    std::pair<geometry::Point2D, geometry::Point2D> 
    ContourVisualizer::calculateBounds(const Contour& contour) const
    {
        if (contour.empty())
        {
            return {geometry::Point2D(0, 0), geometry::Point2D(1, 1)};
        }
        
        double minX = std::numeric_limits<double>::max();
        double minY = std::numeric_limits<double>::max();
        double maxX = std::numeric_limits<double>::lowest();
        double maxY = std::numeric_limits<double>::lowest();
        
        for (size_t i = 0; i < contour.size(); ++i)
        {
            const auto& segment = contour[i];
            
            // Check start and end points
            auto start = segment.getStartPoint();
            auto end = segment.getEndPoint();
            
            minX = std::min({minX, start.x, end.x});
            minY = std::min({minY, start.y, end.y});
            maxX = std::max({maxX, start.x, end.x});
            maxY = std::max({maxY, start.y, end.y});
            
            // For arcs, also consider the bounding box of the full arc
            if (segment.getType() == SegmentType::Arc)
            {
                const auto& arc = static_cast<const ArcSegment&>(segment);
                auto center = arc.getCenter();
                double radius = arc.getRadius();
                
                minX = std::min(minX, center.x - radius);
                minY = std::min(minY, center.y - radius);
                maxX = std::max(maxX, center.x + radius);
                maxY = std::max(maxY, center.y + radius);
            }
        }
        
        return {geometry::Point2D(minX, minY), geometry::Point2D(maxX, maxY)};
    }

    std::pair<geometry::Point2D, geometry::Point2D>
    ContourVisualizer::calculateBounds(const std::vector<const Contour*>& contours) const
    {
        if (contours.empty())
        {
            return {geometry::Point2D(0, 0), geometry::Point2D(1, 1)};
        }
        
        auto [minBounds, maxBounds] = calculateBounds(*contours[0]);
        
        for (size_t i = 1; i < contours.size(); ++i)
        {
            auto [thisMin, thisMax] = calculateBounds(*contours[i]);
            minBounds.x = std::min(minBounds.x, thisMin.x);
            minBounds.y = std::min(minBounds.y, thisMin.y);
            maxBounds.x = std::max(maxBounds.x, thisMax.x);
            maxBounds.y = std::max(maxBounds.y, thisMax.y);
        }
        
        return {minBounds, maxBounds};
    }

    std::pair<geometry::Point2D, geometry::Point2D>
    ContourVisualizer::applyMargin(const geometry::Point2D& minBounds, const geometry::Point2D& maxBounds) const
    {
        double rangeX = maxBounds.x - minBounds.x;
        double rangeY = maxBounds.y - minBounds.y;
        
        double marginX = rangeX * m_options.margin;
        double marginY = rangeY * m_options.margin;
        
        return {
            geometry::Point2D(minBounds.x - marginX, minBounds.y - marginY),
            geometry::Point2D(maxBounds.x + marginX, maxBounds.y + marginY)
        };
    }

    std::string ContourVisualizer::visualizeContour(const Contour& contour) const
    {
        std::ostringstream result;
        
        // Create canvas
        ASCIICanvas canvas(m_options.width, m_options.height, m_options.backgroundChar);
        
        // Calculate and set bounds
        auto [minBounds, maxBounds] = calculateBounds(contour);
        auto [adjustedMin, adjustedMax] = applyMargin(minBounds, maxBounds);
        canvas.setBounds(adjustedMin, adjustedMax);
        
        // Draw segments
        for (size_t i = 0; i < contour.size(); ++i)
        {
            const auto& segment = contour[i];
            
            if (segment.getType() == SegmentType::Line)
            {
                canvas.drawLine(segment.getStartPoint(), segment.getEndPoint(), m_options.lineChar);
            }
            else if (segment.getType() == SegmentType::Arc)
            {
                const auto& arc = static_cast<const ArcSegment&>(segment);
                canvas.drawArc(arc.getCenter(), arc.getRadius(), 
                              arc.getStartAngle(), arc.getEndAngle(), 
                              arc.isClockwise(), m_options.arcChar);
            }
            
            // Mark connection points
            canvas.drawPoint(segment.getStartPoint(), m_options.vertexChar);
            canvas.drawPoint(segment.getEndPoint(), m_options.vertexChar);
        }
        
        // Mark start and end if not closed
        if (!contour.empty() && !contour.isClosed())
        {
            canvas.drawPoint(contour[0].getStartPoint(), m_options.startChar);
            canvas.drawPoint(contour[contour.size()-1].getEndPoint(), m_options.endChar);
        }
        
        // Generate title
        result << utils::drawBox(m_options.width, 3, "Contour Visualization");
        result << "\n";
        
        // Add coordinate info if requested
        if (m_options.showCoordinates)
        {
            result << "Bounds: " << utils::formatPoint(adjustedMin) 
                   << " to " << utils::formatPoint(adjustedMax) << "\n";
            result << "Valid: " << (contour.isValid() ? "YES" : "NO") 
                   << " | Closed: " << (contour.isClosed() ? "YES" : "NO")
                   << " | Segments: " << contour.size() << "\n\n";
        }
        
        // Render canvas
        result << canvas.render();
        
        return result.str();
    }

    std::string ContourVisualizer::visualizeContours(const std::vector<const Contour*>& contours) const
    {
        std::ostringstream result;
        
        if (contours.empty())
        {
            return "No contours to visualize.\n";
        }
        
        // Create canvas
        ASCIICanvas canvas(m_options.width, m_options.height, m_options.backgroundChar);
        
        // Calculate bounds for all contours
        auto [minBounds, maxBounds] = calculateBounds(contours);
        auto [adjustedMin, adjustedMax] = applyMargin(minBounds, maxBounds);
        canvas.setBounds(adjustedMin, adjustedMax);
        
        // Draw each contour with different characters
        char segmentChars[] = {'*', 'o', '+', 'x', '%', '@', '&', '='};
        
        for (size_t contourIndex = 0; contourIndex < contours.size(); ++contourIndex)
        {
            const auto& contour = *contours[contourIndex];
            char currentChar = segmentChars[contourIndex % 8];
            
            for (size_t i = 0; i < contour.size(); ++i)
            {
                const auto& segment = contour[i];
                
                if (segment.getType() == SegmentType::Line)
                {
                    canvas.drawLine(segment.getStartPoint(), segment.getEndPoint(), currentChar);
                }
                else if (segment.getType() == SegmentType::Arc)
                {
                    const auto& arc = static_cast<const ArcSegment&>(segment);
                    canvas.drawArc(arc.getCenter(), arc.getRadius(), 
                                  arc.getStartAngle(), arc.getEndAngle(), 
                                  arc.isClockwise(), currentChar);
                }
            }
        }
        
        // Generate title and legend
        result << utils::drawBox(m_options.width, 3, "Multiple Contours Visualization");
        result << "\nLegend: ";
        for (size_t i = 0; i < contours.size() && i < 8; ++i)
        {
            result << "Contour" << i << "='" << segmentChars[i] << "' ";
        }
        result << "\n\n";
        
        // Render canvas
        result << canvas.render();
        
        return result.str();
    }

    std::string ContourVisualizer::visualizeContours(const std::vector<std::unique_ptr<Contour>>& contours) const
    {
        std::vector<const Contour*> contourPtrs;
        for (const auto& contour : contours)
        {
            contourPtrs.push_back(contour.get());
        }
        return visualizeContours(contourPtrs);
    }

    std::string ContourVisualizer::generateCoordinateInfo(const Contour& contour) const
    {
        std::ostringstream result;
        
        result << "=== Coordinate Information ===\n";
        for (size_t i = 0; i < contour.size(); ++i)
        {
            const auto& segment = contour[i];
            result << "Segment " << i << " (" 
                   << (segment.getType() == SegmentType::Line ? "Line" : "Arc") << "): ";
            result << utils::formatPoint(segment.getStartPoint()) << " -> " 
                   << utils::formatPoint(segment.getEndPoint());
            result << " (Length: " << std::fixed << std::setprecision(2) 
                   << segment.getLength() << ")\n";
        }
        
        return result.str();
    }

    std::string ContourVisualizer::generateContourTable(const Contour& contour) const
    {
        std::ostringstream result;
        
        result << "┌────────────────────────────────────┐\n";
        result << "│           Contour Properties       │\n";
        result << "├────────────────────────────────────┤\n";
        result << "│ Segments:     " << std::setw(17) << contour.size() << " │\n";
        result << "│ Valid:        " << std::setw(17) << (contour.isValid() ? "YES" : "NO") << " │\n";
        result << "│ Closed:       " << std::setw(17) << (contour.isClosed() ? "YES" : "NO") << " │\n";
        result << "│ Total Length: " << std::setw(17) << std::fixed << std::setprecision(2) 
               << contour.getTotalLength() << " │\n";
        result << "└────────────────────────────────────┘\n";
        
        return result.str();
    }

    // Utility functions implementation
    namespace utils
    {
        std::string drawBox(int width, int height, const std::string& title)
        {
            std::ostringstream result;
            
            // Top border
            result << "┌";
            for (int i = 0; i < width - 2; ++i) result << "─";
            result << "┐\n";
            
            // Title line
            if (!title.empty() && height > 2)
            {
                result << "│";
                int padding = (width - 2 - static_cast<int>(title.length())) / 2;
                for (int i = 0; i < padding; ++i) result << " ";
                result << title;
                for (int i = padding + static_cast<int>(title.length()); i < width - 2; ++i) result << " ";
                result << "│\n";
                height--;
            }
            
            // Middle lines
            for (int i = 1; i < height - 1; ++i)
            {
                result << "│";
                for (int j = 0; j < width - 2; ++j) result << " ";
                result << "│\n";
            }
            
            // Bottom border
            result << "└";
            for (int i = 0; i < width - 2; ++i) result << "─";
            result << "┘";
            
            return result.str();
        }

        std::string formatPoint(const geometry::Point2D& point, int precision)
        {
            std::ostringstream oss;
            oss << "(" << std::fixed << std::setprecision(precision) 
                << point.x << "," << point.y << ")";
            return oss.str();
        }

        std::string formatAngle(double radians, int precision)
        {
            double degrees = radians * 180.0 / geometry::PI;
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(precision) << degrees << "°";
            return oss.str();
        }
    }
} 