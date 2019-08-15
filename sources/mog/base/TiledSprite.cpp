#include "mog/base/TiledSprite.h"

using namespace mog;

std::shared_ptr<TiledSprite> TiledSprite::create(std::string filename, const Size &size, const Rect &rect) {
    auto tiledSprite = std::shared_ptr<TiledSprite>(new TiledSprite());
    tiledSprite->filename = filename;
    tiledSprite->size = size;
    tiledSprite->rect = rect;
    tiledSprite->init();
    return tiledSprite;
}

std::shared_ptr<TiledSprite> TiledSprite::createWithTexture(const std::shared_ptr<Texture2D> &texture, const Size &size, const Rect &rect) {
    auto tiledSprite = std::shared_ptr<TiledSprite>(new TiledSprite());
    tiledSprite->transform->size = size;
    tiledSprite->rect = rect;
    tiledSprite->initWithTexture(texture);
    return tiledSprite;
}

std::shared_ptr<TiledSprite> TiledSprite::createWithSprite(const std::shared_ptr<Sprite> &sprite, const Size &size) {
    return TiledSprite::createWithTexture(sprite->getTexture(), size, sprite->getRect());
}

void TiledSprite::init() {
    auto texture = Texture2D::createWithAsset(this->filename);
    this->initWithTexture(texture);
    this->dirtyFlag |= (DIRTY_ALL | DIRTY_SIZE | DIRTY_ANCHOR);
}

void TiledSprite::initWithTexture(const std::shared_ptr<Texture2D> texture) {
    this->textures[0] = texture;
    if (this->rect.size == Size::zero) {
        this->rect.size = Size(this->textures[0]->width / this->textures[0]->density.value,
                          this->textures[0]->height / this->textures[0]->density.value);
    }
    if (this->size == Size::zero) {
        this->size = this->rect.size;
    }
    this->texSize = Size(this->textures[0]->width, this->textures[0]->height) / this->textures[0]->density.value;
}

std::string TiledSprite::getFilename() {
    return this->filename;
}

void TiledSprite::setFilename(std::string filename, bool withReset) {
    this->filename = filename;
    if (withReset) {
        this->rect = Rect::zero;
        this->size = Size::zero;
    }
    this->init();
}

Rect TiledSprite::getRect() {
    return this->rect;
}

void TiledSprite::setRect(const Rect &rect) {
    this->rect = rect;
    this->init();
}

void TiledSprite::bindVertex() {
    if ((this->dirtyFlag & DIRTY_VERTEX) == DIRTY_VERTEX) {
        this->updateTransform();
        int xCount = ceil(this->transform->size.width / texSize.width);
        int yCount = ceil(this->transform->size.height / texSize.height);
        if (this->xCount != xCount || this->yCount != yCount) {
            this->xCount = xCount;
            this->yCount = yCount;
            int vCount = this->xCount * this->yCount * 6 - 2;
            this->initRendererVertices(vCount, vCount);
        }

        this->renderer->newVertexTexCoordsArr(0);
        this->dirtyFlag |= DIRTY_TEX_COORDS;
    }
    Entity::bindVertex();
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
        y0 = 0;
        y1 = 0;
        if (this->active) {
            y0 = this->texSize.height * yi;
            y1 = this->texSize.height * (yi + 1);
            if (y1 > this->transform->size.height) {
                y1 = this->transform->size.height;
            }
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
            
            if (yi > 0 || xi > 0) {
                renderer->vertices[(*verticesIdx)] = renderer->vertices[(*verticesIdx)-2];
                renderer->vertices[(*verticesIdx)+1] = renderer->vertices[(*verticesIdx)-1];
                (*verticesIdx) += 2;

                renderer->vertices[(*verticesIdx)++] = p0.x;    renderer->vertices[(*verticesIdx)++] = p0.y;
                
                renderer->indices[(*indicesIdx)++] = ii++;
                renderer->indices[(*indicesIdx)++] = ii++;
            }

            renderer->vertices[(*verticesIdx)++] = p0.x;    renderer->vertices[(*verticesIdx)++] = p0.y;
            renderer->vertices[(*verticesIdx)++] = p1.x;    renderer->vertices[(*verticesIdx)++] = p1.y;
            renderer->vertices[(*verticesIdx)++] = p2.x;    renderer->vertices[(*verticesIdx)++] = p2.y;
            renderer->vertices[(*verticesIdx)++] = p3.x;    renderer->vertices[(*verticesIdx)++] = p3.y;
            
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
        hs = ((this->transform->size.height * this->textures[0]->density.value) - (this->textures[0]->height * yi)) / this->textures[0]->height;
        if (hs > 1.0f) hs = 1.0f;
        
        for (int xi = 0; xi < this->xCount; xi++) {
            ws = ((this->transform->size.width * this->textures[0]->density.value) - (this->textures[0]->width * xi)) / this->textures[0]->width;
            if (ws > 1.0f) ws = 1.0f;
            
            if (this->textures[0]->isFlip) {
                if (yi > 0 || xi > 0) {
                    renderer->vertexTexCoords[texIdx][(*idx)] = renderer->vertexTexCoords[texIdx][(*idx)-2];
                    renderer->vertexTexCoords[texIdx][(*idx)+1] = renderer->vertexTexCoords[texIdx][(*idx)-1];
                    (*idx) += 2;
                    
                    renderer->vertexTexCoords[texIdx][(*idx)++] = _x;           renderer->vertexTexCoords[texIdx][(*idx)++] = _y + _h * hs;
                }

                renderer->vertexTexCoords[texIdx][(*idx)++] = _x;           renderer->vertexTexCoords[texIdx][(*idx)++] = _y + _h * hs;
                renderer->vertexTexCoords[texIdx][(*idx)++] = _x;           renderer->vertexTexCoords[texIdx][(*idx)++] = _y;
                renderer->vertexTexCoords[texIdx][(*idx)++] = _x + _w * ws; renderer->vertexTexCoords[texIdx][(*idx)++] = _y + _h * hs;
                renderer->vertexTexCoords[texIdx][(*idx)++] = _x + _w * ws; renderer->vertexTexCoords[texIdx][(*idx)++] = _y;
                
            } else {
                if (yi > 0 || xi > 0) {
                    renderer->vertexTexCoords[texIdx][(*idx)] = renderer->vertexTexCoords[texIdx][(*idx)-2];
                    renderer->vertexTexCoords[texIdx][(*idx)+1] = renderer->vertexTexCoords[texIdx][(*idx)-1];
                    (*idx) += 2;
                    
                    renderer->vertexTexCoords[texIdx][(*idx)++] = _x;           renderer->vertexTexCoords[texIdx][(*idx)++] = _y;
                }

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
    auto sprite = TiledSprite::createWithTexture(this->textures[0], this->transform->size, this->rect);
    this->copyProperties(std::static_pointer_cast<Entity>(shared_from_this()));
    return sprite;
}

std::shared_ptr<Dictionary> TiledSprite::serialize() {
    auto dict = Entity::serialize();
    dict->put(PROP_KEY_ENTITY_TYPE, Int::create((int)EntityType::Slice9Sprite));
    dict->put(PROP_KEY_FILENAME, String::create(this->filename));
    dict->put(PROP_KEY_RECT_X, Float::create(this->rect.position.x));
    dict->put(PROP_KEY_RECT_Y, Float::create(this->rect.position.x));
    dict->put(PROP_KEY_RECT_WIDTH, Float::create(this->rect.size.width));
    dict->put(PROP_KEY_RECT_HEIGHT, Float::create(this->rect.size.height));
    return dict;
}

void TiledSprite::deserializeData(const std::shared_ptr<Dictionary> &dict, const std::unordered_map<std::string, std::unordered_map<std::string, std::shared_ptr<Data>>> &params) {
    Entity::deserializeData(dict, params);
    this->filename = dict->get<String>(PROP_KEY_FILENAME)->getValue();
    this->rect.position.x = this->getPropertyData<Float>(dict, PROP_KEY_RECT_X, params)->getValue();
    this->rect.position.y = this->getPropertyData<Float>(dict, PROP_KEY_RECT_Y, params)->getValue();
    this->rect.size.width = this->getPropertyData<Float>(dict, PROP_KEY_RECT_WIDTH, params)->getValue();
    this->rect.size.height = this->getPropertyData<Float>(dict, PROP_KEY_RECT_HEIGHT, params)->getValue();
}
