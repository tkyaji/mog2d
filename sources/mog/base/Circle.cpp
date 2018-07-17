#include "mog/base/Circle.h"
#include "mog/core/Engine.h"
#include "mog/core/Device.h"
#include <math.h>

using namespace mog;

shared_ptr<Circle> Circle::create(float radius) {
    auto circle = shared_ptr<Circle>(new Circle());
    circle->init(radius);
    return circle;
}

Circle::Circle() {
}

void Circle::init(float radius) {
    this->radius = radius;
    this->transform->size = Size(radius * 2, radius * 2);
    
    vector<Point> vertexPoints;
    int n = max((int)(radius * 1.5f), 30);
    if (n % 2 == 1) n++;
    for (int i = 0; i < n/2; i++) {
        float x1 = radius * cos(2.0 * M_PI * (float)i/n);
        float y1 = radius * sin(2.0 * M_PI * (float)i/n);
        vertexPoints.emplace_back(Point(x1 + radius, y1 + radius));
        float x2 = radius * cos(2.0 * M_PI * (float)(n-i-1)/n);
        float y2 = radius * sin(2.0 * M_PI * (float)(n-i-1)/n);
        vertexPoints.emplace_back(Point(x2 + radius, y2 + radius));
    }
    Polygon::init(vertexPoints);
}

float Circle::getRadius() {
    return this->radius;;
}

void Circle::setRadius(float radius) {
    this->init(radius);
    this->reRenderFlag |= RERENDER_ALL;
}

shared_ptr<CIRCLE> Circle::getCIRCLE() {
    float scaleX = sqrt(this->matrix[0] * this->matrix[0] +
                        this->matrix[1] * this->matrix[1]);
    float scaleY = sqrt(this->matrix[4] * this->matrix[4] +
                        this->matrix[5] * this->matrix[5]);
    auto position = Point(this->matrix[12], this->matrix[13]);
    auto size = this->transform->size * Point(scaleX, scaleY);
    float centerX = position.x + size.width * 0.5f;
    float centerY = position.y + size.height * 0.5f;
    return shared_ptr<CIRCLE>(new CIRCLE(centerX, centerY, this->getRadius() * fmin(this->getScaleX(), this->getScaleY())));
}

shared_ptr<Collider> Circle::getCollider() {
    auto collider = shared_ptr<Collider>(new Collider(ColliderShape::Circle));
    collider->aabb = this->getAABB();
    collider->circle = this->getCIRCLE();
    return collider;
}
