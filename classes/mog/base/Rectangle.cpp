#include "mog/base/Rectangle.h"

using namespace mog;

shared_ptr<Rectangle> Rectangle::create(const Size &size) {
    auto rectangle = shared_ptr<Rectangle>(new Rectangle());
    rectangle->init(size);
    return rectangle;
}

Rectangle::Rectangle() {
}

void Rectangle::init(const Size &size) {
    vector<Point> vertexPoints;
    vertexPoints.emplace_back(Point(0, 0));
    vertexPoints.emplace_back(Point(0, size.height));
    vertexPoints.emplace_back(Point(size.width, 0));
    vertexPoints.emplace_back(size);
    Polygon::init(vertexPoints);
    
    this->transform->size.width = size.width;
    this->transform->size.height = size.height;
}

shared_ptr<Rectangle> Rectangle::clone() {
    auto entity = this->cloneEntity();
    return static_pointer_cast<Rectangle>(entity);
}

shared_ptr<Entity> Rectangle::cloneEntity() {
    auto rectangle = shared_ptr<Rectangle>(new Rectangle());
    rectangle->copyFrom(shared_from_this());
    return rectangle;
}
