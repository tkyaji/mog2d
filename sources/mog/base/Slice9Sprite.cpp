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
}

Slice9Sprite::Slice9Sprite() {
}

Rect Slice9Sprite::getCenterRect() {
    return this->centerRect;
}

/*
void Slice9Sprite::getVerticesNum(int *verticesNum, int *indiciesNum) {
    if (!this->visible) return;
    if (verticesNum != nullptr) {
        (*verticesNum) += 16;
    }
    if (indiciesNum != nullptr) {
        if (*indiciesNum > 0) {
            (*indiciesNum) += 2;
        }
        (*indiciesNum) += 28;
    }
}
*/

void Slice9Sprite::bindVertices(const std::shared_ptr<Renderer> &renderer, int *verticesIdx, int *indicesIdx, bool bakeTransform) {
    if (!this->visible) return;
    
    Point offset, v1, v2;
    if (bakeTransform) {
        offset = Point(this->transform->matrix[12], this->transform->matrix[13]);
        v1 = Point(this->transform->matrix[0], this->transform->matrix[1]);
        v2 = Point(this->transform->matrix[4], this->transform->matrix[5]);
    }

    float xx[4];
    xx[0] = 0;
    xx[1] = this->centerRect.position.x / this->transform->scale.x;
    xx[2] = this->transform->size.width - ((this->transform->size.width - (this->centerRect.position.x + this->centerRect.size.width)) / this->transform->scale.x);
    xx[3] = this->transform->size.width;

    float yy[4];
    yy[0] = 0;
    yy[1] = this->centerRect.position.y / this->transform->scale.y;
    yy[2] = this->transform->size.height - ((this->transform->size.height - (this->centerRect.position.y + this->centerRect.size.height)) / this->transform->scale.y);
    yy[3] = this->transform->size.height;
    
    int start = *indicesIdx;
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
    
    if (start > 0) {
        renderer->indices[*indicesIdx] = renderer->indices[(*indicesIdx) - 1];
        (*indicesIdx)++;
        renderer->indices[(*indicesIdx)++] = 0 + start;
    }
    short sliceIndices[28] = {
        0, 4, 1, 5, 2, 6, 3, 7,
        7, 4,
        4, 8, 5, 9, 6, 10, 7, 11,
        11, 8,
        8, 12, 9, 13, 10, 14, 11, 15,
    };
    for (int i = 0; i < 28; i++) {
        renderer->indices[(*indicesIdx)++] = sliceIndices[i] + start;
    }
}

void Slice9Sprite::bindVertexTexCoords(const std::shared_ptr<Renderer> &renderer, int *idx, float x, float y, float w, float h) {
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
            renderer->vertexTexCoords[(*idx)++] = xx[xi];
            renderer->vertexTexCoords[(*idx)++] = yy[yi];
        }
    }
}

/*
void Slice9Sprite::setReRenderFlag(unsigned char flag) {
    Sprite::setReRenderFlag(flag);
    if ((flag & RERENDER_VERTEX) == RERENDER_VERTEX) {
        this->reRenderFlag |= RERENDER_VERTEX;
    }
}
 */
