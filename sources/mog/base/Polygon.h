#ifndef Polygon_h
#define Polygon_h

#include <vector>
#include "mog/base/Entity.h"
#include "mog/core/plain_objects.h"

namespace mog {
    class Polygon : public Entity {
    public:
        Point getMaxPosition();
        Point getMinPosition();

    protected:
        Polygon();
        void init(const std::vector<Point> &vertexPoints);
        virtual void bindVertices(const std::shared_ptr<Renderer> &renderer, int *verticesIdx, int *indicesIdx, bool bakeTransform = false) override;
        virtual void bindVertexTexCoords(const std::shared_ptr<Renderer> &renderer, int *idx, float x, float y, float w, float h) override;
        virtual std::shared_ptr<AABB> getAABB() override;

        std::vector<Point> vertexPoints;
        Point minPosition = Point::zero;
        Point maxPosition = Point::zero;
    };
}

#endif /* Polygon_h */
