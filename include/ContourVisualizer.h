#pragma once

#include "Contour.h"
#include "Geometry.h"
#include <string>
#include <vector>
#include <sstream>

namespace contour
{
    namespace visualization
    {
        // Visualization configuration
        struct VisualizationOptions
        {
            int width = 80;           // Canvas width in characters
            int height = 24;          // Canvas height in characters
            char backgroundChar = '.'; // Background character
            char lineChar = '*';      // Line segment character
            char arcChar = 'o';       // Arc segment character
            char vertexChar = '#';    // Vertex/connection point character
            char startChar = 'S';     // Start point character
            char endChar = 'E';       // End point character
            bool showCoordinates = true; // Show coordinate labels
            bool showGrid = false;    // Show coordinate grid
            double margin = 0.1;      // Margin as percentage of range
        };

        // Canvas for ASCII art rendering
        class ASCIICanvas
        {
        private:
            std::vector<std::vector<char>> m_canvas;
            int m_width;
            int m_height;
            geometry::Point2D m_minBounds;
            geometry::Point2D m_maxBounds;
            char m_backgroundChar;

        public:
            ASCIICanvas(int width, int height, char backgroundChar = '.');
            
            void setBounds(const geometry::Point2D& minBounds, const geometry::Point2D& maxBounds);
            void clear();
            
            // Convert world coordinates to canvas coordinates
            std::pair<int, int> worldToCanvas(const geometry::Point2D& worldPoint) const;
            
            // Drawing functions
            void setPixel(int x, int y, char character);
            void setPixel(const geometry::Point2D& worldPoint, char character);
            void drawLine(const geometry::Point2D& start, const geometry::Point2D& end, char character);
            void drawArc(const geometry::Point2D& center, double radius, 
                        double startAngle, double endAngle, bool clockwise, char character);
            void drawPoint(const geometry::Point2D& point, char character);
            
            // Render to string
            std::string render() const;
        };

        // Main visualizer class
        class ContourVisualizer
        {
        private:
            VisualizationOptions m_options;
            
            // Calculate bounds for a contour
            std::pair<geometry::Point2D, geometry::Point2D> calculateBounds(const Contour& contour) const;
            std::pair<geometry::Point2D, geometry::Point2D> calculateBounds(const std::vector<const Contour*>& contours) const;
            
            // Apply margin to bounds
            std::pair<geometry::Point2D, geometry::Point2D> applyMargin(
                const geometry::Point2D& minBounds, const geometry::Point2D& maxBounds) const;

        public:
            ContourVisualizer(const VisualizationOptions& options = VisualizationOptions{});
            
            // Set visualization options
            void setOptions(const VisualizationOptions& options) { m_options = options; }
            const VisualizationOptions& getOptions() const { return m_options; }
            
            // Visualize single contour
            std::string visualizeContour(const Contour& contour) const;
            
            // Visualize multiple contours
            std::string visualizeContours(const std::vector<const Contour*>& contours) const;
            std::string visualizeContours(const std::vector<std::unique_ptr<Contour>>& contours) const;
            
            // Visualize individual segments
            std::string visualizeSegment(const Segment& segment) const;
            
            // Generate coordinate information
            std::string generateCoordinateInfo(const Contour& contour) const;
            std::string generateSegmentInfo(const Contour& contour) const;
            
            // Generate ASCII table for contour properties
            std::string generateContourTable(const Contour& contour) const;
            std::string generateContoursComparison(const std::vector<const Contour*>& contours) const;
        };

        // Utility functions
        namespace utils
        {
            // Generate simple box drawing
            std::string drawBox(int width, int height, const std::string& title = "");
            
            // Generate coordinate grid
            std::string generateGrid(const geometry::Point2D& minBounds, 
                                   const geometry::Point2D& maxBounds, int gridSize = 10);
            
            // Format point as string
            std::string formatPoint(const geometry::Point2D& point, int precision = 2);
            
            // Format angle in degrees
            std::string formatAngle(double radians, int precision = 1);
            
            // Color codes for console (if supported)
            namespace colors
            {
                const std::string RESET = "\033[0m";
                const std::string RED = "\033[31m";
                const std::string GREEN = "\033[32m";
                const std::string YELLOW = "\033[33m";
                const std::string BLUE = "\033[34m";
                const std::string MAGENTA = "\033[35m";
                const std::string CYAN = "\033[36m";
                const std::string WHITE = "\033[37m";
            }
        }
    }
} 