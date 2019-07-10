#include "mog/base/Circle.h"
#include "mog/base/Group.h"
#include "mog/core/Engine.h"
#include "mog/core/Screen.h"
#include <math.h>

using namespace mog;

std::shared_ptr<Circle> Circle::create(float radius) {
    auto circle = std::shared_ptr<Circle>(new Circle());
    circle->radius = radius;
    circle->init();
    return circle;
}

void Circle::init() {
    this->transform->size = Size(radius * 2, radius * 2);
    
    float density = Screen::getDensity();
    int texWidth = (int)(radius * density + 0.5f);
    int texHeight = texWidth;
    unsigned char *data = (unsigned char *)mogmalloc(sizeof(char) * texWidth * texHeight * 4);
    for (int y = 0; y < texHeight; y++) {
        for (int x = 0; x < texWidth; x++) {
            float l = Point::length(Point(x, y));
            float a = (radius * density) - l;
            if (a > 1.0f) a = 1.0f;
            if (a < 0) a = 0;
            data[(y * texWidth + x) * 4 + 0] = 255;
            data[(y * texWidth + x) * 4 + 1] = 255;
            data[(y * texWidth + x) * 4 + 2] = 255;
            data[(y * texWidth + x) * 4 + 3] = (unsigned char)(a * 255.0f + 0.5f);
        }
    }
    this->textures[0] = Texture2D::createWithRGBA(data, texWidth, texHeight, Screen::getDensity());
    this->rect = Rect(Point::zero, this->transform->size);
    this->initRendererVertices(9, 12);
}

void Circle::bindVertices(const std::shared_ptr<Renderer> &renderer, int *verticesIdx, int *indicesIdx, bool bakeTransform) {
    float xx[3] = {
        0,
        this->transform->size.width * 0.5f,
        this->transform->size.width,
    };
    float yy[3] = {
        0,
        this->transform->size.height * 0.5f,
        this->transform->size.height,
    };

    int startN = *verticesIdx / 2;
    auto offset = Point(this->renderer->matrix[12], this->renderer->matrix[13]);
    for (int yi = 0; yi < 3; yi++) {
        for (int xi = 0; xi < 3; xi++) {
            float x = xx[xi];
            float y = yy[yi];
            if (!this->active) {
                x = 0;  y = 0;
            } else if (bakeTransform) {
                x += offset.x;
                y += offset.y;
            }
            renderer->vertices[(*verticesIdx)++] = x;  renderer->vertices[(*verticesIdx)++] = y;
        }
    }

    if (indicesIdx) {
        int startI = *indicesIdx;
        if (startI > 0) {
            renderer->indices[*indicesIdx] = renderer->indices[(*indicesIdx) - 1];
            (*indicesIdx)++;
            renderer->indices[(*indicesIdx)++] = startN;
        }
        int ii[12] = {0, 3, 1, 4, 2, 5, 5, 8, 4, 7, 3, 6};
        for (int i = 0; i < 12; i++) {
            renderer->indices[(*indicesIdx)++] = ii[i] + startN;
        }
    }
}

void Circle::bindVertexTexCoords(const std::shared_ptr<Renderer> &renderer, int *idx, int texIdx, float x, float y, float w, float h) {
    renderer->vertexTexCoords[texIdx][(*idx)++] = x + w;    renderer->vertexTexCoords[texIdx][(*idx)++] = y + h;
    renderer->vertexTexCoords[texIdx][(*idx)++] = x;        renderer->vertexTexCoords[texIdx][(*idx)++] = y + h;
    renderer->vertexTexCoords[texIdx][(*idx)++] = x + w;    renderer->vertexTexCoords[texIdx][(*idx)++] = y + h;

    renderer->vertexTexCoords[texIdx][(*idx)++] = x + w;    renderer->vertexTexCoords[texIdx][(*idx)++] = y;
    renderer->vertexTexCoords[texIdx][(*idx)++] = x;        renderer->vertexTexCoords[texIdx][(*idx)++] = y;
    renderer->vertexTexCoords[texIdx][(*idx)++] = x + w;    renderer->vertexTexCoords[texIdx][(*idx)++] = y;

    renderer->vertexTexCoords[texIdx][(*idx)++] = x + w;    renderer->vertexTexCoords[texIdx][(*idx)++] = y + h;
    renderer->vertexTexCoords[texIdx][(*idx)++] = x;        renderer->vertexTexCoords[texIdx][(*idx)++] = y + h;
    renderer->vertexTexCoords[texIdx][(*idx)++] = x + w;    renderer->vertexTexCoords[texIdx][(*idx)++] = y + h;
}

float Circle::getRadius() {
    return this->radius;;
}

void Circle::setRadius(float radius) {
    this->radius = radius;
    this->init();
    this->dirtyFlag |= DIRTY_ALL;
}

std::shared_ptr<CIRCLE> Circle::getCIRCLE() {
    float scaleX = sqrt(this->matrix[0] * this->matrix[0] +
                        this->matrix[1] * this->matrix[1]);
    float scaleY = sqrt(this->matrix[4] * this->matrix[4] +
                        this->matrix[5] * this->matrix[5]);
    auto position = Point(this->matrix[12], this->matrix[13]);
    auto size = this->transform->size * Point(scaleX, scaleY);
    float centerX = position.x + size.width * 0.5f;
    float centerY = position.y + size.height * 0.5f;
    return std::shared_ptr<CIRCLE>(new CIRCLE(centerX, centerY, this->getRadius() * fmin(this->getScaleX(), this->getScaleY())));
}

std::shared_ptr<Collider> Circle::getCollider() {
    auto collider = std::shared_ptr<Collider>(new Collider(ColliderShape::Circle));
    collider->aabb = this->getAABB();
    collider->circle = this->getCIRCLE();
    return collider;
}

std::shared_ptr<Circle> Circle::clone() {
    auto e = this->cloneEntity();
    return std::static_pointer_cast<Circle>(e);
}

std::shared_ptr<Entity> Circle::cloneEntity() {
    auto circle = Circle::create(this->radius);
    circle->copyProperties(std::static_pointer_cast<Entity>(shared_from_this()));
    return circle;
}

std::shared_ptr<Dictionary> Circle::serialize() {
    auto dict = Entity::serialize();
    dict->put("radius", Float::create(this->radius));
    return dict;
}

void Circle::deserializeData(const std::shared_ptr<Dictionary> &dict) {
    this->radius = dict->get<Float>("radius")->getValue();
}
