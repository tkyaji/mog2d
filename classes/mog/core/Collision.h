#ifndef Collision_h
#define Collision_h

#include "mog/core/plain_objects.h"
#include "mog/core/Transform.h"
#include <memory>

using namespace std;

namespace mog {
    class Entity;
    
    enum class ColliderShape {
        Rect,
        Circle,
    };

    
    class AABB {
    public:
        float minX;
        float maxX;
        float minY;
        float maxY;
        
        AABB(float minX, float minY, float maxX, float maxY);
    };
    
    
    class Vec2 {
    public:
        float x;
        float y;
        float vLen;
        
        Vec2(float x, float y, float vLen = 1);
        const Vec2 operator*(float v) const;
    };

    
    class OBB {
    public:
        Vec2 vec1;
        Vec2 vec2;
        float centerX;
        float centerY;
        
        OBB(const Vec2 &vec1, const Vec2 &vec2, float centerX, float centerY);
    };
    

    class CIRCLE {
    public:
        float x;
        float y;
        float radius;
        
        CIRCLE(float x, float y, float radius);
    };
    
    
    class Collider {
    public:
        ColliderShape shape;
        shared_ptr<AABB> aabb;
        shared_ptr<OBB> obb;
        shared_ptr<CIRCLE> circle;
        
        Collider(ColliderShape shape);
    };

    
    class Collision {
    public:
        static bool collides(const shared_ptr<Collider> &col1, const shared_ptr<Collider> &col2);
        static bool collides(const shared_ptr<Collider> &col1, const Point &p);
        
    private:
        static bool aabb_aabb(const AABB &aabb1, const AABB &aabb2);
        static bool obb_obb(const OBB &obb1, const OBB &obb2);
        static bool aabb_circle(const AABB &aabb, const CIRCLE &circle);
        static bool circle_circle(const CIRCLE &circle1, const CIRCLE &circle2);
        static bool aabb_point(const AABB &aabb, const Point &p);
        static bool circle_point(const CIRCLE &circle, const Point &p);
        static bool obb_point(const OBB &obb, const Point &p);
    };
}

#endif /* Collision_h */
