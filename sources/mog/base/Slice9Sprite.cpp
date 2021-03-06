#include "mog/base/Slice9Sprite.h"
#include "mog/core/Engine.h"
#include <math.h>

using namespace mog;

std::shared_ptr<Slice9Sprite> Slice9Sprite::create(std::string filename, const Rect &centerRect, const Size &size, const Rect &rect) {
    auto slice9sprite = std::shared_ptr<Slice9Sprite>(new Slice9Sprite());
    slice9sprite->filename = filename;
    slice9sprite->centerRect = centerRect;
    slice9sprite->rect = rect;
    slice9sprite->size = size;
    slice9sprite->init();
    return slice9sprite;
}

std::shared_ptr<Slice9Sprite> Slice9Sprite::createWithTexture(const std::shared_ptr<Texture2D> &texture, const Rect &centerRect, const Size &size, const Rect &rect) {
    auto slice9sprite = std::shared_ptr<Slice9Sprite>(new Slice9Sprite());
    slice9sprite->centerRect = centerRect;
    slice9sprite->rect = rect;
    slice9sprite->size = size;
    slice9sprite->initWithTexture(texture);
    return slice9sprite;
}

std::shared_ptr<Slice9Sprite> Slice9Sprite::createWithSprite(const std::shared_ptr<Sprite> &sprite, const Rect &centerRect, const Size &size, const Rect &rect) {
    return Slice9Sprite::createWithTexture(sprite->getTexture(), centerRect, size, sprite->getRect());
}

void Slice9Sprite::init() {
    auto texture = Texture2D::createWithAsset(filename);
    this->initWithTexture(texture);
    this->dirtyFlag |= (DIRTY_ALL | DIRTY_SIZE | DIRTY_ANCHOR);
}

void Slice9Sprite::initWithTexture(const std::shared_ptr<Texture2D> &texture) {
    this->textures[0] = texture;
    if (this->rect.size == Size::zero) {
        this->rect.size = Size(this->textures[0]->width / this->textures[0]->density.value,
                          this->textures[0]->height / this->textures[0]->density.value);
    }
    if (this->size == Size::zero) {
        this->size = this->rect.size;
    }
    this->initRendererVertices(16, 28);
}

std::string Slice9Sprite::getFilename() {
    return this->filename;
}

void Slice9Sprite::setFilename(std::string filename, bool withReset) {
    this->filename = filename;
    if (withReset) {
        this->rect = Rect::zero;
        this->size = Size::zero;
    }
    this->init();
}

Rect Slice9Sprite::getRect() {
    return this->rect;
}

void Slice9Sprite::setRect(const Rect &rect) {
    this->rect = rect;
    this->init();
}

Rect Slice9Sprite::getCenterRect() {
    return this->centerRect;
}

void Slice9Sprite::setCenterRect(const Rect &centerRect) {
    this->centerRect = centerRect;
    this->init();
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

void Slice9Sprite::bindVertexTexCoords(const std::shared_ptr<Renderer> &renderer, int *idx, int texIdx, float x, float y, float w, float h) {
    Size texSize = Size(this->textures[texIdx]->width, this->textures[texIdx]->height) / this->textures[texIdx]->density.value;
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
            if (this->textures[0]->isFlip) {
                yi = 3 - yi;
            }
            renderer->vertexTexCoords[texIdx][(*idx)++] = xx[xi];
            renderer->vertexTexCoords[texIdx][(*idx)++] = yy[yi];
        }
    }
}

std::shared_ptr<Slice9Sprite> Slice9Sprite::clone() {
    auto e = this->cloneEntity();
    return std::static_pointer_cast<Slice9Sprite>(e);
}

std::shared_ptr<Entity> Slice9Sprite::cloneEntity() {
    auto sprite = Slice9Sprite::createWithTexture(this->textures[0], this->centerRect, this->size, this->rect);
    sprite->copyProperties(std::static_pointer_cast<Slice9Sprite>(sprite));
    return sprite;
}

std::shared_ptr<Dictionary> Slice9Sprite::serialize() {
    auto dict = Entity::serialize();
    dict->put(PROP_KEY_ENTITY_TYPE, Int::create((int)EntityType::Slice9Sprite));
    dict->put(PROP_KEY_FILENAME, String::create(this->filename));
    dict->put(PROP_KEY_RECT_X, Float::create(this->rect.position.x));
    dict->put(PROP_KEY_RECT_Y, Float::create(this->rect.position.x));
    dict->put(PROP_KEY_RECT_WIDTH, Float::create(this->rect.size.width));
    dict->put(PROP_KEY_RECT_HEIGHT, Float::create(this->rect.size.height));
    dict->put(PROP_KEY_CENTER_RECT_X, Float::create(this->centerRect.position.x));
    dict->put(PROP_KEY_CENTER_RECT_Y, Float::create(this->centerRect.position.y));
    dict->put(PROP_KEY_CENTER_RECT_WIDTH, Float::create(this->centerRect.size.width));
    dict->put(PROP_KEY_CENTER_RECT_HEIGHT, Float::create(this->centerRect.size.height));
    return dict;
}

void Slice9Sprite::deserializeData(const std::shared_ptr<Dictionary> &dict, const std::unordered_map<std::string, std::unordered_map<std::string, std::shared_ptr<Data>>> &params) {
    Entity::deserializeData(dict, params);
    this->filename = dict->get<String>(PROP_KEY_FILENAME)->getValue();
    this->rect.position.x = this->getPropertyData<Float>(dict, PROP_KEY_RECT_X, params)->getValue();
    this->rect.position.y = this->getPropertyData<Float>(dict, PROP_KEY_RECT_Y, params)->getValue();
    this->rect.size.width = this->getPropertyData<Float>(dict, PROP_KEY_RECT_WIDTH, params)->getValue();
    this->rect.size.height = this->getPropertyData<Float>(dict, PROP_KEY_RECT_HEIGHT, params)->getValue();
    this->centerRect.position.x = this->getPropertyData<Float>(dict, PROP_KEY_CENTER_RECT_X, params)->getValue();
    this->centerRect.position.y = this->getPropertyData<Float>(dict, PROP_KEY_CENTER_RECT_Y, params)->getValue();
    this->centerRect.size.width = this->getPropertyData<Float>(dict, PROP_KEY_CENTER_RECT_WIDTH, params)->getValue();
    this->centerRect.size.height = this->getPropertyData<Float>(dict, PROP_KEY_CENTER_RECT_HEIGHT, params)->getValue();
}
