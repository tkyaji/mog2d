#include "mog/core/Collision.h"
#include "mog/core/plain_objects.h"
#include "mog/Constants.h"
#include <math.h>

using namespace mog;

static float dot(const Vec2 &v1, const Vec2 &v2) {
    return v1.x * v1.vLen * v2.x * v2.vLen + v1.y * v1.vLen * v2.y * v2.vLen;
}

static float dot(const Vec2 &v1, const Point &p2) {
    return v1.x * v1.vLen * p2.x + v1.y * v1.vLen * p2.y;
}

static float cross(const Vec2 &v1, const Vec2 &v2) {
    return v1.x * v1.vLen * v2.y * v2.vLen - v2.x * v2.vLen * v1.y * v1.vLen;
}

Vec2::Vec2() {
}

Vec2::Vec2(float x, float y, float vLen) {
    this->x = x;
    this->y = y;
    this->vLen = vLen;
}

Vec2::Vec2(float x, float y) {
    this->vLen = sqrt(x * x + y * y);
    this->x = x / this->vLen;
    this->y = y / this->vLen;
}

const Vec2 Vec2::operator*(float v) const {
    return Vec2(this->x * v, this->y * v, this->vLen * v);
}

const Vec2 Vec2::operator/(float v) const {
    return Vec2(this->x / v, this->y / v, this->vLen / v);
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
    this->center = Point(x, y);
    this->radius = radius;
}

POLYGON::POLYGON(Point *points, int length) {
    this->points = points;
    this->length = length;
}

POLYGON::~POLYGON() {
    mogfree(this->points);
}

Point POLYGON::getCentroid() {
    if (this->centroid.x != 0 || this->centroid.y != 0) {
        return this->centroid;
    }
    float suma = 0;
    Point sumcp = Point::zero;
    for (int i = 1; i < length - 1; i++) {
        Point &p0 = points[0];
        Point &p1 = points[i];
        Point &p2 = points[i + 1];
        
        float a = fabs(((p1.x - p0.x) * (p2.y - p0.y) - (p1.y - p0.y) * (p2.x - p0.x)) / 2.0f);
        Point cp = Point((p0.x + p1.x + p2.x) / 3.0f, (p0.y + p1.y + p2.y) / 3.0f);
        
        suma += a;
        sumcp += cp * a;
    }
    this->centroid = Point(sumcp.x / suma, sumcp.y / suma);
    return this->centroid;
}

POLYGONS::POLYGONS(POLYGON *polygons, int length) {
    this->polygons = polygons;
    this->length = length;
}

POLYGONS::~POLYGONS() {
    mogfree(this->polygons);
}

Collider::Collider(ColliderShape shape) {
    this->shape = shape;
}

Point Collider::getCentroid() {
    switch (this->shape) {
        case ColliderShape::Rect:
            return Point(this->obb->centerX, this->obb->centerY);
        case ColliderShape::Circle:
            return this->circle->center;
        case ColliderShape::Polygon:
            return this->polygon->getCentroid();
        case ColliderShape::Polygons:
        default:
            return Point::zero;
    }
}

bool Collision::collides(const std::shared_ptr<Collider> &col1, const std::shared_ptr<Collider> &col2) {
    if (!Collision::aabb_aabb(*col1->aabb.get(), *col2->aabb.get())) return false;
    
    switch (col1->shape) {
        case ColliderShape::Rect:
            switch (col2->shape) {
                case ColliderShape::Rect:
                    return Collision::obb_obb(*col1->obb.get(), *col2->obb.get());
                case ColliderShape::Circle:
                    return Collision::obb_circle(*col1->obb.get(), *col2->circle.get());
                case ColliderShape::Polygon:
                    return Collision::obb_polygon(*col1->obb.get(), *col2->polygon.get());
                case ColliderShape::Polygons:
                    return Collision::obb_polygons(*col1->obb.get(), *col2->polygons.get());
            }
        case ColliderShape::Circle:
            switch (col2->shape) {
                case ColliderShape::Rect:
                    return Collision::obb_circle(*col2->obb.get(), *col1->circle.get());
                case ColliderShape::Circle:
                    return Collision::circle_circle(*col1->circle.get(), *col2->circle.get());
                case ColliderShape::Polygon:
                    return Collision::circle_polygon(*col1->circle.get(), *col2->polygon.get());
                case ColliderShape::Polygons:
                    return Collision::circle_polygons(*col1->circle.get(), *col2->polygons.get());
            }
        case ColliderShape::Polygon:
            switch (col2->shape) {
                case ColliderShape::Rect:
                    return Collision::obb_polygon(*col2->obb.get(), *col1->polygon.get());
                case ColliderShape::Circle:
                    return Collision::circle_polygon(*col2->circle.get(), *col1->polygon.get());
                case ColliderShape::Polygon:
                    return Collision::polygon_polygon(*col1->polygon.get(), *col2->polygon.get());
                case ColliderShape::Polygons:
                    return Collision::polygon_polygons(*col1->polygon.get(), *col2->polygons.get());
            }
        case ColliderShape::Polygons:
            switch (col2->shape) {
                case ColliderShape::Rect:
                    return Collision::obb_polygons(*col2->obb.get(), *col1->polygons.get());
                case ColliderShape::Circle:
                    return Collision::circle_polygons(*col2->circle.get(), *col1->polygons.get());
                case ColliderShape::Polygon:
                    return Collision::polygon_polygons(*col2->polygon.get(), *col1->polygons.get());
                case ColliderShape::Polygons:
                    return Collision::polygons_polygons(*col1->polygons.get(), *col2->polygons.get());
            }
    }
    return false;
}

bool Collision::collides(const std::shared_ptr<Collider> &col, const Point &p) {
    if (!Collision::aabb_point(*col->aabb.get(), p)) return false;
    
    switch (col->shape) {
        case ColliderShape::Rect:
            if (fabs(col->obb->vec1.y) < 0.000001) return true;
            return Collision::obb_point(*col->obb.get(), p);
        case ColliderShape::Circle:
            return circle_point(*col->circle, p);
        case ColliderShape::Polygon:
            return polygon_point(*col->polygon, p);
        case ColliderShape::Polygons:
            return polygons_point(*col->polygons, p);
    }
    return false;
}

bool Collision::aabb_aabb(const AABB &aabb1, const AABB &aabb2) {
    if (aabb1.minX < aabb2.maxX && aabb1.maxX > aabb2.minX &&
        aabb1.minY < aabb2.maxY && aabb1.maxY > aabb2.minY) {
        return true;
    }
    return false;
}

bool Collision::aabb_point(const AABB &aabb, const Point &p) {
    return p.x >= aabb.minX && p.y >= aabb.minY && p.x <= aabb.maxX && p.y <= aabb.maxY;
}

bool check_obb_obb(const Vec2 &vl, const OBB &obb, const Vec2 &interval) {
    Vec2 nvl = Vec2(vl.x, vl.y, 1);
    float a = fabs(dot(obb.vec1, nvl) + fabs(dot(obb.vec2, nvl)));
    float b = vl.vLen;
    float l = fabs(dot(interval, nvl));
    
    if (l < a + b) {
        return true;
    }
    return false;
}

bool Collision::obb_obb(const OBB &obb1, const OBB &obb2) {
    Vec2 interval = Vec2(fabs(obb1.centerX - obb2.centerX), fabs(obb1.centerY - obb2.centerY), 1);
    
    if (!check_obb_obb(obb1.vec1, obb2, interval)) {
        return false;
    } else if (!check_obb_obb(obb1.vec2, obb2, interval)) {
        return false;
    } else if (!check_obb_obb(obb2.vec1, obb1, interval)) {
        return false;
    } else if (!check_obb_obb(obb2.vec2, obb1, interval)) {
        return false;
    }
    return true;
}

static POLYGON obb_to_polygon(const OBB &obb) {
    float x1 = obb.centerX - obb.vec1.x * obb.vec1.vLen - obb.vec2.x * obb.vec2.vLen;
    float y1 = obb.centerY - obb.vec1.y * obb.vec1.vLen - obb.vec2.y * obb.vec2.vLen;
    float x2 = x1 + obb.vec1.x * obb.vec1.vLen * 2.0f;
    float y2 = y1 + obb.vec1.y * obb.vec1.vLen * 2.0f;
    float x3 = x2 + obb.vec2.x * obb.vec2.vLen * 2.0f;
    float y3 = y2 + obb.vec2.y * obb.vec2.vLen * 2.0f;
    float x4 = x3 + obb.vec1.x * obb.vec1.vLen * -2.0f;
    float y4 = y3 + obb.vec1.y * obb.vec1.vLen * -2.0f;
    Point *points = (Point *)mogmalloc(sizeof(Point) * 3);
    points[0] = Point(x1, y1);
    points[1] = Point(x2, y2);
    points[2] = Point(x3, y3);
    points[3] = Point(x4, y4);
    return POLYGON(points, 4);
}

bool Collision::obb_circle(const OBB &obb, const CIRCLE &circle) {
    POLYGON polygon1 = obb_to_polygon(obb);
    return Collision::circle_polygon(circle, polygon1);
}

bool Collision::obb_polygon(const OBB &obb, const POLYGON &polygon) {
    POLYGON polygon1 = obb_to_polygon(obb);
    return Collision::polygon_polygon(polygon1, polygon);
}

bool Collision::obb_polygons(const OBB &obb, const POLYGONS &polygons) {
    for (int i = 0; i < polygons.length; i++) {
        if (Collision::obb_polygon(obb, polygons.polygons[i])) {
            return true;
        }
    }
    return false;
}

bool Collision::obb_point(const OBB &obb, const Point &p) {
    POLYGON polygon = obb_to_polygon(obb);
    return polygon_point(polygon, p);
}

bool Collision::circle_circle(const CIRCLE &circle1, const CIRCLE &circle2) {
    auto dp = Point(fabs(circle1.center.x - circle2.center.x), fabs(circle1.center.y - circle2.center.y));
    return sqrt(dp.x * dp.x + dp.y * dp.y) <= circle1.radius + circle2.radius;
}

bool Collision::circle_point(const CIRCLE &circle, const Point &p) {
    return sqrt(pow(circle.center.x - p.x, 2) + pow(circle.center.y - p.y, 2)) <= circle.radius;
}

bool Collision::circle_polygon(const CIRCLE &circle, const POLYGON &polygon) {
    if (polygon_point(polygon, circle.center)) return true;
    
    for (int i = 0; i < polygon.length; i++) {
        int ix = i + 1;
        if (ix >= polygon.length) ix -= polygon.length;
        
        auto p1 = polygon.points[i];
        auto p2 = polygon.points[ix];
        
        if (circle_point(circle, p1)) return true;
        if (circle_point(circle, p2)) return true;
        
        Vec2 v1 = Vec2(p2.x - p1.x, p2.y - p1.y);
        Vec2 v2 = Vec2(circle.center.x - p1.x, circle.center.y - p1.y);
        
        Vec2 v3 = Vec2(p1.x - p2.x, p1.y - p2.y);
        Vec2 v4 = Vec2(circle.center.x - p2.x, circle.center.y - p2.y);

        float d1 = dot(Vec2(v1.x, v1.y, 1.0f), Vec2(v2.x, v2.y, 1.0f));
        float d2 = dot(Vec2(v3.x, v3.y, 1.0f), Vec2(v4.x, v4.y, 1.0f));
        if (d1 >= 0 && d2 >= 0) {
            float distance = fabs(cross(v1, v2)) / v1.vLen;
            if (distance <= circle.radius) return true;
        }
    }
    
    return false;
}

bool Collision::circle_polygons(const CIRCLE &circle, const POLYGONS &polygons) {
    for (int i = 0; i < polygons.length; i++) {
        if (Collision::circle_polygon(circle, polygons.polygons[i])) {
            return true;
        }
    }
    return false;
}

static bool check_polygon_polygon(const Vec2 &spv, const POLYGON &polygon1, const POLYGON &polygon2) {
    float s1min = 0;
    float s1max = 0;
    for (int i = 0; i < polygon1.length; i++) {
        int ix = i + 1;
        if (ix >= polygon1.length) ix -= polygon1.length;
        
        float a = dot(spv, polygon1.points[i]);
        float b = dot(spv, polygon1.points[ix]);
        if (i == 0) {
            s1min = a;
            s1max = a;
        }
        if (a < b) {
            s1min = fmin(s1min, a);
            s1max = fmax(s1max, b);
        } else {
            s1min = fmin(s1min, b);
            s1max = fmax(s1max, a);
        }
    }
    
    float s2min = 0;
    float s2max = 0;
    for (int i = 0; i < polygon2.length; i++) {
        int ix = i + 1;
        if (ix >= polygon2.length) ix -= polygon2.length;
        
        float a = dot(spv, polygon2.points[i]);
        float b = dot(spv, polygon2.points[ix]);
        if (i == 0) {
            s2min = a;
            s2max = a;
        }
        if (a < b) {
            s2min = fmin(s2min, a);
            s2max = fmax(s2max, b);
        } else {
            s2min = fmin(s2min, b);
            s2max = fmax(s2max, a);
        }
    }
    
    if ((s2min <= s1min && s1min <= s2max) ||
        (s2min <= s1max && s1max <= s2max) ||
        (s1min <= s2min && s2min <= s1max) ||
        (s1min <= s2max && s2max <= s1max)) {
        return true;
    }
    
    return false;
}

bool Collision::polygon_polygon(const POLYGON &polygon1, const POLYGON &polygon2) {
    for (int i = 0; i < polygon1.length; i++) {
        int ix = i + 1;
        if (ix >= polygon1.length) ix -= polygon1.length;
        
        auto p1 = polygon1.points[i];
        auto p2 = polygon1.points[ix];
        Vec2 v = Vec2(p2.x - p1.x, p2.y - p1.y);
        Vec2 spv = Vec2(v.y, -v.x, 1);
        if (!check_polygon_polygon(spv, polygon1, polygon2)) {
            return false;
        }
    }
    for (int i = 0; i < polygon2.length; i++) {
        int ix = i + 1;
        if (ix >= polygon2.length) ix -= polygon2.length;
        
        auto p1 = polygon2.points[i];
        auto p2 = polygon2.points[ix];
        Vec2 v = Vec2(p2.x - p1.x, p2.y - p1.y);
        Vec2 spv = Vec2(v.y, -v.x, 1);
        if (!check_polygon_polygon(spv, polygon1, polygon2)) {
            return false;
        }
    }
    return true;
}

bool Collision::polygon_polygons(const POLYGON &polygon, const POLYGONS &polygons) {
    for (int i = 0; i < polygons.length; i++) {
        if (Collision::polygon_polygon(polygon, polygons.polygons[i])) {
            return true;
        }
    }
    return false;
}

bool Collision::polygon_point(const POLYGON &polygon, const Point &p) {
    int s = 0;
    for (int i = 0; i < polygon.length; i++) {
        int ix = i + 1;
        if (ix >= polygon.length) ix -= polygon.length;
        Point p1 = polygon.points[i];
        Point p2 = polygon.points[ix];
        Vec2 v1 = Vec2(p2.x - p1.x, p2.y - p1.y);
        Vec2 v2 = Vec2(p.x - p1.x, p.y - p1.y);
        float c = cross(v1, v2);
        if (s == -1 && c > 0) return false;
        if (s == 1 && c < 0) return false;
        s = (c < 0) ? -1 : 1;
    }
    return true;
}

bool Collision::polygons_polygons(const POLYGONS &polygons1, const POLYGONS &polygons2) {
    for (int i1 = 0; i1 < polygons1.length; i1++) {
        for (int i2 = 0; i2 < polygons2.length; i2++) {
            if (Collision::polygon_polygon(polygons1.polygons[i1], polygons2.polygons[i2])) {
                return true;
            }
        }
    }
    return false;
}

bool Collision::polygons_point(const POLYGONS &polygons, const Point &p) {
    for (int i = 0; i < polygons.length; i++) {
        if (Collision::polygon_point(polygons.polygons[i], p)) {
            return true;
        }
    }
    return false;
}
