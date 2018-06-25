#include "mog/base/Rectangle.h"

using namespace mog;

std::shared_ptr<Rectangle> Rectangle::create(const Size &size) {
    auto rectangle = shared_ptr<Rectangle>(new Rectangle());
    rectangle->init(size);
    return rectangle;
}

std::shared_ptr<Rectangle> Rectangle::create(float width, float height) {
    return Rectangle::create(Size(width, height));
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
    
    this->transform->size = size;
}
