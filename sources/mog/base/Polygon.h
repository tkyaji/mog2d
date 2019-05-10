#ifndef Polygon_h
#define Polygon_h

#include <vector>
#include "mog/base/Entity.h"
#include "mog/core/plain_objects.h"

namespace mog {
    class Polygon : public Entity {
    public:
        static std::shared_ptr<Polygon> create(const std::vector<Point> &vertexPoints);
        
        template<class First, class... Rest>
        static std::shared_ptr<Polygon> create(const First& first, const Rest&... rest) {
            auto polygon = std::shared_ptr<Polygon>(new Polygon());
            std::vector<Point> vertexPoints;
            addPoint(vertexPoints, first, rest...);
            polygon->init(vertexPoints);
            return polygon;
        }

        std::vector<Point> getPoints();
        virtual std::shared_ptr<Collider> getCollider() override;

    protected:
        void init(const std::vector<Point> &vertexPoints);
        virtual void bindVertices(const std::shared_ptr<Renderer> &renderer, int *verticesIdx, int *indicesIdx, bool bakeTransform = false) override;
        virtual void bindVertexTexCoords(const std::shared_ptr<Renderer> &renderer, int *idx, int texIdx, float x, float y, float w, float h) override;
        virtual std::shared_ptr<AABB> getAABB() override;
        virtual std::shared_ptr<POLYGON> getPOLYGON();
        virtual std::shared_ptr<Entity> cloneEntity() override;

        std::vector<Point> vertexPoints;
        Point minPosition = Point::zero;
        Point maxPosition = Point::zero;
        
    private:
        template<class First, class... Rest>
        static void addPoint(std::vector<Point> &vertexPoints, const First& first, const Rest&... rest) {
            vertexPoints.emplace_back(first);
            addPoint(vertexPoints, rest...);
        }
        static void addPoint(std::vector<Point> &vertexPoints) {}
    };
}

#endif /* Polygon_h */
