#include "mog/base/Triangle.h"
#include "mog/Constants.h"

using namespace mog;

std::shared_ptr<Triangle> Triangle::create(const Point &p1, const Point &p2, const Point &p3) {
    auto triangle = std::shared_ptr<Triangle>(new Triangle());
    triangle->init(p1, p2, p3);
    return triangle;
}

std::shared_ptr<Triangle> Triangle::create(float x1, float y1, float x2, float y2, float x3, float y3) {
    auto triangle = std::shared_ptr<Triangle>(new Triangle());
    triangle->init(Point(x1, y1), Point(x2, y2), Point(x3, y3));
    return triangle;
}

Triangle::Triangle() {
}

std::array<Point, 3> Triangle::getPoints() {
    return this->points;
}

void Triangle::init(const Point &p1, const Point &p2, const Point &p3) {
    this->points[0] = p1;
    this->points[1] = p2;
    this->points[2] = p3;
    std::vector<Point> vertexPoints = {p1, p2, p3};
    Polygon::init(vertexPoints);
}

shared_ptr<POLYGON> Triangle::getPOLYGON() {
    auto offset = Point(this->matrix[12], this->matrix[13]);
    auto v1 = Point(this->matrix[0], this->matrix[1]);
    auto v2 = Point(this->matrix[4], this->matrix[5]);
    auto p1 = this->points[0];
    auto p2 = this->points[1];
    auto p3 = this->points[2];
    Point *points = (Point *)rpmalloc(sizeof(Point) * 3);
    points[0] = v1 * p1.x + v2 * p1.y + offset;
    points[1] = v1 * p2.x + v2 * p2.y + offset;
    points[2] = v1 * p3.x + v2 * p3.y + offset;
    return shared_ptr<POLYGON>(new POLYGON(points, 3));
}

shared_ptr<Collider> Triangle::getCollider() {
    auto collider = shared_ptr<Collider>(new Collider(ColliderShape::Polygon));
    collider->aabb = this->getAABB();
    collider->polygon = this->getPOLYGON();
    return collider;
}
