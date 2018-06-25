#include "mog/base/Polygon.h"
#include <math.h>

using namespace mog;

Polygon::Polygon() {
}

void Polygon::init(const std::vector<Point> &vertexPoints) {
    this->vertexPoints = vertexPoints;
    this->minPosition = vertexPoints[0];
    this->maxPosition = vertexPoints[0];
    for (int i = 1; i < vertexPoints.size(); i++) {
        this->minPosition.x = min(this->vertexPoints[i].x, this->minPosition.x);
        this->minPosition.y = min(this->vertexPoints[i].y, this->minPosition.y);
        this->maxPosition.x = max(this->vertexPoints[i].x, this->maxPosition.x);
        this->maxPosition.y = max(this->vertexPoints[i].y, this->maxPosition.y);
    }
    this->transform->size.width = this->maxPosition.x - this->minPosition.x;
    this->transform->size.height = this->maxPosition.y - this->minPosition.y;
    this->initRendererVertices((int)vertexPoints.size(), (int)vertexPoints.size());
}

void Polygon::bindVertices(const std::shared_ptr<Renderer> &renderer, int *verticesIdx, int *indicesIdx, bool bakeTransform) {
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
        if (!this->visible) {
            p.x = 0;
            p.y = 0;
        }
        renderer->vertices[(*verticesIdx)++] = p.x;
        renderer->vertices[(*verticesIdx)++] = p.y;
        if (startI > -1) {
            renderer->indices[(*indicesIdx)++] = startN + i;
        }
        i++;
    }
}

void Polygon::bindVertexTexCoords(const std::shared_ptr<Renderer> &renderer, int *idx, float x, float y, float w, float h) {
    for (int i = 0; i < this->vertexPoints.size(); i++) {
        renderer->vertexTexCoords[(*idx)++] = -1.0f;
        renderer->vertexTexCoords[(*idx)++] = -1.0f;
    }
}

shared_ptr<AABB> Polygon::getAABB() {
    auto v1 = Point(this->matrix[0], this->matrix[1]);
    auto v2 = Point(this->matrix[4], this->matrix[5]);
    auto offset = Point(this->matrix[12], this->matrix[13]) + this->minPosition;
    auto size = this->transform->size;
    auto p1 = Point::zero;
    auto p2 = Point(v1.x * size.width, v1.y * size.width);
    auto p3 = Point(v2.x * size.height, v2.y * size.height);
    auto p4 = p2 + p3;
    Point minP = Point(min(min(p1.x, p2.x), min(p3.x, p4.x)), min(min(p1.y, p2.y), min(p3.y, p4.y)));
    Point maxP = Point(max(max(p1.x, p2.x), max(p3.x, p4.x)), max(max(p1.y, p2.y), max(p3.y, p4.y)));
    
    return shared_ptr<AABB>(new AABB(offset.x + minP.x, offset.y + minP.y, offset.x + maxP.x, offset.y + maxP.y));
}
