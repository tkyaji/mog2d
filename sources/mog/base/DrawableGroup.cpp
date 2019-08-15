#include "mog/base/DrawableGroup.h"

using namespace mog;

std::shared_ptr<DrawableGroup> DrawableGroup::create() {
    auto group = std::shared_ptr<DrawableGroup>(new DrawableGroup());
    group->init();
    return group;
}

DrawableGroup::DrawableGroup() {
    this->drawableContainer = std::make_shared<DrawableContainer>();
    this->drawableContainer->addChildListener = [this](const std::shared_ptr<Drawable> &drawable) {
        drawable->group = shared_from_this();
    };
    this->drawableContainer->removeChildListener = [this](const std::shared_ptr<Drawable> &drawable) {
        drawable->group.reset();
    };
}

void DrawableGroup::init() {
    this->dirtyFlag |= DIRTY_ALL;
}

void DrawableGroup::updateFrame(const std::shared_ptr<Engine> &engine, float delta, float *parentMatrix, unsigned char parentDirtyFlag) {
    this->drawableContainer->sortChildDrawablesToDraw();
    Drawable::updateFrame(engine, delta, parentMatrix, parentDirtyFlag);
    for (const auto &drawable : this->drawableContainer->sortedChildDrawables) {
        drawable->updateFrame(engine, delta, this->renderer->matrix, this->dirtyFlag);
    }
}

void DrawableGroup::drawFrame(float delta, const std::map<unsigned int, TouchInput> &touches) {
    if (!this->active) return;
    
    for (const auto &drawable : this->drawableContainer->sortedChildDrawables) {
        if (((this->dirtyFlag | drawable->dirtyFlag) & DIRTY_VERTEX) == DIRTY_VERTEX) {
            Transform::multiplyMatrix(drawable->transform->matrix, this->renderer->matrix, drawable->renderer->matrix);
        }
        if (((this->dirtyFlag | drawable->dirtyFlag) & DIRTY_COLOR) == DIRTY_COLOR) {
            Transform::multiplyColor(drawable->transform->matrix, this->renderer->matrix, drawable->renderer->matrix);
        }

        drawable->drawFrame(delta, touches);
    }
    
    this->dirtyFlag = 0;
}

void DrawableGroup::updateMatrix(float *parentMatrix, unsigned char parentDirtyFlag) {
    Drawable::updateMatrix(parentMatrix, parentDirtyFlag);
    unsigned char dirtyFlag = (this->dirtyFlag | parentDirtyFlag);
    float matrix[16];
    Transform::multiplyMatrix(this->renderer->matrix, parentMatrix, matrix);
    for (const auto &drawable : this->drawableContainer->sortedChildDrawables) {
        drawable->updateMatrix(matrix, dirtyFlag);
    }
}

void DrawableGroup::add(const std::shared_ptr<Drawable> &drawable) {
    this->drawableContainer->addChild(drawable);
    this->dirtyFlag |= DIRTY_ALL;
}

void DrawableGroup::insertBefore(const std::shared_ptr<Drawable> &drawable, const std::shared_ptr<Drawable> &baseDrawable) {
    this->drawableContainer->insertChildBefore(drawable, baseDrawable);
    this->dirtyFlag |= DIRTY_ALL;
}

void DrawableGroup::insertAfter(const std::shared_ptr<Drawable> &drawable, const std::shared_ptr<Drawable> &baseDrawable) {
    this->drawableContainer->insertChildAfter(drawable, baseDrawable);
    this->dirtyFlag |= DIRTY_ALL;
}

void DrawableGroup::remove(const std::shared_ptr<Drawable> &drawable) {
    this->drawableContainer->removeChild(drawable);
    this->dirtyFlag |= DIRTY_ALL;
}

void DrawableGroup::removeAll() {
    this->drawableContainer->removeAllChildren();
    this->dirtyFlag |= DIRTY_ALL;
}

std::vector<std::shared_ptr<Drawable>> DrawableGroup::getChildDrawables() {
    return this->drawableContainer->childDrawables;
}
