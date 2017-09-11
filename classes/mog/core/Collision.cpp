#include "mog/core/Collision.h"
#include "mog/core/plain_objects.h"
#include "mog/base/Entity.h"
#include "mog/base/Circle.h"
#include <math.h>

using namespace mog;

float dot(const Vec2 &v1, const Vec2 &v2) {
    return v1.x * v1.vLen * v2.x * v2.vLen + v1.y * v1.vLen * v2.y * v2.vLen;
}

float cross(const Vec2 &v1, const Vec2 &v2) {
    return v1.x * v1.vLen * v2.y * v2.vLen - v2.x * v2.vLen * v1.y * v1.vLen;
}

Vec2::Vec2(float x, float y, float vLen) {
    this->x = x;
    this->y = y;
    this->vLen = vLen;
}

const Vec2 Vec2::operator*(float v) const {
    return Vec2(this->x * v, this->y * v, this->vLen * v);
}


AABB::AABB(float minX, float minY, float maxX, float maxY) {
    this->minX = minX;
    this->minY = minY;
    this->maxX = maxX;
    this->maxY = maxY;
}


OBB::OBB(const Vec2 &vec1, const Vec2 &vec2, float centerX, float centerY) : vec1(vec1), vec2(vec2) {
    this->centerX = centerX;
    this->centerY = centerY;
}


CIRCLE::CIRCLE(float x, float y, float radius) {
    this->x = x;
    this->y = y;
    this->radius = radius;
}


Collider::Collider(ColliderShape shape) {
    this->shape = shape;
}


bool Collision::collides(const shared_ptr<Collider> &col1, const shared_ptr<Collider> &col2) {
    if (!Collision::aabb_aabb(*col1->aabb.get(), *col2->aabb.get())) return false;
    
    if (col1->shape == ColliderShape::Circle && col2->shape == ColliderShape::Rect) {
        // TODO: rotation
        return Collision::aabb_circle(*col2->aabb.get(), *col1->circle.get());
        
    } else if (col1->shape == ColliderShape::Rect && col2->shape == ColliderShape::Circle) {
        // TODO: rotation
        return Collision::aabb_circle(*col1->aabb.get(), *col2->circle.get());
        
    } else if (col1->shape == ColliderShape::Circle && col2->shape == ColliderShape::Circle) {
        return Collision::circle_circle(*col1->circle.get(), *col2->circle.get());
        
    } else {
        if (fabs(col1->obb->vec1.y) < 0.000001 && fabs(col2->obb->vec1.y) < 0.000001) return true;
        return Collision::obb_obb(*col1->obb.get(), *col2->obb.get());
    }
}

bool Collision::collides(const shared_ptr<Collider> &col, const Point &p) {
    if (col->shape == ColliderShape::Circle) {
        return Collision::circle_point(*col->circle.get(), p);
    } else {
        if (!Collision::aabb_point(*col->aabb.get(), p)) return false;
        if (fabs(col->obb->vec1.y) < 0.000001) return true;
        return Collision::obb_point(*col->obb.get(), p);
    }
}

bool Collision::aabb_aabb(const AABB &aabb1, const AABB &aabb2) {
    if (aabb1.minX < aabb2.maxX && aabb1.maxX > aabb2.minX &&
        aabb1.minY < aabb2.maxY && aabb1.maxY > aabb2.minY) {
        return true;
    }
    return false;
}

bool check_obb_obb(const Vec2 &_vl, const Vec2 &vea, const Vec2 &veb, const Vec2 &interval) {
    Vec2 vl = Vec2(_vl.x, _vl.y, 1);
    float a = fabs(dot(vl, vea) + fabs(dot(vl, veb)));
    float b = veb.vLen;
    float l = fabs(dot(interval, vl));
    
    if (l > a + b) {
        return true;
    }
    return false;
}

bool Collision::obb_obb(const OBB &obb1, const OBB &obb2) {
    Vec2 interval = Vec2(fabs(obb1.centerX - obb2.centerX), fabs(obb1.centerY - obb2.centerY), 1);
    
    Vec2 obb1vec1 = Vec2(obb1.vec1.x, obb1.vec1.y, obb1.vec1.vLen * 0.5f);
    Vec2 obb1vec2 = Vec2(obb1.vec2.x, obb1.vec2.y, obb1.vec2.vLen * 0.5f);
    Vec2 obb2vec1 = Vec2(obb2.vec1.x, obb2.vec1.y, obb2.vec1.vLen * 0.5f);
    Vec2 obb2vec2 = Vec2(obb2.vec2.x, obb2.vec2.y, obb2.vec2.vLen * 0.5f);
    
    if (check_obb_obb(obb1vec1, obb2vec1, obb2vec2, interval)) {
        return false;
    } else if (check_obb_obb(obb1vec2, obb2vec1, obb2vec2, interval)) {
        return false;
    } else if (check_obb_obb(obb2vec1, obb1vec1, obb1vec2, interval)) {
        return false;
    } else if (check_obb_obb(obb2vec2, obb1vec1, obb1vec2, interval)) {
        return false;
    }
    return true;
}

bool Collision::aabb_circle(const AABB &aabb, const CIRCLE &circle) {
    Point p = Point(circle.x, circle.y);
    if (Collision::aabb_point(aabb, p)) {
        return true;
    }
    
    if (circle.x < aabb.minX) {
        p.x = aabb.minX;
    } else if (circle.x > aabb.maxX) {
        p.x = aabb.maxX;
    }
    
    if (circle.y < aabb.minY) {
        p.y = aabb.minY;
    } else if (circle.y > aabb.maxY) {
        p.y = aabb.maxY;
    }
    
    Point dp = Point(fabs(circle.x - p.x), fabs(circle.y - p.y));
    return sqrt(dp.x * dp.x + dp.y * dp.y) <= circle.radius;
}

bool Collision::circle_circle(const CIRCLE &circle1, const CIRCLE &circle2) {
    auto dp = Point(fabs(circle1.x - circle2.x), fabs(circle1.y - circle2.y));
    return sqrt(dp.x * dp.x + dp.y * dp.y) <= circle1.radius + circle2.radius;
}

bool Collision::aabb_point(const AABB &aabb, const Point &p) {
    return p.x >= aabb.minX && p.y >= aabb.minY && p.x <= aabb.maxX && p.y <= aabb.maxY;
}

bool Collision::circle_point(const CIRCLE &circle, const Point &p) {
    return sqrt(pow(circle.x - p.x, 2) + pow(circle.y - p.y, 2)) <= circle.radius;
}

bool Collision::obb_point(const OBB &obb, const Point &p) {
    float x = obb.centerX - (obb.vec1.x * obb.vec1.vLen * 0.5f + obb.vec2.x * obb.vec2.vLen * 0.5f);
    float y = obb.centerY - (obb.vec1.y * obb.vec1.vLen * 0.5f + obb.vec2.y * obb.vec2.vLen * 0.5f);
    
    Vec2 v1 = Vec2(obb.vec1.x * obb.vec1.vLen, obb.vec1.y * obb.vec1.vLen);
    Vec2 v2 = Vec2(obb.vec2.x * obb.vec2.vLen, obb.vec2.y * obb.vec2.vLen);
    Vec2 v3 = Vec2(v1.x * -1, v1.y * -1);
    Vec2 v4 = Vec2(v2.x * -1, v2.y * -1);
    
    Vec2 va = Vec2(p.x - x, p.y - y);
    if (cross(v1, va) < 0) {
        return false;
    }
    
    x += obb.vec1.x * obb.vec1.vLen;
    y += obb.vec1.y * obb.vec1.vLen;
    Vec2 vb = Vec2(p.x - x, p.y - y);
    if (cross(v2, vb) < 0) {
        return false;
    }
    
    x += obb.vec2.x * obb.vec2.vLen;
    y += obb.vec2.y * obb.vec2.vLen;
    Vec2 vc = Vec2(p.x - x, p.y - y);
    if (cross(v3, vc) < 0) {
        return false;
    }
    
    x -= obb.vec1.x * obb.vec1.vLen;
    y -= obb.vec1.y * obb.vec1.vLen;
    Vec2 vd = Vec2(p.x - x, p.y - y);
    if (cross(v4, vd) < 0){
        return false;
    }
    
    return true;
}
