#ifndef Collision_h
#define Collision_h

#include "mog/core/plain_objects.h"
#include "mog/core/Transform.h"
#include <memory>
#include <array>

namespace mog {
    class Entity;
    
    enum class ColliderShape {
        Rect,
        Circle,
        Polygon,
        Polygons,
    };

    
    class Vec2 {
    public:
        float x = 0.0f;
        float y = 0.0f;
        float vLen = 0.0f;
        
        Vec2();
        Vec2(float x, float y, float vLen);
        Vec2(float x, float y);
        const Vec2 operator*(float v) const;
        const Vec2 operator/(float v) const;
    };

    
    class AABB {
    public:
        float minX;
        float maxX;
        float minY;
        float maxY;
        
        AABB(float minX, float minY, float maxX, float maxY);
    };
    
    
    class OBB {
    public:
        Vec2 vec1;
        Vec2 vec2;
        float centerX;
        float centerY;
        
        OBB(const Vec2 &vec1, const Vec2 &vec2, float centerX, float centerY);
    };
    
    
    class POLYGON {
    public:
        Point *points = nullptr;
        int length = 0;
        POLYGON(Point *points, int length);
        ~POLYGON();
    };

    
    class CIRCLE {
    public:
        Point center;
        float radius;
        
        CIRCLE(float x, float y, float radius);
    };
    
    
    class Collider {
    public:
        ColliderShape shape;
        std::shared_ptr<AABB> aabb;
        std::shared_ptr<OBB> obb;
        std::shared_ptr<CIRCLE> circle;
        std::shared_ptr<POLYGON> polygon;

        Collider(ColliderShape shape);
    };

    
    class Collision {
    public:
        static bool collides(const std::shared_ptr<Collider> &col1, const std::shared_ptr<Collider> &col2);
        static bool collides(const std::shared_ptr<Collider> &col1, const Point &p);
        
        static bool aabb_aabb(const AABB &aabb1, const AABB &aabb2);
        static bool aabb_point(const AABB &aabb, const Point &p);

        static bool obb_obb(const OBB &obb1, const OBB &obb2);
        static bool obb_circle(const OBB &obb, const CIRCLE &circle);
        static bool obb_polygon(const OBB &obb, const POLYGON &polygon);
        static bool obb_point(const OBB &obb, const Point &p);

        static bool circle_circle(const CIRCLE &circle1, const CIRCLE &circle2);
        static bool circle_polygon(const CIRCLE &circle, const POLYGON &polygon);
        static bool circle_point(const CIRCLE &circle, const Point &p);

        static bool polygon_polygon(const POLYGON &polygon1, const POLYGON &polygon2);
        static bool polygon_point(const POLYGON &polygon, const Point &p);
    };
}

#endif /* Collision_h */
