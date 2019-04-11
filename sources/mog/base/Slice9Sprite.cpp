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
    this->rect = sprite->getRect();
    this->centerRect = centerRect;
    this->initRendererVertices(16, 28);
}

Rect Slice9Sprite::getCenterRect() {
    return this->centerRect;
}

void Slice9Sprite::bindVertices(const std::shared_ptr<Renderer> &renderer, int *verticesIdx, int *indicesIdx, bool bakeTransform) {
    Point offset, v1, v2;
    if (bakeTransform) {
        offset = Point(this->renderer->matrix[12], this->renderer->matrix[13]);
        v1 = Point(this->renderer->matrix[0], this->renderer->matrix[1]);
        v2 = Point(this->renderer->matrix[4], this->renderer->matrix[5]);
    }
    
    float xx[4];
    xx[0] = 0;
    xx[1] = this->centerRect.position.x;
    xx[2] = this->transform->size.width - (this->rect.size.width - (this->centerRect.position.x + this->centerRect.size.width));
    xx[3] = this->transform->size.width;
    
    float yy[4];
    yy[0] = 0;
    yy[1] = this->centerRect.position.y;
    yy[2] = this->transform->size.height - (this->rect.size.height - (this->centerRect.position.y + this->centerRect.size.height));
    yy[3] = this->transform->size.height;
    
    if (!this->active) {
        xx[1] = 0;  yy[1] = 0;
        xx[2] = 0;  yy[2] = 0;
        xx[3] = 0;  yy[3] = 0;
    }
    
    int startN = *verticesIdx / 2;
    for (int xi = 0; xi < 4; xi++) {
        for (int yi = 0; yi < 4; yi++) {
            Point p = Point(xx[xi], yy[yi]);
            if (bakeTransform) {
                p = v1 * p.x + v2 * p.y + offset;
            }
            renderer->vertices[(*verticesIdx)++] = p.x;
            renderer->vertices[(*verticesIdx)++] = p.y;
        }
    }
    
    if (startN > 0) {
        renderer->indices[*indicesIdx] = renderer->indices[(*indicesIdx) - 1];
        (*indicesIdx)++;
        renderer->indices[(*indicesIdx)++] = 0 + startN;
    }
    short sliceIndices[28] = {
        0, 4, 1, 5, 2, 6, 3, 7,
        7, 4,
        4, 8, 5, 9, 6, 10, 7, 11,
        11, 8,
        8, 12, 9, 13, 10, 14, 11, 15,
    };
    for (int i = 0; i < 28; i++) {
        renderer->indices[(*indicesIdx)++] = sliceIndices[i] + startN;
    }
}

void Slice9Sprite::bindVertexTexCoords(const std::shared_ptr<Renderer> &renderer, int *idx, float x, float y, float w, float h) {
    Size texSize = Size(this->texture->width, this->texture->height) / this->texture->density.value;
    x += this->rect.position.x / texSize.width;
    y += this->rect.position.y / texSize.height;
    w *= (this->rect.size.width / texSize.width);
    h *= (this->rect.size.height / texSize.height);
    float xx[4] = {
        x,
        x + (this->centerRect.position.x / this->rect.size.width) * w,
        x + ((this->centerRect.position.x + this->centerRect.size.width) / this->rect.size.width) * w,
        x + w
    };
    float yy[4] = {
        y,
        y + (this->centerRect.position.y / this->rect.size.height) * h,
        y + ((this->centerRect.position.y + this->centerRect.size.height) / this->rect.size.height) * h,
        y + h
    };
    for (int xi = 0; xi < 4; xi++) {
        for (int _yi = 0; _yi < 4; _yi++) {
            int yi = _yi;
            if (this->texture->isFlip) {
                yi = 3 - yi;
            }
            renderer->vertexTexCoords[(*idx)++] = xx[xi];
            renderer->vertexTexCoords[(*idx)++] = yy[yi];
        }
    }
}
