#include "mog/base/RoundedRectangle.h"
#include "mog/core/Engine.h"
#include "mog/core/Density.h"
#include <math.h>

using namespace mog;

shared_ptr<RoundedRectangle> RoundedRectangle::create(const Size &size, float cornerRadius) {
    auto rectangle = shared_ptr<RoundedRectangle>(new RoundedRectangle());
    rectangle->init(size, cornerRadius);
    return rectangle;
}

shared_ptr<RoundedRectangle> RoundedRectangle::create(float width, float height, float cornerRadius) {
    return RoundedRectangle::create(Size(width, height), cornerRadius);
}

RoundedRectangle::RoundedRectangle() {
}

float RoundedRectangle::getCornerRadius() {
    return this->cornerRadius;
}

void RoundedRectangle::init(const Size &size, float cornerRadius) {
    this->cornerRadius = cornerRadius;
    this->transform->size = size;
    
    vector<Point> vertexPoints;
    int n = max((int)(cornerRadius * 0.375f), 8);
    
    float r1 = 270.0f * (M_PI / 180.0f);
    float r2 = 180.0f * (M_PI / 180.0f);
    float _r = (r2 - r1) / n;
    for (int i = 0; i < n+1; i++) {
        float r = r1 + _r * i;
        float y = cornerRadius * sin(r) + cornerRadius;
        float x1 = cornerRadius * cos(r) + cornerRadius;
        float x2 = size.width - x1;
        vertexPoints.emplace_back(Point(x1, y));
        vertexPoints.emplace_back(Point(x2, y));
    }
    for (int i = n; i >= 0; i--) {
        float r = r1 + _r * i;
        float y = size.height - (cornerRadius * sin(r) + cornerRadius);
        float x1 = cornerRadius * cos(r) + cornerRadius;
        float x2 = size.width - x1;
        vertexPoints.emplace_back(Point(x1, y));
        vertexPoints.emplace_back(Point(x2, y));
    }

    Polygon::init(vertexPoints);
}





