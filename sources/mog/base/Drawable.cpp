#include "mog/base/Drawable.h"
#include "mog/base/DrawableContainer.h"
#include "mog/core/MogStats.h"
#include <string.h>

using namespace mog;

Drawable::Drawable() {
    MogStats::instanceCount++;
    this->renderer = Renderer::create();
    this->transform = std::make_shared<Transform>();
}

Drawable::~Drawable() {
    MogStats::instanceCount--;
}

void Drawable::updateFrame(const std::shared_ptr<Engine> &engine, float delta, float *parentMatrix, unsigned char parentDirtyFlag) {
    this->onUpdate(delta);
    this->updateTween(delta);
    this->renderer->initScreenParameters();

    unsigned char mergedDirtyFlag = (this->dirtyFlag | parentDirtyFlag);
    
    if ((mergedDirtyFlag & (DIRTY_SIZE | DIRTY_ANCHOR)) > 0) {
        this->updateTransform();
    }
    
    if ((this->dirtyFlag & DIRTY_VERTEX) == DIRTY_VERTEX) {
        this->transform->updateMatrix();
    }
    if ((this->dirtyFlag & DIRTY_COLOR) == DIRTY_COLOR) {
        this->transform->updateColor();
    }
    
    if ((mergedDirtyFlag & DIRTY_VERTEX) == DIRTY_VERTEX) {
        Transform::multiplyMatrix(this->transform->matrix, parentMatrix, this->renderer->matrix);
    }
    if ((mergedDirtyFlag & DIRTY_COLOR) == DIRTY_COLOR) {
        Transform::multiplyColor(this->transform->matrix, parentMatrix, this->renderer->matrix);
    }
    
    this->dirtyFlag |= parentDirtyFlag;
}

void Drawable::drawFrame(float delta, const std::map<unsigned int, TouchInput> &touches) {
    if (!this->active) return;
    if ((this->dirtyFlag & DIRTY_VERTEX) == DIRTY_VERTEX) {
        this->renderer->shader->setUniformMatrix(this->renderer->matrix);
    }
    if ((this->dirtyFlag & DIRTY_COLOR) == DIRTY_COLOR) {
        this->renderer->shader->setUniformColor(this->renderer->matrix[16], this->renderer->matrix[17], this->renderer->matrix[18], this->renderer->matrix[19]);
    }
    this->bindVertex();
    this->renderer->drawFrame();
    this->dirtyFlag = 0;
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

void Drawable::setPivot(const Point &pivot) {
    this->transform->pivot = pivot;
    this->dirtyFlag |= DIRTY_VERTEX;
}

void Drawable::setPivot(float x, float y) {
    this->transform->pivot.x = x;
    this->transform->pivot.y = y;
    this->dirtyFlag |= DIRTY_VERTEX;
}

void Drawable::setPivotX(float x) {
    this->transform->pivot.x = x;
    this->dirtyFlag |= DIRTY_VERTEX;
}

void Drawable::setPivotY(float y) {
    this->transform->pivot.y = y;
    this->dirtyFlag |= DIRTY_VERTEX;
}

Point Drawable::getPivot() {
    return this->transform->pivot;
}

float Drawable::getPivotX() {
    return this->transform->pivot.x;
}

float Drawable::getPivotY() {
    return this->transform->pivot.y;
}

void Drawable::setAnchor(const Point &anchor) {
    this->anchor = anchor;
    this->dirtyFlag |= (DIRTY_VERTEX | DIRTY_ANCHOR);
}

void Drawable::setAnchor(float x, float y) {
    this->anchor.x = x;
    this->anchor.y = y;
    this->dirtyFlag |= (DIRTY_VERTEX | DIRTY_ANCHOR);
}

void Drawable::setAnchorX(float x) {
    this->anchor.x = x;
    this->dirtyFlag |= (DIRTY_VERTEX | DIRTY_ANCHOR);
}

void Drawable::setAnchorY(float y) {
    this->anchor.y = y;
    this->dirtyFlag |= (DIRTY_VERTEX | DIRTY_ANCHOR);
}

Point Drawable::getAnchor() {
    return this->anchor;
}

float Drawable::getAnchorX() {
    return this->anchor.x;
}

float Drawable::getAnchorY() {
    return this->anchor.y;
}

void Drawable::setPosition(const Point &position) {
    this->transform->position = position;
    this->dirtyFlag |= DIRTY_VERTEX;
}

void Drawable::setPosition(float x, float y) {
    this->transform->position.x = x;
    this->transform->position.y = y;
    this->dirtyFlag |= DIRTY_VERTEX;
}

void Drawable::setPositionX(float x) {
    this->transform->position.x = x;
    this->dirtyFlag |= DIRTY_VERTEX;
}

void Drawable::setPositionY(float y) {
    this->transform->position.y = y;
    this->dirtyFlag |= DIRTY_VERTEX;
}

Point Drawable::getPosition() {
    return this->transform->position;
}

Point Drawable::getPosition(const Point &pivot, const Point &anchor) {
    if ((this->dirtyFlag & DIRTY_VERTEX) == DIRTY_VERTEX) {
        this->updateTransform();
    }
    Point basePosition = this->transform->position - this->transform->size * this->transform->scale * this->transform->pivot;
    return basePosition + this->transform->size * this->transform->scale * pivot + this->transform->offset;
}

float Drawable::getPositionX() {
    return this->transform->position.x;
}

float Drawable::getPositionY() {
    return this->transform->position.y;
}

void Drawable::setScale(float scale) {
    this->setScale(Point(scale, scale));
    this->dirtyFlag |= DIRTY_VERTEX;
}

void Drawable::setScale(float scaleX, float scaleY) {
    this->transform->scale.x = scaleX;
    this->transform->scale.y = scaleY;
    this->dirtyFlag |= DIRTY_VERTEX;
}

void Drawable::setScale(const Point &scale) {
    this->transform->scale = scale;
    this->dirtyFlag |= DIRTY_VERTEX;
}

void Drawable::setScaleX(float scaleX) {
    this->transform->scale.x = scaleX;
    this->dirtyFlag |= DIRTY_VERTEX;
}

void Drawable::setScaleY(float scaleY) {
    this->transform->scale.y = scaleY;
    this->dirtyFlag |= DIRTY_VERTEX;
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
    this->dirtyFlag |= DIRTY_VERTEX;
}

float Drawable::getRotation() {
    return this->transform->rotation;
}

void Drawable::setColor(const Color &color) {
    this->transform->color = color;
    this->dirtyFlag |= DIRTY_COLOR;
}

void Drawable::setColor(float r, float g, float b, float a) {
    this->transform->color.r = r;
    this->transform->color.g = g;
    this->transform->color.b = b;
    this->transform->color.a = a;
    this->dirtyFlag |= DIRTY_COLOR;
}

void Drawable::setColorR(float r) {
    this->transform->color.r = r;
    this->dirtyFlag |= DIRTY_COLOR;
}

void Drawable::setColorG(float g) {
    this->transform->color.g = g;
    this->dirtyFlag |= DIRTY_COLOR;
}

void Drawable::setColorB(float b) {
    this->transform->color.b = b;
    this->dirtyFlag |= DIRTY_COLOR;
}

void Drawable::setColorA(float a) {
    this->transform->color.a = a;
    this->dirtyFlag |= DIRTY_COLOR;
}

void Drawable::setColor(std::string hexString) {
    if (hexString.length() == 0) return;
    if (hexString.c_str()[0] == '#') {
        hexString = hexString.substr(1, hexString.length() - 1);
    }
    if (hexString.length() < 6) return;
    
    long r = strtol(hexString.substr(0, 2).c_str(), NULL, 16);
    long g = strtol(hexString.substr(2, 2).c_str(), NULL, 16);
    long b = strtol(hexString.substr(4, 2).c_str(), NULL, 16);
    this->setColor((r/255.0f), (g/255.0f), (b/255.0f));
}

Color Drawable::getColor() {
    return this->transform->color;
}

float Drawable::getColorR() {
    return this->transform->color.r;
}

float Drawable::getColorG() {
    return this->transform->color.g;
}

float Drawable::getColorB() {
    return this->transform->color.b;
}

float Drawable::getColorA() {
    return this->transform->color.a;
}

std::string Drawable::getColorCode() {
    int r = (int)(this->transform->color.r * 255.0f + 0.5f);
    int g = (int)(this->transform->color.g * 255.0f + 0.5f);
    int b = (int)(this->transform->color.b * 255.0f + 0.5f);
    int a = (int)(this->transform->color.a * 255.0f + 0.5f);
    
    char colorCode[9];
    sprintf(colorCode, "%02x%02x%02x%02x", r, g, b, a);
    return std::string(colorCode);
}

void Drawable::setSize(const Size &size, unsigned char setInRatioFlag) {
    this->size = size;
    this->sizeSetInRatioFlag = setInRatioFlag;
    this->dirtyFlag |= (DIRTY_VERTEX | DIRTY_SIZE);
}

void Drawable::setSize(float width, float height, unsigned char setInRatioFlag) {
    this->size.width = width;
    this->size.height = height;
    this->sizeSetInRatioFlag = setInRatioFlag;
    this->dirtyFlag |= (DIRTY_VERTEX | DIRTY_SIZE);
}

Size Drawable::getSize() {
    return this->size;
}

Size Drawable::getRealSize() {
    if ((this->dirtyFlag & DIRTY_VERTEX) == DIRTY_VERTEX) {
        this->updateTransform();
    }
    return this->transform->size;
}

unsigned char Drawable::getSizeSetInRatioFlag() {
    return this->sizeSetInRatioFlag;
}

void Drawable::setWidth(float width, bool setInRatio) {
    this->size.width = width;
    if (setInRatio) {
        this->sizeSetInRatioFlag |= SET_IN_RATIO_WIDTH;
    } else {
        this->sizeSetInRatioFlag = (this->sizeSetInRatioFlag & ~SET_IN_RATIO_WIDTH);
    }
    this->dirtyFlag |= (DIRTY_VERTEX | DIRTY_SIZE);
}

float Drawable::getWidth() {
    return this->size.width;
}

float Drawable::getRealWidth() {
    return this->getRealSize().width;
}

void Drawable::setHeight(float height, bool setInRatio) {
    this->size.height = height;
    if (setInRatio) {
        this->sizeSetInRatioFlag |= SET_IN_RATIO_HEIGHT;
    } else {
        this->sizeSetInRatioFlag = (this->sizeSetInRatioFlag & ~SET_IN_RATIO_HEIGHT);
    }
    this->dirtyFlag |= (DIRTY_VERTEX | DIRTY_SIZE);
}

float Drawable::getHeight() {
    return this->size.height;
}

float Drawable::getRealHeight() {
    return this->getRealSize().height;
}

void Drawable::setZIndex(int zIndex) {
    if (this->zIndex != zIndex) {
        if (auto dg = this->parentDrawableContainer.lock()) {
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
    this->dirtyFlag |= DIRTY_VERTEX;
}

bool Drawable::isActive() {
    return this->active;
}

void Drawable::updateMatrix() {
    if (auto g = this->group.lock()) {
        g->updateMatrix();
    } else {
        this->updateMatrix(Renderer::identityMatrix, 0);
    }
}

void Drawable::updateMatrix(float *parentMatrix, unsigned char parentDirtyFlag) {
    unsigned char mergedDirtyFlag = (this->dirtyFlag | parentDirtyFlag);
    
    if ((mergedDirtyFlag & (DIRTY_SIZE | DIRTY_ANCHOR)) > 0) {
        this->updateTransform();
    }
    if ((this->dirtyFlag & DIRTY_VERTEX) == DIRTY_VERTEX) {
        this->transform->updateMatrix();
    }
    if ((dirtyFlag & DIRTY_VERTEX) == DIRTY_VERTEX) {
        Transform::multiplyMatrix(this->transform->matrix, parentMatrix, this->renderer->matrix);
    }
}

void Drawable::runTween(const std::shared_ptr<Tween> &tween) {
    this->tweens[tween->getTweenId()] = tween;
    tween->addOnFinishEventForParent([this](const std::shared_ptr<Tween> &t) {
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
    if (auto dg = this->parentDrawableContainer.lock()) {
        dg->removeChild(shared_from_this());
    }
}

std::shared_ptr<Renderer> Drawable::getRenderer() {
    return this->renderer;
}

std::shared_ptr<Transform> Drawable::getTransform() {
    return this->transform;
}

unsigned char Drawable::getDirtyFlag(bool withParent) {
    unsigned char dirtyFlag = this->dirtyFlag;
    if (withParent) {
        if (auto g = this->group.lock()) {
            dirtyFlag |= g->getDirtyFlag(true);
        }
    }
    return dirtyFlag;
}

float *Drawable::getMatrix() {
    return this->renderer->matrix;
}

std::shared_ptr<Texture2D> Drawable::getTexture(int textureIdx) {
    if (textureIdx >= this->textures.size()) return nullptr;
    return this->textures[textureIdx];
}

void Drawable::setTexture(int textureIdx, const std::shared_ptr<Texture2D> &texture) {
    this->textures[textureIdx] = texture;
    this->dirtyFlag |= (DIRTY_TEXTURE | DIRTY_TEX_COORDS);
}

void Drawable::updateTransform() {
    Size parentSize;
    if (auto parent = this->group.lock()) {
        parentSize = parent->getRealSize();
    } else {
        parentSize = Screen::getSize();
    }

    if ((this->dirtyFlag & DIRTY_ANCHOR) == DIRTY_ANCHOR) {
        this->transform->offset = parentSize* this->anchor;
    }
    
    if ((this->dirtyFlag & DIRTY_SIZE) == DIRTY_SIZE) {
        auto size = this->size;
        if ((this->sizeSetInRatioFlag & SET_IN_RATIO_WIDTH) == SET_IN_RATIO_WIDTH) {
            size.width = parentSize.width * size.width;
        }
        if ((this->sizeSetInRatioFlag & SET_IN_RATIO_HEIGHT) == SET_IN_RATIO_HEIGHT) {
            size.height = parentSize.height * size.height;
        }
        this->transform->size = size;
    }
    
    this->dirtyFlag = (this->dirtyFlag & ~(DIRTY_ANCHOR | DIRTY_SIZE));
}
