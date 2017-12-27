#include <math.h>
#include "mog/base/AppBase.h"
#include "mog/base/Scene.h"
#include "mog/core/Engine.h"
#include "mog/base/Rectangle.h"

using namespace mog;

class LoadingScene : public Scene {
public:
};


class NoUpdateGroup : public Group {
public:
    static shared_ptr<NoUpdateGroup> create() {
        auto g = shared_ptr<NoUpdateGroup>(new NoUpdateGroup());
        g->setTouchEnable(false);
        return g;
    }
    
    virtual void updateFrame(const shared_ptr<Engine> &engine, float delta) override {
        this->extractEvent(engine, delta);
    }
};

void AppBase::setEngine(const shared_ptr<Engine> &engine) {
    this->engine = engine;
}

AppBase::AppBase() {
    this->pubsub = make_shared<PubSub>();
}

void AppBase::loadScene(const shared_ptr<Scene> &scene) {
    this->reserveLoadScene(scene, Transition::None, 0, Easing::Linear, LoadMode::Load);
}

void AppBase::loadScene(const shared_ptr<Scene> &scene, Transition transition, float duration, Easing easing) {
    this->reserveLoadScene(scene, transition, duration, easing, LoadMode::Load);
}

void AppBase::pushScene(const shared_ptr<Scene> &scene) {
    if (this->currentScene) {
        this->sceneStack.emplace_back(this->currentScene);
    }
    this->reserveLoadScene(scene, Transition::None, 0, Easing::Linear, LoadMode::Push);
}

void AppBase::pushScene(const shared_ptr<Scene> &scene, Transition transition, float duration, Easing easing) {
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

void AppBase::reserveLoadScene(const shared_ptr<Scene> &scene, Transition transition, float duration,
                               Easing easing, LoadMode loadMode) {
    scene->getRootGroup()->setReRenderFlagToChild(RERENDER_ALL);
    this->loadSceneParams.setParams(scene, transition, duration, easing, loadMode);
    this->isReservedLoadScene = true;
}

bool AppBase::doLoadScene() {
    if (this->isReservedLoadScene) {
        this->loadSceneMain(this->loadSceneParams);
        this->isReservedLoadScene = false;
        this->loadSceneParams.clear();
        return true;
    }
    return false;
}

void AppBase::loadSceneMain(const LoadSceneParams &params) {
    this->loadSceneMain(params.scene, params.transition, params.duration, params.easing, params.loadMode);
}

void AppBase::loadSceneMain(const shared_ptr<Scene> &scene, LoadMode loadMode) {
    if (this->currentScene) {
        this->currentScene->onDisable();
        if (loadMode != LoadMode::Push) {
            this->currentScene->onDispose();
        }
    }
    this->currentScene = scene;
    auto self = static_pointer_cast<AppBase>(shared_from_this());
    this->currentScene->setApp(self);
    
    if (loadMode != LoadMode::Pop) {
        this->currentScene->onLoad();
    }
    this->currentScene->onEnable();
}

void AppBase::loadSceneMain(const shared_ptr<Scene> &scene, Transition transition, float duration, Easing easing, LoadMode loadMode) {
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

void AppBase::loadSceneWithTransition(const shared_ptr<Scene> &scene, float duration, Easing easing, LoadMode loadMode, float loadSceneValue,
                                      function<void(shared_ptr<Entity> current, shared_ptr<Entity> next, float value)> onModify) {
    auto engine = this->engine.lock();
    bool touchEnable = engine->isTouchEnable();
    engine->setTouchEnable(false);
    
    auto loadingScene = make_shared<LoadingScene>();
    
    auto current = NoUpdateGroup::create();
    current->add(this->currentScene->getRootGroup());
    loadingScene->getRootGroup()->add(current);
    
    auto next = NoUpdateGroup::create();
    next->add(scene->getRootGroup());
    loadingScene->getRootGroup()->add(next);

    this->currentScene = loadingScene;
    
    auto tween = TweenValue::create(0, 1.0f, duration, easing);
    tween->setOnModifyEvent([this, scene, loadSceneValue, current, loadMode, next, onModify](float v, const shared_ptr<mog::Entity> &e) {
        if (e->getTag() != "loaded" && v >= loadSceneValue) {
            this->currentScene->onDisable();
            if (this->currentScene && loadMode != LoadMode::Push) {
                this->currentScene->onDispose();
            }
            scene->setApp(static_pointer_cast<AppBase>(shared_from_this()));
            if (loadMode != LoadMode::Pop) {
                scene->onLoad();
            }
            scene->onEnable();
            e->setTag("loaded");
        }
        onModify(current, next, v);
    });
    tween->setOnFinishEvent([loadingScene, this, scene, touchEnable](const shared_ptr<mog::Entity> &e) {
        loadingScene->removeAll();
        this->currentScene = scene;
        this->engine.lock()->setTouchEnable(touchEnable);
    });
    loadingScene->runTween(tween);
}

void AppBase::loadSceneWithFade(const shared_ptr<Scene> &scene, float duration, Easing easing, LoadMode loadMode) {
    auto f = [](shared_ptr<mog::Entity> current, shared_ptr<mog::Entity> next, float value) {
        if (value < 0.5f) {
            float v = 1.0 - value / 0.5f;
            current->setColorA(v);
        } else {
            current->hide();
            float v = (value - 0.5f) / 0.5f;
            next->setColorA(v);
        }
    };
    this->loadSceneWithTransition(scene, duration, easing, loadMode, 0.5f, f);
}

/*
void AppBase::loadSceneWithCrossFade(const shared_ptr<Scene> &scene, float duration, Easing easing, LoadMode loadMode) {
}
*/

void AppBase::loadSceneWithMoveIn(const shared_ptr<Scene> &scene, Transition transition, float duration, Easing easing, LoadMode loadMode) {
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
    
    auto f = [start](shared_ptr<mog::Entity> current, shared_ptr<mog::Entity> next, float value) {
        next->setZIndex(1);
        float v = 1.0f - value;
        next->setPosition(Point(start.x * v, start.y * v));
    };
    this->loadSceneWithTransition(scene, duration, easing, loadMode, 0, f);
}

void AppBase::loadSceneWithMoveOut(const shared_ptr<Scene> &scene, Transition transition, float duration, Easing easing, LoadMode loadMode) {
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
    
    auto f = [end](shared_ptr<mog::Entity> current, shared_ptr<mog::Entity> next, float value) {
        next->setZIndex(1);
        next->setPosition(Point(end.x * value, end.y * value));
    };
    this->loadSceneWithTransition(scene, duration, easing, loadMode, 0, f);
}

void AppBase::loadSceneWithSlideIn(const shared_ptr<Scene> &scene, Transition transition, float duration, Easing easing, LoadMode loadMode) {
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
    
    auto f = [currentEnd, nextStart](shared_ptr<mog::Entity> current, shared_ptr<mog::Entity> next, float value) {
        float v = 1.0f - value;
        current->setPosition(Point(currentEnd.x * v, currentEnd.y * v));
        next->setPosition(Point(nextStart.x * value, nextStart.y * value));
    };
    this->loadSceneWithTransition(scene, duration, easing, loadMode, 0, f);
}

void AppBase::drawFrame(float delta) {
    if (this->currentScene) {
        auto engine = this->engine.lock();
        this->currentScene->getRootGroup()->updateFrame(engine, delta);
        this->currentScene->getRootGroup()->drawFrame(delta);
        
        if (this->doLoadScene()) {
            return;
        }
        
        this->currentScene->onUpdate(delta);
        
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

shared_ptr<Scene> AppBase::getCurrentScene() {
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

shared_ptr<PubSub> AppBase::getPubSub() {
    return this->pubsub;
}

unsigned int AppBase::getSceneStackSize() {
    return (unsigned int)this->sceneStack.size();
}
