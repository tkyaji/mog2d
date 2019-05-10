#include "mog/base/TiledSprite.h"

using namespace mog;

std::shared_ptr<TiledSprite> TiledSprite::create(std::string filename, const Size &size, const Rect &rect) {
    auto texture = Texture2D::createWithAsset(filename);
    return TiledSprite::create(texture, size, rect);
}

std::shared_ptr<TiledSprite> TiledSprite::create(const std::shared_ptr<Texture2D> &texture, const Size &size, const Rect &rect) {
    auto tiledSprite = std::shared_ptr<TiledSprite>(new TiledSprite());
    tiledSprite->init(texture, size, rect);
    return tiledSprite;
}

std::shared_ptr<TiledSprite> TiledSprite::create(const std::shared_ptr<Sprite> &sprite, const Size &size) {
    return TiledSprite::create(sprite->getTexture(), size, sprite->getRect());
}

void TiledSprite::init(const std::shared_ptr<Texture2D> texture, const Size &size, const Rect &rect) {
    this->textures[0] = texture;
    this->numOfTexture = 1;
    this->transform->size = size;
    Rect _rect = rect;
    if (rect.size == Size::zero) {
        _rect.size = Size(this->textures[0]->width / this->textures[0]->density.value,
                          this->textures[0]->height / this->textures[0]->density.value);
    }
    this->rect = _rect;

    this->texSize = Size(this->textures[0]->width, this->textures[0]->height) / this->textures[0]->density.value;
    this->xCount = ceil(this->transform->size.width / texSize.width);
    this->yCount = ceil(this->transform->size.height / texSize.height);
    int vCount = this->xCount * this->yCount * 4 + (this->yCount - 1) * 2;
    this->initRendererVertices(vCount, vCount);
}

Rect TiledSprite::getRect() {
    return this->rect;
}

void TiledSprite::bindVertices(const std::shared_ptr<Renderer> &renderer, int *verticesIdx, int *indicesIdx, bool bakeTransform) {
    Point offset, v1, v2;
    if (bakeTransform) {
        offset = Point(this->renderer->matrix[12], this->renderer->matrix[13]);
        v1 = Point(this->renderer->matrix[0], this->renderer->matrix[1]);
        v2 = Point(this->renderer->matrix[4], this->renderer->matrix[5]);
    }
    
    int startN = *verticesIdx / 2;
    int ii = startN;
    
    if (startN > 0) {
        renderer->indices[*indicesIdx] = renderer->indices[(*indicesIdx) - 1];
        (*indicesIdx)++;
        renderer->indices[(*indicesIdx)++] = 0 + startN;
    }

    float y0 = 0;
    float y1 = 0;
    float x0 = 0;
    float x1 = 0;
    for (int yi = 0; yi < this->yCount; yi++) {
        if (yi > 0) {
            renderer->vertices[(*verticesIdx)++] = x1;
            renderer->vertices[(*verticesIdx)++] = y1;
            renderer->indices[(*indicesIdx)++] = ii++;
        }
        y0 = 0;
        y1 = 0;
        if (this->active) {
            y0 = this->texSize.height * yi;
            y1 = this->texSize.height * (yi + 1);
            if (y1 > this->transform->size.height) {
                y1 = this->transform->size.height;
            }
        }
        if (yi > 0) {
            renderer->vertices[(*verticesIdx)++] = 0;
            renderer->vertices[(*verticesIdx)++] = y1;
            renderer->indices[(*indicesIdx)++] = ii++;
        }
        for (int xi = 0; xi < this->xCount; xi++) {
            x0 = 0;
            x1 = 0;
            if (this->active) {
                x0 = this->texSize.width * xi;
                x1 = this->texSize.width * (xi + 1);
                if (x1 > this->transform->size.width) {
                    x1 = this->transform->size.width;
                }
            }
            Point p0 = Point(x0, y0);
            Point p1 = Point(x0, y1);
            Point p2 = Point(x1, y0);
            Point p3 = Point(x1, y1);
            if (bakeTransform) {
                p0 = v1 * p0.x + v2 * p0.y + offset;
                p1 = v1 * p1.x + v2 * p1.y + offset;
                p2 = v1 * p2.x + v2 * p2.y + offset;
                p3 = v1 * p3.x + v2 * p3.y + offset;
            }
            renderer->vertices[(*verticesIdx)++] = p0.x;
            renderer->vertices[(*verticesIdx)++] = p0.y;
            renderer->vertices[(*verticesIdx)++] = p1.x;
            renderer->vertices[(*verticesIdx)++] = p1.y;
            renderer->vertices[(*verticesIdx)++] = p2.x;
            renderer->vertices[(*verticesIdx)++] = p2.y;
            renderer->vertices[(*verticesIdx)++] = p3.x;
            renderer->vertices[(*verticesIdx)++] = p3.y;
            
            renderer->indices[(*indicesIdx)++] = ii++;
            renderer->indices[(*indicesIdx)++] = ii++;
            renderer->indices[(*indicesIdx)++] = ii++;
            renderer->indices[(*indicesIdx)++] = ii++;
        }
    }
}

void TiledSprite::bindVertexTexCoords(const std::shared_ptr<Renderer> &renderer, int *idx, int texIdx, float x, float y, float w, float h) {
    float _x = x + this->rect.position.x / this->texSize.width;
    float _y = y + this->rect.position.y / this->texSize.height;
    float _w = w * (this->rect.size.width / this->texSize.width);
    float _h = h * (this->rect.size.height / this->texSize.height);
    
    float hs = 1.0f;
    float ws = 1.0f;
    
    for (int yi = 0; yi < this->yCount; yi++) {
        if (yi > 0) {
            if (this->textures[0]->isFlip) {
                renderer->vertexTexCoords[texIdx][(*idx)++] = _x + _w * ws; renderer->vertexTexCoords[texIdx][(*idx)++] = _y;
                renderer->vertexTexCoords[texIdx][(*idx)++] = _x;           renderer->vertexTexCoords[texIdx][(*idx)++] = _y + _h * hs;
            } else {
                renderer->vertexTexCoords[texIdx][(*idx)++] = _x + _w * ws; renderer->vertexTexCoords[texIdx][(*idx)++] = _y + _h * hs;
                renderer->vertexTexCoords[texIdx][(*idx)++] = _x;           renderer->vertexTexCoords[texIdx][(*idx)++] = _y;
            }
        }

        hs = ((this->transform->size.height * this->textures[0]->density.value) - (this->textures[0]->height * yi)) / this->textures[0]->height;
        if (hs > 1.0f) hs = 1.0f;
        
        for (int xi = 0; xi < this->xCount; xi++) {
            ws = ((this->transform->size.width * this->textures[0]->density.value) - (this->textures[0]->width * xi)) / this->textures[0]->width;
            if (ws > 1.0f) ws = 1.0f;
            
            if (this->textures[0]->isFlip) {
                renderer->vertexTexCoords[texIdx][(*idx)++] = _x;           renderer->vertexTexCoords[texIdx][(*idx)++] = _y + _h * hs;
                renderer->vertexTexCoords[texIdx][(*idx)++] = _x;           renderer->vertexTexCoords[texIdx][(*idx)++] = _y;
                renderer->vertexTexCoords[texIdx][(*idx)++] = _x + _w * ws; renderer->vertexTexCoords[texIdx][(*idx)++] = _y + _h * hs;
                renderer->vertexTexCoords[texIdx][(*idx)++] = _x + _w * ws; renderer->vertexTexCoords[texIdx][(*idx)++] = _y;
            } else {
                renderer->vertexTexCoords[texIdx][(*idx)++] = _x;           renderer->vertexTexCoords[texIdx][(*idx)++] = _y;
                renderer->vertexTexCoords[texIdx][(*idx)++] = _x;           renderer->vertexTexCoords[texIdx][(*idx)++] = _y + _h * hs;
                renderer->vertexTexCoords[texIdx][(*idx)++] = _x + _w * ws; renderer->vertexTexCoords[texIdx][(*idx)++] = _y;
                renderer->vertexTexCoords[texIdx][(*idx)++] = _x + _w * ws; renderer->vertexTexCoords[texIdx][(*idx)++] = _y + _h * hs;
            }
        }
    }
}

std::shared_ptr<TiledSprite> TiledSprite::clone() {
    auto e = this->cloneEntity();
    return std::static_pointer_cast<TiledSprite>(e);
}

std::shared_ptr<Entity> TiledSprite::cloneEntity() {
    auto sprite = TiledSprite::create(this->textures[0], this->transform->size, this->rect);
    this->copyProperties(std::static_pointer_cast<Entity>(shared_from_this()));
    return sprite;
}
