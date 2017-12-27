#include "mog/base/Scene.h"
#include "mog/core/Engine.h"

using namespace mog;

Scene::Scene() {
    this->rootGroup = Group::create();
    this->pubsub = make_shared<PubSub>();
}

void Scene::add(const shared_ptr<Entity> &entity) {
    this->rootGroup->add(entity);
}

void Scene::remove(const shared_ptr<Entity> &entity) {
    this->rootGroup->remove(entity);
}

void Scene::removeAll() {
    this->rootGroup->removeAll();
}

shared_ptr<AppBase> Scene::getApp() {
    return this->app.lock();
}

void Scene::setApp(const shared_ptr<AppBase> &app) {
    this->app = app;
    this->rootGroup->setSize(app->getScreenSize());
}

shared_ptr<Group> Scene::getRootGroup() {
    return this->rootGroup;
}

void Scene::setRootGroup(const shared_ptr<Group> &rootGroup) {
    this->rootGroup = rootGroup;
}

shared_ptr<PubSub> Scene::getPubSub() {
    return this->pubsub;
}

void Scene::runTween(const shared_ptr<Tween> &tween) {
    this->rootGroup->runTween(tween);
}
