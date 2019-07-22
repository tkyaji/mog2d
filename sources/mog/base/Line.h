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
        friend class EntityCreator;
    public:
        static std::shared_ptr<Line> create(const std::vector<Point> &points, float lineWidth = 10.0f, LineType lineType = LineType::LineStrip);
        std::vector<Point> getPoints();
        float getLineWidth();
        std::shared_ptr<Line> clone();
        virtual std::shared_ptr<Dictionary> serialize() override;

    protected:
        Line() {}
        
        virtual void init() override;
        std::vector<Point> createLinesVertexPoints(const std::vector<Point> &points, float lineWidth);
        std::vector<Point> createLineStripVertexPoints(const std::vector<Point> &points, float lineWidth);
        std::vector<Point> createLineLoopVertexPoints(const std::vector<Point> &points, float lineWidth);
        virtual std::shared_ptr<Entity> cloneEntity() override;
        virtual void deserializeData(const std::shared_ptr<Dictionary> &dict, const std::unordered_map<std::string, std::unordered_map<std::string, std::shared_ptr<Data>>> &params) override;

        float lineWidth = 0;
        LineType lineType = LineType::LineStrip;
        std::vector<Point> points;
    };
}

#endif /* Line_h */
