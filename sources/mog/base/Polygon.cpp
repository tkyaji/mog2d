#include "mog/base/Polygon.h"
#include <math.h>

using namespace mog;

std::shared_ptr<Polygon> Polygon::create(const std::vector<Point> &vertexPoints) {
    auto polygon = std::shared_ptr<Polygon>(new Polygon());
    polygon->init(vertexPoints);
    return polygon;
}

void Polygon::init(const std::vector<Point> &vertexPoints) {
    this->vertexPoints = vertexPoints;
    this->minPosition = vertexPoints[0];
    this->maxPosition = vertexPoints[0];
    for (int i = 1; i < vertexPoints.size(); i++) {
        this->minPosition.x = fmin(this->vertexPoints[i].x, this->minPosition.x);
        this->minPosition.y = fmin(this->vertexPoints[i].y, this->minPosition.y);
        this->maxPosition.x = fmax(this->vertexPoints[i].x, this->maxPosition.x);
        this->maxPosition.y = fmax(this->vertexPoints[i].y, this->maxPosition.y);
    }
    this->transform->size.width = this->maxPosition.x - this->minPosition.x;
    this->transform->size.height = this->maxPosition.y - this->minPosition.y;
    this->initRendererVertices((int)vertexPoints.size(), (int)vertexPoints.size());
}

void Polygon::bindVertices(const std::shared_ptr<Renderer> &renderer, int *verticesIdx, int *indicesIdx, bool bakeTransform) {
    float width = this->maxPosition.x - this->minPosition.x;
    float height = this->maxPosition.y - this->minPosition.y;
    Point scale = Point(this->transform->size.width / width, this->transform->size.height / height);

    int startI = -1;
    int startN = *verticesIdx / 2;
    if (indicesIdx) {
        startI = *indicesIdx;
    }
    Point offset, v1, v2;
    if (bakeTransform) {
        offset = Point(this->renderer->matrix[12], this->renderer->matrix[13]);
        v1 = Point(this->renderer->matrix[0], this->renderer->matrix[1]);
        v2 = Point(this->renderer->matrix[4], this->renderer->matrix[5]);
    }
    if (startI > 0) {
        renderer->indices[*indicesIdx] = renderer->indices[startI - 1];
        (*indicesIdx)++;
        renderer->indices[(*indicesIdx)++] = startN;
    }
    int i = 0;
    for (auto p : this->vertexPoints) {
        if (bakeTransform) {
            p = v1 * p.x + v2 * p.y + offset;
        }
        if (!this->active) {
            p.x = 0;
            p.y = 0;
        }
        renderer->vertices[(*verticesIdx)++] = p.x * scale.x;
        renderer->vertices[(*verticesIdx)++] = p.y * scale.y;
        if (startI > -1) {
            renderer->indices[(*indicesIdx)++] = startN + i;
        }
        i++;
    }
}

void Polygon::bindVertexTexCoords(const std::shared_ptr<Renderer> &renderer, int *idx, int texIdx, float x, float y, float w, float h) {
    if (this->textures[0]) {
        for (int i = 0; i < this->vertexPoints.size(); i++) {
            renderer->vertexTexCoords[texIdx][(*idx)++] = (this->vertexPoints[i].x / this->maxPosition.x) * w;
            renderer->vertexTexCoords[texIdx][(*idx)++] = (this->vertexPoints[i].y / this->maxPosition.y) * h;
        }
    } else {
        for (int i = 0; i < this->vertexPoints.size(); i++) {
            renderer->vertexTexCoords[texIdx][(*idx)++] = -1.0f;
            renderer->vertexTexCoords[texIdx][(*idx)++] = -1.0f;
        }
    }}

std::vector<Point> Polygon::getPoints() {
    return this->vertexPoints;
}

std::shared_ptr<AABB> Polygon::getAABB() {
    auto v1 = Point(this->matrix[0], this->matrix[1]);
    auto v2 = Point(this->matrix[4], this->matrix[5]);
    auto offset = Point(this->matrix[12], this->matrix[13]) + this->minPosition;
    auto size = this->transform->size;
    auto p1 = Point::zero;
    auto p2 = Point(v1.x * size.width, v1.y * size.width);
    auto p3 = Point(v2.x * size.height, v2.y * size.height);
    auto p4 = p2 + p3;
    Point minP = Point(fmin(fmin(p1.x, p2.x), fmin(p3.x, p4.x)), fmin(fmin(p1.y, p2.y), fmin(p3.y, p4.y)));
    Point maxP = Point(fmax(fmax(p1.x, p2.x), fmax(p3.x, p4.x)), fmax(fmax(p1.y, p2.y), fmax(p3.y, p4.y)));
    
    return std::shared_ptr<AABB>(new AABB(offset.x + minP.x, offset.y + minP.y, offset.x + maxP.x, offset.y + maxP.y));
}

std::shared_ptr<POLYGON> Polygon::getPOLYGON() {
    auto offset = Point(this->matrix[12], this->matrix[13]);
    auto v1 = Point(this->matrix[0], this->matrix[1]);
    auto v2 = Point(this->matrix[4], this->matrix[5]);
    int size = (int)this->vertexPoints.size();
    Point *points = (Point *)mogmalloc(sizeof(Point) * size);
    for (int i = 0; i < size; i++) {
        auto p = this->vertexPoints[i];
        points[i] = v1 * p.x + v2 * p.y + offset;
    }
    return std::shared_ptr<POLYGON>(new POLYGON(points, size));
}

std::shared_ptr<Collider> Polygon::getCollider() {
    auto collider = std::shared_ptr<Collider>(new Collider(ColliderShape::Polygon));
    collider->aabb = this->getAABB();
    collider->polygon = this->getPOLYGON();
    return collider;
}

std::shared_ptr<Entity> Polygon::cloneEntity() {
    auto polygon = Polygon::create(this->vertexPoints);
    this->copyProperties(std::static_pointer_cast<Entity>(shared_from_this()));
    return polygon;
}
