#ifndef Line_h
#define Line_h

#include <memory>
#include <vector>
#include "mog/base/Polygon.h"
#include "mog/core/plain_objects.h"

namespace mog {
    
    enum class LineType {
        Lines,
        LineStrip,
        LineLoop,
    };
    
    class Line : public Polygon {
    public:
        static std::shared_ptr<Line> create(const std::vector<Point> &points, float lineWidth = 10.0f, LineType lineType = LineType::LineStrip);
        std::vector<Point> getPoints();
        float getLineWidth();

    protected:
        Line();
        void init(const std::vector<Point> &points, float lineWidth, LineType lineType = LineType::LineStrip);
        std::vector<Point> createLinesVertexPoints(const std::vector<Point> &points, float lineWidth);
        std::vector<Point> createLineStripVertexPoints(const std::vector<Point> &points, float lineWidth);
        std::vector<Point> createLineLoopVertexPoints(const std::vector<Point> &points, float lineWidth);

        float lineWidth = 0;
        LineType lineType = LineType::LineStrip;
        std::vector<Point> points;
    };
}

#endif /* Line_h */
