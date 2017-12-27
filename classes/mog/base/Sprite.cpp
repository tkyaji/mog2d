#include "mog/Constants.h"
#include "mog/base/Sprite.h"
#include "mog/core/Engine.h"

using namespace mog;

unordered_map<string, weak_ptr<Texture2D>> Sprite::cachedTexture2d;
unordered_map<string, weak_ptr<Texture2D>> Sprite::globalCachedTexture2d;

shared_ptr<Sprite> Sprite::create(string filename) {
    auto sprite = shared_ptr<Sprite>(new Sprite());
    sprite->init(filename, Rect::zero);
    return sprite;
}

shared_ptr<Sprite> Sprite::create(string filename, const Rect &rect) {
    auto sprite = shared_ptr<Sprite>(new Sprite());
    sprite->init(filename, rect);
    return sprite;
}

shared_ptr<Sprite> Sprite::createWithFilePath(string filepath, Density density) {
    auto sprite = shared_ptr<Sprite>(new Sprite());
    sprite->initWithFilePath(filepath, Rect::zero, density);
    return sprite;
}

shared_ptr<Sprite> Sprite::createWithFilePath(string filepath, const Rect &rect, Density density) {
    auto sprite = shared_ptr<Sprite>(new Sprite());
    sprite->initWithFilePath(filepath, rect, density);
    return sprite;
}

shared_ptr<Sprite> Sprite::createWithImage(unsigned char *image, int length) {
    auto sprite = shared_ptr<Sprite>(new Sprite());
    sprite->initWithImage(image, length);
    return sprite;
}

shared_ptr<Sprite> Sprite::createWithImage(const Bytes &bytes) {
    return Sprite::createWithImage(bytes.value, bytes.length);
}

shared_ptr<Sprite> Sprite::createWithRGBA(unsigned char *data, int width, int height) {
    auto sprite = shared_ptr<Sprite>(new Sprite());
    sprite->initWithRGBA(data, width, height);
    return sprite;
}

shared_ptr<Sprite> Sprite::createWithTexture(const shared_ptr<Texture2D> &texture) {
    auto sprite = shared_ptr<Sprite>(new Sprite());
    sprite->initWithTexture(texture);
    return sprite;
}

void Sprite::registerCache(string filename) {
    auto texture = Texture2D::createWithAsset(filename);
    if (texture) {
        Sprite::globalCachedTexture2d[filename] = texture;
    }
}

void Sprite::removeCache(string filename) {
    if (Sprite::globalCachedTexture2d.count(filename) > 0) {
        Sprite::globalCachedTexture2d.erase(filename);
    }
}

void Sprite::clearCache() {
    Sprite::globalCachedTexture2d.clear();
}

Sprite::Sprite() {
}

Sprite::~Sprite() {
    if (this->filename.size() > 0 && Sprite::cachedTexture2d.count(this->filename) > 0) {
        Sprite::cachedTexture2d.erase(this->filename);
    }
}

string Sprite::getFilename() {
    return this->filename;
}

Rect Sprite::getRect() {
    return this->rect;
}

void Sprite::init(string filename, const Rect &rect) {
    this->filename = filename;
    if (Sprite::cachedTexture2d.count(filename) > 0) {
        this->texture = Sprite::cachedTexture2d[filename].lock();
    }
    if (this->texture == nullptr) {
        this->texture = Texture2D::createWithAsset(filename);
        if (this->texture) {
            Sprite::cachedTexture2d[filename] = this->texture;
        }
    }
    
    Rect _rect = rect;
    if (rect.size == Size::zero) {
        _rect.size = Size(this->texture->width / this->texture->density.value,
                         this->texture->height / this->texture->density.value);
    }
    this->rect = _rect;
    this->size = _rect.size;
    this->transform->size = this->rect.size;
}

void Sprite::initWithFilePath(string filepath, const Rect &rect, Density density) {
    this->filename = filepath;
    this->texture = Texture2D::createWithFile(filepath, density);
    
    Rect _rect = rect;
    if (rect.size == Size::zero) {
        _rect.size = Size(this->texture->width / this->texture->density.value,
                          this->texture->height / this->texture->density.value);
    }
    this->rect = _rect;
    this->size = _rect.size;
    this->transform->size = this->rect.size;
}

void Sprite::initWithImage(unsigned char *image, int length) {
    this->texture = Texture2D::createWithImage(image, length);
    
    this->transform->size.width = this->texture->width / this->texture->density.value;
    this->transform->size.height = this->texture->height / this->texture->density.value;
    this->size = this->transform->size;
    this->rect = Rect(Point::zero, this->size);
}

void Sprite::initWithRGBA(unsigned char *data, int width, int height) {
    this->texture = Texture2D::createWithRGBA(data, width, height);
    
    this->transform->size.width = this->texture->width / this->texture->density.value;
    this->transform->size.height = this->texture->height / this->texture->density.value;
    this->size = this->transform->size;
    this->rect = Rect(Point::zero, this->size);
}

void Sprite::initWithTexture(const shared_ptr<Texture2D> &texture) {
    this->texture = texture;
    
    this->transform->size.width = this->texture->width / this->texture->density.value;
    this->transform->size.height = this->texture->height / this->texture->density.value;
    this->size = this->transform->size;
    this->rect = Rect(Point::zero, this->size);
}

shared_ptr<Sprite> Sprite::clone() {
    auto entity = this->cloneEntity();
    return static_pointer_cast<Sprite>(entity);
}

shared_ptr<Entity> Sprite::cloneEntity() {
    auto sprite = shared_ptr<Sprite>(new Sprite());
    sprite->copyFrom(shared_from_this());
    return sprite;
}

void Sprite::bindVertexTexCoords(float *vertexTexCoords, int *idx, float x, float y, float w, float h) {
    Size texSize = Size(this->texture->width, this->texture->height) / this->texture->density.value;
    x += this->rect.position.x / texSize.width;
    y += this->rect.position.y / texSize.height;
    w *= this->rect.size.width / texSize.width;
    h *= this->rect.size.height / texSize.height;
    DrawEntity::bindVertexTexCoords(vertexTexCoords, idx, x, y, w, h);
}

void Sprite::copyFrom(const shared_ptr<Entity> &src) {
    DrawEntity::copyFrom(src);
    auto srcSprite = static_pointer_cast<Sprite>(src);
    this->filename = srcSprite->filename;
    this->rect = srcSprite->rect;
}

EntityType Sprite::getEntityType() {
    return EntityType::Sprite;
}
