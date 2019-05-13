#include "mog/Constants.h"
#include "mog/base/Sprite.h"
#include "mog/core/Engine.h"

using namespace mog;

std::unordered_map<std::string, std::weak_ptr<Texture2D>> Sprite::cachedTexture2d;
std::unordered_map<std::string, std::weak_ptr<Texture2D>> Sprite::globalCachedTexture2d;

std::shared_ptr<Sprite> Sprite::create(std::string filename, const Rect &rect) {
    auto sprite = std::shared_ptr<Sprite>(new Sprite());
    sprite->init(filename, rect);
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

Rect Sprite::getRect() {
    return this->rect;
}

void Sprite::init(std::string filename, const Rect &rect) {
    this->filename = filename;
    if (Sprite::cachedTexture2d.count(filename) > 0) {
        this->textures[0] = Sprite::cachedTexture2d[filename].lock();
    }
    if (this->textures[0] == nullptr) {
        this->textures[0] = Texture2D::createWithAsset(filename);
        if (this->textures[0]) {
            Sprite::cachedTexture2d[filename] = this->textures[0];
        }
    }
    this->numOfTexture = 1;

    Rect _rect = rect;
    if (rect.size == Size::zero) {
        _rect.size = Size(this->textures[0]->width / this->textures[0]->density.value,
                          this->textures[0]->height / this->textures[0]->density.value);
    }
    this->rect = _rect;
    this->transform->size = this->rect.size;
    
    this->initRendererVertices(4, 4);
}

void Sprite::initWithFilePath(std::string filepath, const Rect &rect, Density density) {
    this->filename = filepath;
    this->textures[0] = Texture2D::createWithFile(filepath, density);
    this->numOfTexture = 1;

    Rect _rect = rect;
    if (rect.size == Size::zero) {
        _rect.size = Size(this->textures[0]->width / this->textures[0]->density.value,
                          this->textures[0]->height / this->textures[0]->density.value);
    }
    this->rect = _rect;
    this->transform->size = this->rect.size;
    
    this->initRendererVertices(4, 4);
}

void Sprite::initWithImage(const std::shared_ptr<ByteArray> &bytes) {
    this->textures[0] = Texture2D::createWithImage(bytes);
    this->numOfTexture = 1;

    this->transform->size.width = this->textures[0]->width / this->textures[0]->density.value;
    this->transform->size.height = this->textures[0]->height / this->textures[0]->density.value;
    this->rect = Rect(Point::zero, this->transform->size);
    
    this->initRendererVertices(4, 4);
}

void Sprite::initWithRGBA(unsigned char *data, int width, int height) {
    this->textures[0] = Texture2D::createWithRGBA(data, width, height, Screen::getDensity());
    this->numOfTexture = 1;

    this->transform->size.width = this->textures[0]->width / this->textures[0]->density.value;
    this->transform->size.height = this->textures[0]->height / this->textures[0]->density.value;
    this->rect = Rect(Point::zero, this->transform->size);
    
    this->initRendererVertices(4, 4);
}

void Sprite::initWithTexture(const std::shared_ptr<Texture2D> &texture, const Rect &rect) {
    this->textures[0] = texture;
    this->numOfTexture = 1;

    this->transform->size.width = this->textures[0]->width / this->textures[0]->density.value;
    this->transform->size.height = this->textures[0]->height / this->textures[0]->density.value;
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
