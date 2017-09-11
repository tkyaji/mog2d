#include "mog/base/Circle.h"
#include "mog/core/Engine.h"
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
    
    float v = 1.0f;
    float r = radius - v;
    int length = (int)(radius * radius + 0.5f) * 4;
    unsigned char *data = new unsigned char[length];
    for (int y = 0; y < radius; y++) {
        for (int x = 0; x < radius; x++) {
            int i = y * radius * 4 + x * 4;
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
    this->frameSize = Size(this->texture->width, this->texture->height);
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
        this->renderer->applyTransform(this->transform, false);
        m = this->renderer->matrix;
        this->renderer->popMatrix();
    } else {
        m = Renderer::identityMatrix;
    }
    
    auto v1 = Point(m[0], m[1]);
    auto v2 = Point(m[4], m[5]);
    auto offset = Point(m[12], m[13]);
    
    Point size = Point(this->transform->size.width * this->transform->screenScale,
                      this->transform->size.height * this->transform->screenScale);
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
    
}

bool Circle::contains(const Point &point) {
    return false;
}

bool Circle::collidesWith(const shared_ptr<Entity> &other) {
    return false;
}

shared_ptr<CIRCLE> Circle::getCIRCLE() {
    return shared_ptr<CIRCLE>(new CIRCLE(this->transform->position.x, this->transform->position.y, this->getRadius()));
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
