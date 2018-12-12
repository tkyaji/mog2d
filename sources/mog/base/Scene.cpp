#include "mog/base/Scene.h"
#include "mog/core/Engine.h"

using namespace mog;

Scene::Scene() {
    this->pubsub = make_shared<PubSub>();
    this->drawableGroup = make_shared<DrawableGroup>();
}

void Scene::add(const std::shared_ptr<Drawable> &drawable) {
    this->drawableGroup->addChild(drawable);
}

void Scene::remove(const std::shared_ptr<Drawable> &drawable) {
    this->drawableGroup->removeChild(drawable);
}

void Scene::removeAll() {
    this->drawableGroup->removeAllChildren();
}

void Scene::updateFrame(const shared_ptr<Engine> &engine, float delta) {
    this->onUpdate(delta);
    this->drawableGroup->sortChildDrawablesToDraw();
    for (auto drawable : this->drawableGroup->sortedChildDrawables) {
        drawable->reRenderFlag |= this->reRenderFlag;
        drawable->updateFrame(engine, delta, this->matrix, this->reRenderFlag);
    }
}

void Scene::drawFrame(float delta) {
    for (auto drawable : this->drawableGroup->sortedChildDrawables) {
        if (((this->reRenderFlag | drawable->reRenderFlag) & RERENDER_VERTEX) == RERENDER_VERTEX) {
            Transform::multiplyMatrix(drawable->transform->matrix, this->matrix, drawable->renderer->matrix);
        }
        if (((this->reRenderFlag | drawable->reRenderFlag) & RERENDER_COLOR) == RERENDER_COLOR) {
            Transform::multiplyColor(drawable->transform->matrix, this->matrix, drawable->renderer->matrix);
        }
        drawable->drawFrame(delta);
    }
    this->reRenderFlag = 0;
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
