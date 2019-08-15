#include "mog/base/Scene.h"
#include "mog/core/Engine.h"

using namespace mog;

Scene::Scene() {
    this->pubsub = std::make_shared<PubSub>();
    this->rootGroup = DrawableGroup::create();
    this->rootGroup->setSize(1.0f, 1.0f, SET_IN_RATIO_BOTH);
}

void Scene::add(const std::shared_ptr<Drawable> &drawable) {
    this->rootGroup->add(drawable);
}

void Scene::insertBefore(const std::shared_ptr<Drawable> &drawable, const std::shared_ptr<Drawable> &baseDrawable) {
    this->rootGroup->insertBefore(drawable, baseDrawable);
}

void Scene::insertAfter(const std::shared_ptr<Drawable> &drawable, const std::shared_ptr<Drawable> &baseDrawable) {
    this->rootGroup->insertAfter(drawable, baseDrawable);
}

void Scene::remove(const std::shared_ptr<Drawable> &drawable) {
    this->rootGroup->remove(drawable);
}

void Scene::removeAll() {
    this->rootGroup->removeAll();
}

void Scene::updateFrame(const std::shared_ptr<Engine> &engine, float delta, unsigned char parentDirtyFlag) {
    this->onUpdate(delta);
    this->dirtyFlag |= parentDirtyFlag;
    this->rootGroup->updateFrame(engine, delta, this->matrix, this->dirtyFlag);
}

void Scene::drawFrame(float delta, const std::map<unsigned int, TouchInput> &touches) {
    this->rootGroup->drawFrame(delta, touches);
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

std::shared_ptr<DrawableGroup> Scene::getRootDrawableGroup() {
    return this->rootGroup;
}

