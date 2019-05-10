#include <math.h>
#include "mog/base/AppBase.h"
#include "mog/core/Engine.h"
#include "mog/base/Scene.h"
#include "mog/base/Entity.h"
#include "mog/base/Rectangle.h"

using namespace mog;

void AppBase::setEngine(const std::shared_ptr<Engine> &engine) {
    this->engine = engine;
}

AppBase::AppBase() {
    this->pubsub = std::make_shared<PubSub>();
}

void AppBase::loadScene(const std::shared_ptr<Scene> &scene) {
    this->reserveLoadScene(scene, Transition::None, 0, Easing::Linear, LoadMode::Load);
}

void AppBase::loadScene(const std::shared_ptr<Scene> &scene, Transition transition, float duration, Easing easing) {
    this->reserveLoadScene(scene, transition, duration, easing, LoadMode::Load);
}

void AppBase::pushScene(const std::shared_ptr<Scene> &scene) {
    if (this->currentScene) {
        this->sceneStack.emplace_back(this->currentScene);
    }
    this->reserveLoadScene(scene, Transition::None, 0, Easing::Linear, LoadMode::Push);
}

void AppBase::pushScene(const std::shared_ptr<Scene> &scene, Transition transition, float duration, Easing easing) {
    if (this->currentScene) {
        this->sceneStack.emplace_back(this->currentScene);
    }
    this->reserveLoadScene(scene, transition, duration, easing, LoadMode::Push);
}

void AppBase::popScene() {
    if (this->sceneStack.size() == 0) return;
    
    auto scene = this->sceneStack.back();
    this->sceneStack.pop_back();
    this->reserveLoadScene(scene, Transition::None, 0, Easing::Linear, LoadMode::Pop);
}

void AppBase::popScene(Transition transition, float duration, Easing easing) {
    if (this->sceneStack.size() == 0) return;

    auto scene = this->sceneStack.back();
    this->sceneStack.pop_back();
    this->reserveLoadScene(scene, transition, duration, easing, LoadMode::Pop);
}

void AppBase::reserveLoadScene(const std::shared_ptr<Scene> &scene, Transition transition, float duration,
                               Easing easing, LoadMode loadMode) {
    if (this->currentScene) {
        this->loadSceneParams.setParams(scene, transition, duration, easing, loadMode);
        this->isReservedLoadScene = true;
    } else {
        this->loadSceneMain(scene, loadMode);
    }
}

bool AppBase::doLoadScene() {
    if (this->isReservedLoadScene) {
        auto params = this->loadSceneParams;
        this->isReservedLoadScene = false;
        this->loadSceneParams.clear();
        this->loadSceneMain(params);
        return true;
    }
    return false;
}

void AppBase::loadSceneMain(const LoadSceneParams &params) {
    this->loadSceneMain(params.scene, params.transition, params.duration, params.easing, params.loadMode);
}

void AppBase::loadSceneMain(const std::shared_ptr<Scene> &scene, LoadMode loadMode) {
    if (this->currentScene) {
        this->currentScene->onDisable();
        if (loadMode != LoadMode::Push) {
            this->currentScene->onDispose();
        }
    }
    this->currentScene = scene;
    this->currentScene->setApp(shared_from_this());
    
    if (loadMode != LoadMode::Pop) {
        this->currentScene->onLoad();
    }
    this->currentScene->onEnable();
}

void AppBase::loadSceneMain(const std::shared_ptr<Scene> &scene, Transition transition, float duration, Easing easing, LoadMode loadMode) {
    switch (transition) {
        case Transition::Fade:
            this->loadSceneWithFade(scene, duration, easing, loadMode);
            break;
            /*
        case Transition::CrossFade:
            this->loadSceneWithCrossFade(scene, duration, easing, loadMode);
            break;
             */
        case Transition::MoveInTop:
        case Transition::MoveInBottom:
        case Transition::MoveInLeft:
        case Transition::MoveInRight:
            this->loadSceneWithMoveIn(scene, transition, duration, easing, loadMode);
            break;
        case Transition::MoveOutTop:
        case Transition::MoveOutBottom:
        case Transition::MoveOutLeft:
        case Transition::MoveOutRight:
            this->loadSceneWithMoveOut(scene, transition, duration, easing, loadMode);
            break;
        case Transition::SlideInTop:
        case Transition::SlideInBottom:
        case Transition::SlideInLeft:
        case Transition::SlideInRight:
            this->loadSceneWithSlideIn(scene, transition, duration, easing, loadMode);
            break;
        case Transition::None:
            this->loadSceneMain(scene, loadMode);
            break;
    }
}

void AppBase::loadSceneWithFade(const std::shared_ptr<Scene> &scene, float duration, Easing easing, LoadMode loadMode) {
    auto f = [](std::shared_ptr<mog::Scene> current, std::shared_ptr<mog::Scene> next, float value) {
        if (value < 0.5f) {
            float v = 1.0 - value / 0.5f;
            current->matrix[16] = v;
            current->matrix[17] = v;
            current->matrix[18] = v;
            current->reRenderFlag |= RERENDER_COLOR;
        } else {
            float v = (value - 0.5f) / 0.5f;
            next->matrix[16] = v;
            next->matrix[17] = v;
            next->matrix[18] = v;
            next->reRenderFlag |= RERENDER_COLOR;
        }
    };
    this->sceneTransition = SceneTransition::create(this->engine.lock(), shared_from_this(), this->currentScene, scene,
                                                    duration, easing, loadMode, 0.5f, SceneTransition::SceneOrder::CurrentNext, f);
}

/*
void AppBase::loadSceneWithCrossFade(const shared_ptr<Scene> &scene, float duration, Easing easing, LoadMode loadMode) {
}
*/

void AppBase::loadSceneWithMoveIn(const std::shared_ptr<Scene> &scene, Transition transition, float duration, Easing easing, LoadMode loadMode) {
    Point start = Point::zero;
    switch (transition) {
        case Transition::MoveInTop:
            start.y = -this->getScreenSize().height;
            break;
        case Transition::MoveInBottom:
            start.y = this->getScreenSize().height;
            break;
        case Transition::MoveInRight:
            start.x = this->getScreenSize().width;
            break;
        case Transition::MoveInLeft:
        default:
            start.x = -this->getScreenSize().width;
            break;
    }
    
    auto f = [start](std::shared_ptr<mog::Scene> current, std::shared_ptr<mog::Scene> next, float value) {
        float v = 1.0f - value;
        next->matrix[12] = start.x * v;
        next->matrix[13] = start.y * v;
        next->reRenderFlag |= RERENDER_VERTEX;
    };
    this->sceneTransition = SceneTransition::create(this->engine.lock(), shared_from_this(), this->currentScene, scene,
                                                    duration, easing, loadMode, 0, SceneTransition::SceneOrder::CurrentNext, f);
}

void AppBase::loadSceneWithMoveOut(const std::shared_ptr<Scene> &scene, Transition transition, float duration, Easing easing, LoadMode loadMode) {
    Point end = Point::zero;
    switch (transition) {
        case Transition::MoveOutTop:
            end.y = -this->getScreenSize().height;
            break;
        case Transition::MoveOutBottom:
            end.y = this->getScreenSize().height;
            break;
        case Transition::MoveOutLeft:
            end.x = -this->getScreenSize().width;
            break;
        case Transition::MoveOutRight:
        default:
            end.x = this->getScreenSize().width;
            break;
    }
    
    auto f = [end](std::shared_ptr<mog::Scene> current, std::shared_ptr<mog::Scene> next, float value) {
        current->matrix[12] = end.x * value;
        current->matrix[13] = end.y * value;
        current->reRenderFlag |= RERENDER_VERTEX;
    };
    this->sceneTransition = SceneTransition::create(this->engine.lock(), shared_from_this(), this->currentScene, scene,
                                                    duration, easing, loadMode, 0, SceneTransition::SceneOrder::NextCurrent, f);
}

void AppBase::loadSceneWithSlideIn(const std::shared_ptr<Scene> &scene, Transition transition, float duration, Easing easing, LoadMode loadMode) {
    Point currentEnd = Point::zero;
    Point nextStart = Point::zero;
    switch (transition) {
        case Transition::SlideInTop:
            currentEnd.y = this->getScreenSize().height;
            nextStart.y = -this->getScreenSize().height;
            break;
        case Transition::SlideInBottom:
            currentEnd.y = -this->getScreenSize().height;
            nextStart.y = this->getScreenSize().height;
            break;
        case Transition::SlideInRight:
            currentEnd.x = -this->getScreenSize().width;
            nextStart.x = this->getScreenSize().width;
            break;
        case Transition::SlideInLeft:
        default:
            currentEnd.x = this->getScreenSize().width;
            nextStart.x = -this->getScreenSize().width;
            break;
    }
    
    auto f = [currentEnd, nextStart](std::shared_ptr<mog::Scene> current, std::shared_ptr<mog::Scene> next, float value) {
        float v = 1.0f - value;
        current->matrix[12] = currentEnd.x * value;
        current->matrix[13] = currentEnd.y * value;
        next->matrix[12] = nextStart.x * v;
        next->matrix[13] = nextStart.y * v;
        current->reRenderFlag |= RERENDER_VERTEX;
        next->reRenderFlag |= RERENDER_VERTEX;
    };
    this->sceneTransition = SceneTransition::create(this->engine.lock(), shared_from_this(), this->currentScene, scene,
                                                    duration, easing, loadMode, 0, SceneTransition::SceneOrder::CurrentNext, f);
}

void AppBase::drawFrame(float delta) {
    if (this->currentScene) {
        auto engine = this->engine.lock();
        
        if (this->sceneTransition) {
            if (this->sceneTransition->update(delta)) {
                return;
            } else {
                this->sceneTransition = nullptr;
            }
        } else {
            this->currentScene->updateFrame(engine, delta);
            this->currentScene->drawFrame(delta);
        }
        if (this->doLoadScene()) {
            return;
        }
        
    } else {
        if (this->doLoadScene()) {
            return;
        }
    }
    this->onUpdate(delta);
}

Color AppBase::getBackgroundColor() {
    return this->engine.lock()->getClearColor();
}

void AppBase::setBackgroundColor(const Color &color) {
    this->engine.lock()->setClearColor(color);
}

std::shared_ptr<Scene> AppBase::getCurrentScene() {
    return this->currentScene;
}

void AppBase::setScreenSizeBasedOnHeight(float height) {
    this->engine.lock()->setScreenSizeBasedOnHeight(height);
}

void AppBase::setScreenSizeBasedOnWidth(float width) {
    this->engine.lock()->setScreenSizeBasedOnWidth(width);
}

Size AppBase::getDisplaySize() {
    return this->engine.lock()->getDisplaySize();
}

Size AppBase::getScreenSize() {
    return this->engine.lock()->getScreenSize();
}

float AppBase::getScreenScale() {
    return this->engine.lock()->getScreenScale();
}

void AppBase::setTouchEnable(bool enable) {
    this->engine.lock()->setTouchEnable(enable);
}

void AppBase::setMultiTouchEnable(bool enable) {
    this->engine.lock()->setMultiTouchEnable(enable);
}

bool AppBase::isTouchEnable() {
    return this->engine.lock()->isTouchEnable();
}

bool AppBase::isMultiTouchEnable() {
    return this->engine.lock()->isMultiTouchEnable();
}

void AppBase::setStatsViewEnable(bool enable) {
    this->engine.lock()->setStatsEnable(enable);
}

void AppBase::setStatsViewAlignment(Alignment alignment) {
    this->engine.lock()->setStatsAlignment(alignment);
}

std::shared_ptr<PubSub> AppBase::getPubSub() {
    return this->pubsub;
}

unsigned int AppBase::getSceneStackSize() {
    return (unsigned int)this->sceneStack.size();
}


std::unique_ptr<AppBase::SceneTransition> AppBase::SceneTransition::create(const std::shared_ptr<Engine> &engine, const std::shared_ptr<AppBase> &app,
                                                                           const std::shared_ptr<Scene> &currentScene, const std::shared_ptr<Scene> &nextScene,
                                                                           float duration, Easing easing, AppBase::LoadMode loadMode, float loadSceneValue,
                                                                           AppBase::SceneTransition::SceneOrder sceneOrder,
                                                                           std::function<void(std::shared_ptr<Scene> current, std::shared_ptr<Scene> next, float value)> onModify) {
    auto sceneTransition = std::unique_ptr<AppBase::SceneTransition>(new SceneTransition());
    sceneTransition->engine = engine;
    sceneTransition->app = app;
    sceneTransition->currentScene = currentScene;
    sceneTransition->nextScene = nextScene;
    sceneTransition->duration = duration;
    sceneTransition->easingFunc = EasingFunc::getEasingFunc(easing);
    sceneTransition->loadMode = loadMode;
    sceneTransition->loadSceneValue = loadSceneValue;
    sceneTransition->sceneOrder = sceneOrder;
    sceneTransition->onModify = onModify;
    sceneTransition->elapsedTime = 0;
    sceneTransition->loaded = false;
    sceneTransition->touchEnable = engine->isTouchEnable();
    sceneTransition->init();
    return sceneTransition;
}

void AppBase::SceneTransition::init() {
    this->engine.lock()->setTouchEnable(false);
    this->currentScene->onDisable();
    this->update(0);
}

bool AppBase::SceneTransition::update(float delta) {
    auto engine = this->engine.lock();
    auto app = this->app.lock();
    this->elapsedTime += delta;
    float value = this->easingFunc->process(this->elapsedTime / this->duration);
    if (value > 1.0f) value = 1.0f;
    if (!this->loaded && value >= this->loadSceneValue) {
        this->nextScene->setApp(app);
        if (this->loadMode == LoadMode::Load) {
            this->nextScene->onLoad();
        }
        this->loaded = true;
    }
    this->onModify(this->currentScene, this->nextScene, value);
    
    if (this->loaded) {
        if (this->sceneOrder == SceneOrder::CurrentNext) {
            this->currentScene->updateFrame(engine, 0);
            this->currentScene->drawFrame(0);
            this->nextScene->updateFrame(engine, 0);
            this->nextScene->drawFrame(0);
        } else {
            this->nextScene->updateFrame(engine, 0);
            this->nextScene->drawFrame(0);
            this->currentScene->updateFrame(engine, 0);
            this->currentScene->drawFrame(0);
        }
    } else {
        this->currentScene->updateFrame(engine, 0);
        this->currentScene->drawFrame(0);
    }
    if (this->elapsedTime >= this->duration) {
        this->nextScene->onEnable();
        if (this->loadMode == LoadMode::Load) {
            this->currentScene->onDispose();
        }
        app->currentScene = this->nextScene;
        engine->setTouchEnable(this->touchEnable);
        return false;
    }
    return true;
}
