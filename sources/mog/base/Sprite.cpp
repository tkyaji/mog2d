#include "mog/Constants.h"
#include "mog/base/Sprite.h"
#include "mog/core/Engine.h"

using namespace mog;

std::unordered_map<std::string, std::weak_ptr<Texture2D>> Sprite::cachedTexture2d;
std::unordered_map<std::string, std::weak_ptr<Texture2D>> Sprite::globalCachedTexture2d;

std::shared_ptr<Sprite> Sprite::create(std::string filename, const Rect &rect) {
    auto sprite = std::shared_ptr<Sprite>(new Sprite());
    sprite->filename = filename;
    sprite->rect = rect;
    sprite->init();
    return sprite;
}

std::shared_ptr<Sprite> Sprite::createWithFilePath(std::string filepath, Density density) {
    auto sprite = std::shared_ptr<Sprite>(new Sprite());
    sprite->initWithFilePath(filepath, Rect::zero, density);
    return sprite;
}

std::shared_ptr<Sprite> Sprite::createWithFilePath(std::string filepath, const Rect &rect, Density density) {
    auto sprite = std::shared_ptr<Sprite>(new Sprite());
    sprite->initWithFilePath(filepath, rect, density);
    return sprite;
}

std::shared_ptr<Sprite> Sprite::createWithImage(const std::shared_ptr<ByteArray> &bytes) {
    auto sprite = std::shared_ptr<Sprite>(new Sprite());
    sprite->initWithImage(bytes);
    return sprite;
}

std::shared_ptr<Sprite> Sprite::createWithRGBA(unsigned char *data, int width, int height) {
    auto sprite = std::shared_ptr<Sprite>(new Sprite());
    sprite->initWithRGBA(data, width, height);
    return sprite;
}

std::shared_ptr<Sprite> Sprite::createWithTexture(const std::shared_ptr<Texture2D> &texture, const Rect &rect) {
    auto sprite = std::shared_ptr<Sprite>(new Sprite());
    sprite->initWithTexture(texture, rect);
    return sprite;
}

void Sprite::registerCache(std::string filename) {
    auto texture = Texture2D::createWithAsset(filename);
    if (texture) {
        Sprite::globalCachedTexture2d[filename] = texture;
    }
}

void Sprite::removeCache(std::string filename) {
    if (Sprite::globalCachedTexture2d.count(filename) > 0) {
        Sprite::globalCachedTexture2d.erase(filename);
    }
}

void Sprite::clearCache() {
    Sprite::globalCachedTexture2d.clear();
}

Sprite::~Sprite() {
    if (this->filename.size() > 0 && Sprite::cachedTexture2d.count(this->filename) > 0) {
        Sprite::cachedTexture2d.erase(this->filename);
    }
}

std::string Sprite::getFilename() {
    return this->filename;
}

void Sprite::setFilename(std::string filename, bool withReset) {
    this->filename = filename;
    if (withReset) {
        this->rect = Rect::zero;
        this->size = Size::zero;
    }
    this->init();
}

Rect Sprite::getRect() {
    return this->rect;
}

void Sprite::setRect(const Rect &rect) {
    this->rect = rect;
    this->init();
}

void Sprite::init() {
    if (Sprite::cachedTexture2d.count(filename) > 0) {
        this->textures[0] = Sprite::cachedTexture2d[filename].lock();
    }
    this->textures[0] = Texture2D::createWithAsset(filename);
    if (this->textures[0]) {
        Sprite::cachedTexture2d[filename] = this->textures[0];
    }
    
    if (this->rect.size == Size::zero) {
        this->rect.size = Size(this->textures[0]->width / this->textures[0]->density.value,
                               this->textures[0]->height / this->textures[0]->density.value);
    }
    if (this->size == Size::zero) {
        this->size = this->rect.size;
    }

    this->initRendererVertices(4, 4);
    this->dirtyFlag |= (DIRTY_ALL | DIRTY_SIZE | DIRTY_ANCHOR);
}

void Sprite::initWithFilePath(std::string filepath, const Rect &rect, Density density) {
    this->filename = filepath;
    this->textures[0] = Texture2D::createWithFile(filepath, density);

    Rect _rect = rect;
    if (rect.size == Size::zero) {
        _rect.size = Size(this->textures[0]->width / this->textures[0]->density.value,
                          this->textures[0]->height / this->textures[0]->density.value);
    }
    this->rect = _rect;
    this->size = this->rect.size;
    
    this->initRendererVertices(4, 4);
}

void Sprite::initWithImage(const std::shared_ptr<ByteArray> &bytes) {
    this->textures[0] = Texture2D::createWithImage(bytes);
    this->size.width = this->textures[0]->width / this->textures[0]->density.value;
    this->size.height = this->textures[0]->height / this->textures[0]->density.value;
    this->rect = Rect(Point::zero, this->size);
    
    this->initRendererVertices(4, 4);
}

void Sprite::initWithRGBA(unsigned char *data, int width, int height) {
    this->textures[0] = Texture2D::createWithRGBA(data, width, height, Screen::getDensity());
    this->size.width = this->textures[0]->width / this->textures[0]->density.value;
    this->size.height = this->textures[0]->height / this->textures[0]->density.value;
    this->rect = Rect(Point::zero, this->size);
    
    this->initRendererVertices(4, 4);
}

void Sprite::initWithTexture(const std::shared_ptr<Texture2D> &texture, const Rect &rect) {
    this->textures[0] = texture;
    this->size.width = this->textures[0]->width / this->textures[0]->density.value;
    this->size.height = this->textures[0]->height / this->textures[0]->density.value;
    Rect _rect = rect;
    if (rect.size == Size::zero) {
        _rect.size = Size(this->textures[0]->width / this->textures[0]->density.value,
                          this->textures[0]->height / this->textures[0]->density.value);
    }
    this->rect = _rect;
    
    this->initRendererVertices(4, 4);
}

void Sprite::bindVertexTexCoords(const std::shared_ptr<Renderer> &renderer, int *idx, int texIdx, float x, float y, float w, float h) {
    Size texSize = Size(this->textures[texIdx]->width, this->textures[texIdx]->height) / this->textures[texIdx]->density.value;
    x += this->rect.position.x / texSize.width * w;
    y += this->rect.position.y / texSize.height * h;
    w *= this->rect.size.width / texSize.width;
    h *= this->rect.size.height / texSize.height;
    Entity::bindVertexTexCoords(renderer, idx, texIdx, x, y, w, h);
}

std::shared_ptr<Sprite> Sprite::clone() {
    auto e = this->cloneEntity();
    return std::static_pointer_cast<Sprite>(e);
}

std::shared_ptr<Entity> Sprite::cloneEntity() {
    auto sprite = Sprite::createWithTexture(this->textures[0], this->rect);
    sprite->copyProperties(std::static_pointer_cast<Entity>(shared_from_this()));
    return sprite;
}

std::shared_ptr<Dictionary> Sprite::serialize() {
    auto dict = Entity::serialize();
    dict->put(PROP_KEY_ENTITY_TYPE, Int::create((int)EntityType::Sprite));
    dict->put(PROP_KEY_FILENAME, String::create(this->filename));
    dict->put(PROP_KEY_RECT_X, Float::create(this->rect.position.x));
    dict->put(PROP_KEY_RECT_Y, Float::create(this->rect.position.x));
    dict->put(PROP_KEY_RECT_WIDTH, Float::create(this->rect.size.width));
    dict->put(PROP_KEY_RECT_HEIGHT, Float::create(this->rect.size.height));
    return dict;
}

void Sprite::deserializeData(const std::shared_ptr<Dictionary> &dict, const std::unordered_map<std::string, std::unordered_map<std::string, std::shared_ptr<Data>>> &params) {
    Entity::deserializeData(dict, params);
    this->filename = getPropertyData<String>(dict, PROP_KEY_FILENAME, params)->getValue();
    this->rect.position.x = this->getPropertyData<Float>(dict, PROP_KEY_RECT_X, params)->getValue();
    this->rect.position.y = this->getPropertyData<Float>(dict, PROP_KEY_RECT_Y, params)->getValue();
    this->rect.size.width = this->getPropertyData<Float>(dict, PROP_KEY_RECT_WIDTH, params)->getValue();
    this->rect.size.height = this->getPropertyData<Float>(dict, PROP_KEY_RECT_HEIGHT, params)->getValue();
}
