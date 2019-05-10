#include "mog/base/Rectangle.h"

using namespace mog;

std::shared_ptr<Rectangle> Rectangle::create(const Size &size) {
    auto rectangle = std::shared_ptr<Rectangle>(new Rectangle());
    rectangle->init(size);
    return rectangle;
}

std::shared_ptr<Rectangle> Rectangle::create(float width, float height) {
    return Rectangle::create(Size(width, height));
}

void Rectangle::init(const Size &size) {
    std::vector<Point> vertexPoints;
    vertexPoints.emplace_back(Point(0, 0));
    vertexPoints.emplace_back(Point(0, size.height));
    vertexPoints.emplace_back(Point(size.width, 0));
    vertexPoints.emplace_back(size);
    Polygon::init(vertexPoints);
    
    this->transform->size = size;
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
