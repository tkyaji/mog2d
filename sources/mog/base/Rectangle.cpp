#include "mog/base/Rectangle.h"

using namespace mog;

std::shared_ptr<Rectangle> Rectangle::create(const Size &size) {
    auto rectangle = std::shared_ptr<Rectangle>(new Rectangle());
    rectangle->transform->size = size;
    rectangle->init();
    return rectangle;
}

std::shared_ptr<Rectangle> Rectangle::create(float width, float height) {
    return Rectangle::create(Size(width, height));
}

void Rectangle::init() {
    std::vector<Point> vertexPoints;
    vertexPoints.emplace_back(Point(0, 0));
    vertexPoints.emplace_back(Point(0, this->transform->size.height));
    vertexPoints.emplace_back(Point(this->transform->size.width, 0));
    vertexPoints.emplace_back(this->transform->size);
    this->vertexPoints = vertexPoints;
    Polygon::init();
}

std::shared_ptr<Collider> Rectangle::getCollider() {
    if (this->collider) return this->collider;
    this->collider = std::shared_ptr<Collider>(new Collider(ColliderShape::Rect));
    this->collider->aabb = this->getAABB();
    this->collider->obb = this->getOBB();
    return this->collider;
}

std::shared_ptr<Rectangle> Rectangle::clone() {
    auto e = this->cloneEntity();
    return std::static_pointer_cast<Rectangle>(e);
}

std::shared_ptr<Entity> Rectangle::cloneEntity() {
    auto rectangle = Rectangle::create(this->getSize());
    rectangle->copyProperties(std::static_pointer_cast<Entity>(shared_from_this()));
    return rectangle;
}
