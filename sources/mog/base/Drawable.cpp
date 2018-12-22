#include "mog/base/Drawable.h"
#include "mog/base/DrawableGroup.h"
#include "mog/core/MogStats.h"
#include <string.h>

using namespace mog;

Drawable::Drawable() {
    MogStats::instanceCount++;
    this->renderer = make_shared<Renderer>();
    this->transform = make_shared<Transform>();
}

Drawable::~Drawable() {
    MogStats::instanceCount--;
}

void Drawable::updateFrame(const shared_ptr<Engine> &engine, float delta, float *parentMatrix, unsigned char parentReRenderFlag) {
    this->updateTween(delta);
    this->renderer->initScreenParameters(engine);
    this->reRenderFlag |= parentReRenderFlag;
    if ((this->reRenderFlag & RERENDER_VERTEX) == RERENDER_VERTEX) {
        this->transform->updateMatrix();
        memcpy(this->renderer->matrix, this->transform->matrix, sizeof(float) * 16);
        Transform::multiplyMatrix(this->transform->matrix, parentMatrix, this->matrix);
    }
    if ((this->reRenderFlag & RERENDER_COLOR) == RERENDER_COLOR) {
        this->transform->updateColor();
        memcpy(&this->renderer->matrix[16], &this->transform->matrix[16], sizeof(float) * 4);
    }
}

void Drawable::drawFrame(float delta) {
    if (!this->active) return;
    if ((this->reRenderFlag & RERENDER_VERTEX) == RERENDER_VERTEX) {
        this->renderer->setUniformMatrix(this->renderer->matrix);
    }
    if ((this->reRenderFlag & RERENDER_COLOR) == RERENDER_COLOR) {
        this->renderer->setUniformColor(this->renderer->matrix[16], this->renderer->matrix[17], this->renderer->matrix[18], this->renderer->matrix[19]);
    }
    this->bindVertex();
    this->renderer->drawFrame();
    this->reRenderFlag = 0;
}

void Drawable::updateTween(float delta) {
    if (this->tweenIdsToRemove.size() > 0) {
        for (unsigned id : this->tweenIdsToRemove) {
            this->tweens.erase(id);
        }
        this->tweenIdsToRemove.clear();
    }
    if (this->tweens.size() > 0) {
        for (const auto &t : this->tweens) {
            t.second->update(delta, shared_from_this());
        }
    }
}

void Drawable::bindVertex() {
}

void Drawable::setAnchor(const Point &anchor) {
    this->transform->anchor = anchor;
    this->reRenderFlag |= RERENDER_VERTEX;
}

void Drawable::setAnchor(float x, float y) {
    this->transform->anchor.x = x;
    this->transform->anchor.y = y;
    this->reRenderFlag |= RERENDER_VERTEX;
}

void Drawable::setAnchorX(float x) {
    this->transform->anchor.x = x;
    this->reRenderFlag |= RERENDER_VERTEX;
}

void Drawable::setAnchorY(float y) {
    this->transform->anchor.y = y;
    this->reRenderFlag |= RERENDER_VERTEX;
}

Point Drawable::getAnchor() {
    return this->transform->anchor;
}

float Drawable::getAnchorX() {
    return this->transform->anchor.x;
}

float Drawable::getAnchorY() {
    return this->transform->anchor.y;
}

void Drawable::setPosition(const Point &position) {
    this->transform->position = position;
    this->reRenderFlag |= RERENDER_VERTEX;
}

void Drawable::setPosition(float x, float y) {
    this->transform->position.x = x;
    this->transform->position.y = y;
    this->reRenderFlag |= RERENDER_VERTEX;
}

void Drawable::setPositionX(float x) {
    this->transform->position.x = x;
    this->reRenderFlag |= RERENDER_VERTEX;
}

void Drawable::setPositionY(float y) {
    this->transform->position.y = y;
    this->reRenderFlag |= RERENDER_VERTEX;
}

Point Drawable::getPosition() {
    return this->transform->position;
}

Point Drawable::getPosition(const Point &anchor) {
    Point basePosition = this->transform->position - this->transform->size * this->transform->scale * this->transform->anchor;
    return basePosition + this->transform->size * this->transform->scale * anchor;
}

float Drawable::getPositionX() {
    return this->transform->position.x;
}

float Drawable::getPositionY() {
    return this->transform->position.y;
}

void Drawable::setScale(float scale) {
    this->setScale(Point(scale, scale));
    this->reRenderFlag |= RERENDER_VERTEX;
}

void Drawable::setScale(float scaleX, float scaleY) {
    this->transform->scale.x = scaleX;
    this->transform->scale.y = scaleY;
    this->reRenderFlag |= RERENDER_VERTEX;
}

void Drawable::setScale(const Point &scale) {
    this->transform->scale = scale;
    this->reRenderFlag |= RERENDER_VERTEX;
}

void Drawable::setScaleX(float scaleX) {
    this->transform->scale.x = scaleX;
    this->reRenderFlag |= RERENDER_VERTEX;
}

void Drawable::setScaleY(float scaleY) {
    this->transform->scale.y = scaleY;
    this->reRenderFlag |= RERENDER_VERTEX;
}

Point Drawable::getScale() {
    return this->transform->scale;
}

float Drawable::getScaleX() {
    return this->transform->scale.x;
}

float Drawable::getScaleY() {
    return this->transform->scale.y;
}

void Drawable::setRotation(float angle) {
    this->transform->rotation = fmod(angle, 360.0f);
    this->reRenderFlag |= RERENDER_VERTEX;
}

float Drawable::getRotation() {
    return this->transform->rotation;
}

void Drawable::setColor(const Color &color) {
    this->transform->color = color;
    this->reRenderFlag |= RERENDER_COLOR;
}

void Drawable::setColor(float r, float g, float b, float a) {
    this->transform->color.r = r;
    this->transform->color.g = g;
    this->transform->color.b = b;
    this->transform->color.a = a;
    this->reRenderFlag |= RERENDER_COLOR;
}

void Drawable::setColorR(float r) {
    this->transform->color.r = r;
    this->reRenderFlag |= RERENDER_COLOR;
}

void Drawable::setColorG(float g) {
    this->transform->color.g = g;
    this->reRenderFlag |= RERENDER_COLOR;
}

void Drawable::setColorB(float b) {
    this->transform->color.b = b;
    this->reRenderFlag |= RERENDER_COLOR;
}

void Drawable::setColorA(float a) {
    this->transform->color.a = a;
    this->reRenderFlag |= RERENDER_COLOR;
}

Color Drawable::getColor() {
    return this->transform->color;
}

void Drawable::setSize(const Size &size) {
    this->transform->size = size;
    this->reRenderFlag |= RERENDER_VERTEX;
}

void Drawable::setSize(float width, float height) {
    this->transform->size.width = width;
    this->transform->size.height = height;
    this->reRenderFlag |= RERENDER_VERTEX;
}

Size Drawable::getSize() {
    return this->transform->size;
}

void Drawable::setWidth(float width) {
    this->transform->size.width = width;
    this->reRenderFlag |= RERENDER_VERTEX;
}

float Drawable::getWidth() {
    return this->transform->size.width;
}

void Drawable::setHeight(float height) {
    this->transform->size.height = height;
    this->reRenderFlag |= RERENDER_VERTEX;
}

float Drawable::getHeight() {
    return this->transform->size.height;
}

void Drawable::setZIndex(int zIndex) {
    if (this->zIndex != zIndex) {
        if (auto dg = this->drawableGroup.lock()) {
            dg->sortOrderDirty = true;
        }
    }
    this->zIndex = zIndex;
}

int Drawable::getZIndex() {
    return this->zIndex;
}

void Drawable::setActive(bool active) {
    this->active = active;
    this->reRenderFlag |= RERENDER_VERTEX;
}

bool Drawable::isActive() {
    return this->active;
}

void Drawable::runTween(const std::shared_ptr<Tween> &tween) {
    this->tweens[tween->getTweenId()] = tween;
    tween->addOnFinishEventForParent([this](const shared_ptr<Tween> &t) {
        this->tweenIdsToRemove.emplace_back(t->getTweenId());
    });
    tween->init();
    tween->update(0, shared_from_this());
}

void Drawable::cancelTween(unsigned int tweenId) {
    this->tweens.erase(tweenId);
}

void Drawable::cancelAllTweens() {
    this->tweens.clear();
}

void Drawable::removeFromParent() {
    if (auto dg = this->drawableGroup.lock()) {
        dg->removeChild(shared_from_this());
    }
}

std::shared_ptr<Renderer> Drawable::getRenderer() {
    return this->renderer;
}

std::shared_ptr<Transform> Drawable::getTransform() {
    return this->transform;
}

unsigned char Drawable::getReRenderFlag() {
    return this->reRenderFlag;
}

float *Drawable::getMatrix() {
    return this->matrix;
}

