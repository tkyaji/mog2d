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

shared_ptr<Sprite> Sprite::createWithImage(const Bytes &bytes) {
    auto sprite = shared_ptr<Sprite>(new Sprite());
    sprite->initWithImage(bytes);
    return sprite;
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
    this->transform->size = this->rect.size;
    
    this->initRendererVertices(4, 4);
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
    this->transform->size = this->rect.size;
    
    this->initRendererVertices(4, 4);
}

void Sprite::initWithImage(const Bytes &bytes) {
    this->texture = Texture2D::createWithImage(bytes);
    
    this->transform->size.width = this->texture->width / this->texture->density.value;
    this->transform->size.height = this->texture->height / this->texture->density.value;
    this->rect = Rect(Point::zero, this->transform->size);
    
    this->initRendererVertices(4, 4);
}

void Sprite::initWithRGBA(unsigned char *data, int width, int height) {
    this->texture = Texture2D::createWithRGBA(data, width, height);
    
    this->transform->size.width = this->texture->width / this->texture->density.value;
    this->transform->size.height = this->texture->height / this->texture->density.value;
    this->rect = Rect(Point::zero, this->transform->size);
    
    this->initRendererVertices(4, 4);
}

void Sprite::initWithTexture(const shared_ptr<Texture2D> &texture) {
    this->texture = texture;
    
    this->transform->size.width = this->texture->width / this->texture->density.value;
    this->transform->size.height = this->texture->height / this->texture->density.value;
    this->rect = Rect(Point::zero, this->transform->size);
    
    this->initRendererVertices(4, 4);
}

void Sprite::bindVertexTexCoords(const std::shared_ptr<Renderer> &renderer, int *idx, float x, float y, float w, float h) {
    Size texSize = Size(this->texture->width, this->texture->height) / this->texture->density.value;
    x += this->rect.position.x / texSize.width;
    y += this->rect.position.y / texSize.height;
    w *= this->rect.size.width / texSize.width;
    h *= this->rect.size.height / texSize.height;
    Entity::bindVertexTexCoords(renderer, idx, x, y, w, h);
}
