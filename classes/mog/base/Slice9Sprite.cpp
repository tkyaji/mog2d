#include "mog/base/Slice9Sprite.h"
#include "mog/core/Engine.h"
#include <math.h>

using namespace mog;

shared_ptr<Slice9Sprite> Slice9Sprite::create(string filename, const Point &centerRectPosition, const Size &centerRectSize) {
    auto sprite = shared_ptr<Slice9Sprite>(new Slice9Sprite());
    sprite->init(filename, Point::zero, Size::zero, centerRectPosition, centerRectSize);
    return sprite;
}

shared_ptr<Slice9Sprite> Slice9Sprite::create(string filename, const Point &framePosition, const Size &frameSize, const Point &centerRectPosition, const Size &centerRectSize) {
    auto sprite = shared_ptr<Slice9Sprite>(new Slice9Sprite());
    sprite->init(filename, framePosition, frameSize, centerRectPosition, centerRectSize);
    return sprite;
}

void Slice9Sprite::init(string filename, const Point &framePosition, const Size &frameSize, const Point &centerRectPosition, const Size &centerRectSize) {
    Sprite::init(filename, framePosition, frameSize);
    this->centerRectPosition = centerRectPosition;
    this->centerRectSize = centerRectSize;
}

Slice9Sprite::Slice9Sprite() {
    this->dynamicDraw = true;
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
        this->renderer->applyTransform(this->transform, false);
        m = this->renderer->matrix;
        this->renderer->popMatrix();
    } else {
        m = Renderer::identityMatrix;
    }
    
    auto v1 = Point(m[0], m[1]);
    auto v2 = Point(m[4], m[5]);
    auto offset = Point(m[12], m[13]);
    
    Size texSize = Size(this->texture->width, this->texture->height) / this->texture->density.value;
    Point baseScale = Point((this->transform->size.width / texSize.width) * this->transform->scale.x,
                            (this->transform->size.height / texSize.height) * this->transform->scale.y);
    Size baseSize = texSize * baseScale;
    
    float xp[2] = {
        this->centerRectPosition.x / texSize.width,
        (texSize.width - (this->centerRectPosition.x + this->centerRectSize.width)) / texSize.width,
    };
    
    float yp[2] = {
        this->centerRectPosition.y / texSize.height,
        (texSize.height - (this->centerRectPosition.y + this->centerRectSize.height)) / texSize.height,
    };
    
    float xx[4];
    xx[0] = 0;
    xx[1] = xp[0] * texSize.width;
    xx[2] = (baseSize.width - (xp[1] * texSize.width));
    xx[3] = baseSize.width;
    
    float yy[4];
    yy[0] = 0;
    yy[1] = yp[0] * texSize.height;
    yy[2] = (baseSize.height - (yp[1] * texSize.height));
    yy[3] = baseSize.height;
    
    for (int xi = 0; xi < 4; xi++) {
        for (int yi = 0; yi < 4; yi++) {
            float x = xx[xi] * this->transform->screenScale / this->transform->scale.x;
            float y = yy[yi] * this->transform->screenScale / this->transform->scale.y;
            
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
    float xx[4] = {
        x,
        x + w * (this->centerRectPosition.x / (this->texture->width / this->texture->density.value)),
        x + w * ((this->centerRectPosition.x + this->centerRectSize.width) / (this->texture->width / this->texture->density.value)),
        x + w
    };
    float yy[4] = {
        y,
        y + h * (this->centerRectPosition.y / (this->texture->height / this->texture->density.value)),
        y + h * ((this->centerRectPosition.y + this->centerRectSize.height) / (this->texture->height / this->texture->density.value)),
        y + h
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
    Sprite::copyFrom(src);
    auto srcSprite = static_pointer_cast<Slice9Sprite>(src);
    this->centerRectPosition = srcSprite->centerRectPosition;
    this->centerRectSize = srcSprite->centerRectSize;
}
