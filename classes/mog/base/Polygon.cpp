#include "mog/base/Polygon.h"
#include <math.h>

using namespace mog;

shared_ptr<Texture2D> Polygon::sharedTexture = nullptr;

Polygon::Polygon() {
}

void Polygon::init(const vector<Point> &vertexPoints) {
    this->vertexPoints = vertexPoints;
    
    if (!Polygon::sharedTexture) {
        Polygon::sharedTexture = Texture2D::createWithColor(TextureType::RGBA, Color::white, 16, 16);
    }
    this->texture = Polygon::sharedTexture;
    
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
}

void Polygon::getVerticesNum(int *num) {
    if (!this->visible) return;
    (*num) += (int)this->vertexPoints.size();
}

void Polygon::getIndiciesNum(int *num) {
    if (!this->visible) return;
    if (*num > 0) {
        (*num) += 2;
    }
    (*num) += (int)this->vertexPoints.size();
}

void Polygon::bindVertices(float *vertices, int *idx, bool bakeTransform) {
    if (!this->visible) return;
    float *m;
    if (bakeTransform) {
        this->renderer->pushMatrix();
        this->renderer->applyTransform(this->transform, this->screenScale, false);
        m = this->renderer->matrix;
        this->renderer->popMatrix();
    } else {
        m = Renderer::identityMatrix;
    }
    
    auto v1 = Point(m[0], m[1]);
    auto v2 = Point(m[4], m[5]);
    auto offset = Point(m[12], m[13]);
    
    for (auto &p : this->vertexPoints) {
        float xp = p.x / (this->maxPosition.x - this->minPosition.x);
        float yp = p.y / (this->maxPosition.y - this->minPosition.y);
        auto pp = ((v1 * xp * this->transform->size.width * this->screenScale) +
                   (v2 * yp * this->transform->size.height * this->screenScale));
        vertices[(*idx)++] = pp.x + offset.x;
        vertices[(*idx)++] = pp.y + offset.y;
    }
}

void Polygon::bindIndices(short *indices, int *idx, int start) {
    if (!this->visible) return;
    if (start > 0) {
        indices[*idx] = indices[(*idx) - 1];
        (*idx)++;
        indices[(*idx)++] = 0 + start;
    }
    for (int i = 0; i < this->vertexPoints.size(); i++) {
        indices[(*idx)++] = start + i;
    }
}

void Polygon::bindVertexTexCoords(float *vertexTexCoords, int *idx, float x, float y, float w, float h) {
    if (!this->visible) return;
    float x1 = x + 0.001;
    float y1 = y + 0.001;
    float x2 = x1 + (w - 0.002);
    float y2 = y1 + (h - 0.002);
    float p[8] = {
        x1, y1,
        x1, y2,
        x2, y1,
        x2, y2,
    };
    int verticesNum = 0;
    this->getVerticesNum(&verticesNum);
    for (int i = 0; i < verticesNum; i++) {
        vertexTexCoords[(*idx)++] = p[(i * 2) % 8];
        vertexTexCoords[(*idx)++] = p[(i * 2 + 1) % 8];
    }
}

void Polygon::copyFrom(const shared_ptr<Entity> &src) {
    DrawEntity::copyFrom(src);
    auto srcPolygon = static_pointer_cast<Polygon>(src);
    this->vertexPoints = srcPolygon->vertexPoints;
    this->minPosition = srcPolygon->minPosition;
    this->maxPosition = srcPolygon->maxPosition;
}
