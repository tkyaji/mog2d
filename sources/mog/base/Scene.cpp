#include "mog/base/Scene.h"
#include "mog/core/Engine.h"

using namespace mog;

Scene::Scene() {
    this->pubsub = std::make_shared<PubSub>();
    this->drawableGroup = std::make_shared<DrawableGroup>();
}

void Scene::add(const std::shared_ptr<Drawable> &drawable) {
    this->drawableGroup->addChild(drawable);
}

void Scene::insertChildBefore(const std::shared_ptr<Drawable> &drawable, const std::shared_ptr<Drawable> &baseDrawable) {
    this->drawableGroup->insertChildBefore(drawable, baseDrawable);
}

void Scene::remove(const std::shared_ptr<Drawable> &drawable) {
    this->drawableGroup->removeChild(drawable);
}

void Scene::removeAll() {
    this->drawableGroup->removeAllChildren();
}

void Scene::updateFrame(const std::shared_ptr<Engine> &engine, float delta, unsigned char parentDirtyFlag) {
    this->onUpdate(delta);
    this->drawableGroup->sortChildDrawablesToDraw();
    this->dirtyFlag |= parentDirtyFlag;
    for (auto drawable : this->drawableGroup->sortedChildDrawables) {
        drawable->dirtyFlag |= this->dirtyFlag;
        drawable->updateFrame(engine, delta, this->matrix, this->dirtyFlag);
    }
}

void Scene::drawFrame(float delta, const std::map<unsigned int, TouchInput> &touches) {
    for (auto drawable : this->drawableGroup->sortedChildDrawables) {
        if (((this->dirtyFlag | drawable->dirtyFlag) & DIRTY_VERTEX) == DIRTY_VERTEX) {
            Transform::multiplyMatrix(drawable->transform->matrix, this->matrix, drawable->renderer->matrix);
        }
        if (((this->dirtyFlag | drawable->dirtyFlag) & DIRTY_COLOR) == DIRTY_COLOR) {
            Transform::multiplyColor(drawable->transform->matrix, this->matrix, drawable->renderer->matrix);
        }
        drawable->drawFrame(delta, touches);
    }
    this->dirtyFlag = 0;
}

std::shared_ptr<AppBase> Scene::getApp() {
    return this->app.lock();
}

void Scene::setApp(const std::shared_ptr<AppBase> &app) {
    this->app = app;
}

std::shared_ptr<PubSub> Scene::getPubSub() {
    return this->pubsub;
}

std::vector<std::shared_ptr<Drawable>> Scene::getChildDrawables() {
    return this->drawableGroup->childDrawables;
}

