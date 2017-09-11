#include "mog/core/TouchEventManager.h"
#include "mog/core/Engine.h"
#include <algorithm>

using namespace mog;

//unsigned long TouchEventManager::eventIdCounter = 0;

TouchEventManager *TouchEventManager::instance;

TouchEventManager * TouchEventManager::initialize() {
    if (TouchEventManager::instance) {
        return TouchEventManager::instance;
    }
    TouchEventManager::instance = new TouchEventManager();
    return TouchEventManager::instance;
}

TouchEventManager *TouchEventManager::getInstance() {
    return TouchEventManager::instance;
}

TouchEventManager::TouchEventManager() {
    this->touchEnable = true;
    this->multiTouchEnable = true;
    this->touchableEntities.reserve(256);
}

void TouchEventManager::setTouchEnable(bool enable) {
    this->touchEnable = enable;
}

void TouchEventManager::setMultiTouchEnable(bool enable) {
    this->multiTouchEnable = enable;
}

bool TouchEventManager::isTouchEnable() {
    return this->touchEnable;
}

bool TouchEventManager::isMultiTouchEnable() {
    return this->multiTouchEnable;
}

void TouchEventManager::pushEntity(const shared_ptr<Entity> &entity) {
    this->touchableEntities.emplace_back(entity);
}

void TouchEventManager::fireTouchListeners(map<unsigned int, TouchInput> touches) {
    auto engine = Engine::getInstance();
    float scale = engine->getApp()->getScreenScale();
    float density = engine->getDeviceDensity();
    float uptime = Engine::getInstance()->getTimerElapsedSec();

    for (auto pair : touches) {
        unsigned int touchId = pair.first;
        auto touchInput = pair.second;
        auto vp = Point(touchInput.x, touchInput.y);
        auto p = vp * density / scale;
        auto touch = Touch(touchId, p, vp, uptime);

        if (this->prevTouches.count(touchId) == 0) {
            touch.startTime = uptime;
            touch.startPosition = p;
            touch.startViewPosition = vp;
        } else {
            auto prevTouch = this->prevTouches[touchId];
            touch.startTime = prevTouch.startTime;
            touch.deltaTime = uptime - prevTouch.uptime;
            touch.startPosition = prevTouch.startPosition;
            touch.startViewPosition = prevTouch.startViewPosition;
            touch.deltaPosition = p - prevTouch.position;
            touch.deltaViewPosition = vp - prevTouch.viewPosition;
        }
        
        if (this->touchEnable) {
            bool isSwallowTouches = true;
            
            for (int i = (int)this->touchableEntities.size() - 1; i >= 0; i--) {
                auto entity = this->touchableEntities[i];
                
                if (touchInput.action == TouchAction::TouchDown || touchInput.action == TouchAction::TouchDownUp) {
                    if (isSwallowTouches && entity->contains(p)) {
                        entity->fireTouchBeginEvent(touch);

                        if (!entity->isSwallowTouches()) {
                            isSwallowTouches = false;
                        }
                    }
                }
                if (touchInput.action == TouchAction::TouchMove) {
                    entity->fireTouchMoveEvent(touch);
                }
                if (touchInput.action == TouchAction::TouchUp || touchInput.action == TouchAction::TouchDownUp) {
                    entity->fireTouchEndEvent(touch);
                }
            }
        }
        
        if (touchInput.action == TouchAction::TouchUp || touchInput.action == TouchAction::TouchDownUp) {
            this->prevTouches.erase(touchId);
        } else {
            this->prevTouches[touchId] = touch;
        }

        if (!this->multiTouchEnable) {
            break;
        }
    }
    
    this->touchableEntities.clear();
}

