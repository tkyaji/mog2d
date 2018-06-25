#ifndef Triangle_h
#define Triangle_h

#include <memory>
#include <array>
#include "mog/base/Polygon.h"
#include "mog/core/plain_objects.h"
#include "mog/core/Collision.h"

namespace mog {
    class Triangle : public Polygon {
    public:
        static std::shared_ptr<Triangle> create(const Point &p1, const Point &p2, const Point &p3);
        static std::shared_ptr<Triangle> create(float x1, float y1, float x2, float y2, float x3, float y3);
        std::array<Point, 3> getPoints();
        
    protected:
        Triangle();
        
        std::array<Point, 3> points;
        void init(const Point &p1, const Point &p2, const Point &p3);
        shared_ptr<POLYGON> getPOLYGON();
        virtual shared_ptr<Collider> getCollider() override;
    };
}


#endif /* Triangle_h */
