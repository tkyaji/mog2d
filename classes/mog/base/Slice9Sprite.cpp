#include "mog/base/Slice9Sprite.h"
#include "mog/core/Engine.h"
#include <math.h>

using namespace mog;

shared_ptr<Slice9Sprite> Slice9Sprite::create(const shared_ptr<Sprite> sprite, const Rect &centerRect) {
    auto slice9sprite = shared_ptr<Slice9Sprite>(new Slice9Sprite());
    slice9sprite->init(sprite, centerRect);
    return slice9sprite;
}

void Slice9Sprite::init(const shared_ptr<Sprite> sprite, const Rect &centerRect) {
    this->filename = sprite->getFilename();
    this->texture = sprite->getTexture();
    this->transform->size = sprite->getSize();
    this->size = this->transform->size;
    this->rect = sprite->getRect();
    this->centerRect = centerRect;
}

Slice9Sprite::Slice9Sprite() {
    this->dynamicDraw = true;
}

Rect Slice9Sprite::getCenterRect() {
    return this->centerRect;
}

void Slice9Sprite::getVerticesNum(int *num) {
    if (!this->visible) return;
    (*num) += 16;
}

void Slice9Sprite::getIndiciesNum(int *num) {
    if (!this->visible) return;
    if (*num > 0) {
        (*num) += 2;
    }
    (*num) += 28;
}

void Slice9Sprite::bindVertices(float *vertices, int *idx, bool bakeTransform) {
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
    
    float right = this->rect.size.width - (this->centerRect.position.x + this->centerRect.size.width);
    float bottom = this->rect.size.height - (this->centerRect.position.y + this->centerRect.size.height);

    float xx[4];
    xx[0] = 0;
    xx[1] = this->centerRect.position.x;
    xx[2] = this->transform->size.width - right;
    xx[3] = this->transform->size.width;
    
    float yy[4];
    yy[0] = 0;
    yy[1] = this->centerRect.position.y;
    yy[2] = this->transform->size.height - bottom;
    yy[3] = this->transform->size.height;
    
    for (int xi = 0; xi < 4; xi++) {
        for (int yi = 0; yi < 4; yi++) {
            float x = xx[xi] * this->screenScale / this->transform->scale.x;
            float y = yy[yi] * this->screenScale / this->transform->scale.y;
            
            Point p = v1 * x + v2 * y;
            vertices[(*idx)++] = (p.x + offset.x);
            vertices[(*idx)++] = (p.y + offset.y);
        }
    }
}

void Slice9Sprite::bindIndices(short *indices, int *idx, int start) {
    if (!this->visible) return;
    if (start > 0) {
        indices[*idx] = indices[(*idx) - 1];
        (*idx)++;
        indices[(*idx)++] = 0 + start;
    }
    
    short sliceIndices[28] = {
        0, 4, 1, 5, 2, 6, 3, 7,
        7, 4,
        4, 8, 5, 9, 6, 10, 7, 11,
        11, 8,
        8, 12, 9, 13, 10, 14, 11, 15,
    };
    for (int i = 0; i < 28; i++) {
        indices[(*idx)++] = sliceIndices[i] + start;
    }
}

void Slice9Sprite::bindVertexTexCoords(float *vertexTexCoords, int *idx, float x, float y, float w, float h) {
    if (!this->visible) return;
    
    Size texSize = Size(this->texture->width, this->texture->height) / this->texture->density.value;
    x += (this->rect.position.x / texSize.width) * w;
    y += (this->rect.position.y / texSize.height) * h;

    float xx[4] = {
        x,
        x + (this->centerRect.position.x / texSize.width) * w,
        x + ((this->centerRect.position.x + this->centerRect.size.width) / texSize.width) * w,
        x + (this->rect.size.width / texSize.width) * w
    };
    float yy[4] = {
        y,
        y + (this->centerRect.position.y / texSize.height) * h,
        y + ((this->centerRect.position.y + this->centerRect.size.height) / texSize.height) * h,
        y + (this->rect.size.height / texSize.height) * h
    };
    for (int xi = 0; xi < 4; xi++) {
        for (int _yi = 0; _yi < 4; _yi++) {
            int yi = _yi;
            if (this->texture->isFlip) {
                yi = 3 - yi;
            }
            vertexTexCoords[(*idx)++] = xx[xi];
            vertexTexCoords[(*idx)++] = yy[yi];
        }
    }
}

void Slice9Sprite::setReRenderFlag(unsigned char flag) {
    Sprite::setReRenderFlag(flag);
    if ((flag & RERENDER_VERTEX) == RERENDER_VERTEX) {
        this->reRenderFlag |= RERENDER_VERTEX;
    }
}

shared_ptr<Slice9Sprite> Slice9Sprite::clone() {
    auto entity = this->cloneEntity();
    return static_pointer_cast<Slice9Sprite>(entity);
}

shared_ptr<Entity> Slice9Sprite::cloneEntity() {
    auto sprite = shared_ptr<Slice9Sprite>(new Slice9Sprite());
    sprite->copyFrom(shared_from_this());
    return sprite;
}

void Slice9Sprite::copyFrom(const shared_ptr<Entity> &src) {
    auto srcSprite = static_pointer_cast<Slice9Sprite>(src);
    this->filename = srcSprite->getFilename();
    this->centerRect = srcSprite->centerRect;
    this->texture = srcSprite->getTexture();
    this->transform->size = srcSprite->getSize();
    this->rect = srcSprite->getRect();
}

EntityType Slice9Sprite::getEntityType() {
    return EntityType::Slice9Sprite;
}
