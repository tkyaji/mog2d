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
    this->size = this->transform->size;
    
    float deviceDensity = Device::getDeviceDensity();
    int radiusi = (int)(radius * deviceDensity * 0.5f);
    float v = 1.0f;
    float r = radiusi - v;
    int length = (int)(radiusi * radiusi) * 4;
    unsigned char *data = new unsigned char[length];
    for (int y = 0; y < radiusi; y++) {
        for (int x = 0; x < radiusi; x++) {
            int i = y * radiusi * 4 + x * 4;
            data[i + 0] = 255;
            data[i + 1] = 255;
            data[i + 2] = 255;
            
            float _x = x + 0.5f;
            float _y = y + 0.5f;
            float l = sqrt(_x * _x + _y * _y);
            if (l < r - v) {
                data[i + 3] = 255;
            } else if (l > r + v) {
                data[i + 3] = 0;
            } else {
                float dist = r - l;
                float alpha = smoothstep(-v, v, dist);
                data[i + 3] = 255 * alpha;
            }
        }
    }
    
    this->texture = Texture2D::createWithRGBA(data, radius, radius);
    this->rect = Rect(0, 0, this->texture->width, this->texture->height);
}

void Circle::getVerticesNum(int *num) {
    if (!this->visible) return;
    (*num) += 9;
}

void Circle::getIndiciesNum(int *num) {
    if (!this->visible) return;
    if (*num > 0) {
        (*num) += 2;
    }
    (*num) += 14;
}

void Circle::bindVertices(float *vertices, int *idx, bool bakeTransform) {
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
    
    Point size = Point(this->transform->size.width * this->screenScale,
                       this->transform->size.height * this->screenScale);
    float xx[3] = {
        0,
        size.x * 0.5f,
        size.x,
    };
    float yy[3] = {
        0,
        size.y * 0.5f,
        size.y,
    };
    
    for (int xi = 0; xi < 3; xi++) {
        for (int yi = 0; yi < 3; yi++) {
            float x = xx[xi];
            float y = yy[yi];
            
            Point p = v1 * x + v2 * y;
            vertices[(*idx)++] = (p.x + offset.x);
            vertices[(*idx)++] = (p.y + offset.y);
        }
    }
}

void Circle::bindIndices(short *indices, int *idx, int start) {
    if (!this->visible) return;
    if (start > 0) {
        indices[*idx] = indices[(*idx) - 1];
        (*idx)++;
        indices[(*idx)++] = 0 + start;
    }
    
    short sliceIndices[14] = {
        0, 3, 1, 4, 2, 5,
        5, 3,
        3, 6, 4, 7, 5, 8,
    };
    for (int i = 0; i < 14; i++) {
        indices[(*idx)++] = sliceIndices[i] + start;
    }
}

void Circle::bindVertexTexCoords(float *vertexTexCoords, int *idx, float x, float y, float w, float h) {
    if (!this->visible) return;
    
    vertexTexCoords[(*idx)++] = x + w;  vertexTexCoords[(*idx)++] = y + h;
    vertexTexCoords[(*idx)++] = x;      vertexTexCoords[(*idx)++] = y + h;
    vertexTexCoords[(*idx)++] = x + w;  vertexTexCoords[(*idx)++] = y + h;
    
    vertexTexCoords[(*idx)++] = x + w;  vertexTexCoords[(*idx)++] = y;
    vertexTexCoords[(*idx)++] = x;      vertexTexCoords[(*idx)++] = y;
    vertexTexCoords[(*idx)++] = x + w;  vertexTexCoords[(*idx)++] = y;
    
    vertexTexCoords[(*idx)++] = x + w;  vertexTexCoords[(*idx)++] = y + h;
    vertexTexCoords[(*idx)++] = x;      vertexTexCoords[(*idx)++] = y + h;
    vertexTexCoords[(*idx)++] = x + w;  vertexTexCoords[(*idx)++] = y + h;
}

float Circle::getRadius() {
    return this->radius;;
}

void Circle::setRadius(float radius) {
    this->init(radius);
    this->setReRenderFlag(RERENDER_ALL);
}

shared_ptr<CIRCLE> Circle::getCIRCLE() {
    float scaleX = sqrt(this->renderer->matrix[0] * this->renderer->matrix[0] +
                        this->renderer->matrix[1] * this->renderer->matrix[1]);
    float scaleY = sqrt(this->renderer->matrix[4] * this->renderer->matrix[4] +
                        this->renderer->matrix[5] * this->renderer->matrix[5]);
    auto position = Point(this->renderer->matrix[12], this->renderer->matrix[13]) / this->screenScale;
    auto size = this->transform->size * Point(scaleX, scaleY);
    float centerX = position.x + size.width * 0.5f;
    float centerY = position.y + size.height * 0.5f;
    return shared_ptr<CIRCLE>(new CIRCLE(centerX, centerY, this->getRadius()));
}

shared_ptr<Collider> Circle::getCollider() {
    auto collider = shared_ptr<Collider>(new Collider(ColliderShape::Circle));
    collider->aabb = this->getAABB();
    collider->circle = this->getCIRCLE();
    return collider;
}

shared_ptr<Circle> Circle::clone() {
    auto entity = this->cloneEntity();
    return static_pointer_cast<Circle>(entity);
}

shared_ptr<Entity> Circle::cloneEntity() {
    auto circle = shared_ptr<Circle>(new Circle());
    circle->copyFrom(shared_from_this());
    return circle;
}

void Circle::copyFrom(const shared_ptr<Entity> &src) {
    auto circleSrc = static_pointer_cast<Circle>(src);
    this->radius = circleSrc->radius;
    this->texture = circleSrc->texture;
    this->transform->copyFrom(circleSrc->transform);
}

EntityType Circle::getEntityType() {
    return EntityType::Circle;
}
