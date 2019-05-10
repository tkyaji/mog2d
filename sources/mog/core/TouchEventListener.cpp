#include "mog/core/TouchEventListener.h"
#include "mog/core/Engine.h"

using namespace mog;

#define ON_TAP_THRESHOLD 8.0f

std::shared_ptr<TouchEventListener> TouchEventListener::create() {
    return std::shared_ptr<TouchEventListener>(new TouchEventListener());
}

TouchEventListener::TouchEventListener() {
}

TouchEventListener::~TouchEventListener() {
}

void TouchEventListener::touchBegin(const Touch &touch, const std::shared_ptr<Entity> &entity) {
    if (!this->enabled) return;
    
    unsigned long long tid = ((unsigned long long)entity.get() << 32) + touch.touchId;
    
    if (this->onTouchEnterEvent) {
        this->onTouchEnterEvent(touch, entity);
    }
    this->touchEnteredIds.insert(tid);
    
    if (this->onTouchOverEvent) {
        this->onTouchOverEvent(touch, entity);
    }
    
    if (this->onTouchBeginEvent) {
        if (!this->onTouchBeginEvent(touch, entity)) return;
    }
    this->touchBeganIds.insert(tid);
}

void TouchEventListener::touchMove(const Touch &touch, const std::shared_ptr<Entity> &entity) {
    if (!this->enabled) return;
    
    unsigned long long tid = ((unsigned long long)entity.get() << 32) + touch.touchId;
    
    if (this->isTouchBegan(tid)) {
        if (this->onTouchMoveEvent) {
            this->onTouchMoveEvent(touch, entity);
        }
    }
    
    if (entity->contains(touch.position)) {
        if (!this->isTouchEntered(tid)) {
            if (this->onTouchEnterEvent) {
                this->onTouchEnterEvent(touch, entity);
            }
            this->touchEnteredIds.insert(tid);
        }
        if (this->onTouchOverEvent) {
            this->onTouchOverEvent(touch, entity);
        }
        
    } else {
        if (this->isTouchEntered(tid)) {
            if (this->onTouchExitEvent) {
                this->onTouchExitEvent(touch, entity);
            }
            this->touchEnteredIds.erase(tid);
        }
    }
}

void TouchEventListener::touchEnd(const Touch &touch, const std::shared_ptr<Entity> &entity) {
    if (!this->enabled) return;
    
    unsigned long long tid = ((unsigned long long)entity.get() << 32) + touch.touchId;
    
    if (this->isTouchBegan(tid)) {
        if (this->onTouchEndEvent) {
            this->onTouchEndEvent(touch, entity);
        }
        
        if (abs(touch.viewPosition.x - touch.startViewPosition.x) < ON_TAP_THRESHOLD &&
            abs(touch.viewPosition.y - touch.startViewPosition.y) < ON_TAP_THRESHOLD) {
            if (entity->contains(touch.position)) {
                if (this->onTapEvent) {
                    this->onTapEvent(touch, entity);
                }
            }
        }
        
        this->touchBeganIds.erase(tid);
    }
    
    if (this->isTouchEntered(tid)) {
        if (this->onTouchExitEvent) {
            this->onTouchExitEvent(touch, entity);
        }
        this->touchEnteredIds.erase(tid);
    }
}

void TouchEventListener::setOnTapEvent(std::function<void(const Touch &t, const std::shared_ptr<Entity> &e)> onTapEvent) {
    this->onTapEvent = onTapEvent;
}

void TouchEventListener::setOnTouchBeginEvent(std::function<bool(const Touch &t, const std::shared_ptr<Entity> &e)> onTouchBeginEvent) {
    this->onTouchBeginEvent = onTouchBeginEvent;
}

void TouchEventListener::setOnTouchMoveEvent(std::function<void(const Touch &t, const std::shared_ptr<Entity> &e)> onTouchMoveEvent) {
    this->onTouchMoveEvent = onTouchMoveEvent;
}

void TouchEventListener::setOnTouchEndEvent(std::function<void(const Touch &t, const std::shared_ptr<Entity> &e)> onTouchEndEvent) {
    this->onTouchEndEvent = onTouchEndEvent;
}

void TouchEventListener::setOnTouchEnterEvent(std::function<void(const Touch &t, const std::shared_ptr<Entity> &e)> onTouchEnterEvent) {
    this->onTouchEnterEvent = onTouchEnterEvent;
}

void TouchEventListener::setOnTouchOverEvent(std::function<void(const Touch &t, const std::shared_ptr<Entity> &e)> onTouchOverEvent) {
    this->onTouchOverEvent = onTouchOverEvent;
}

void TouchEventListener::setOnTouchExitEvent(std::function<void(const Touch &t, const std::shared_ptr<Entity> &e)> onTouchExitEvent) {
    this->onTouchExitEvent = onTouchExitEvent;
}

void TouchEventListener::setEnable(bool enabled) {
    this->enabled = enabled;
}

bool TouchEventListener::isTouchBegan(unsigned long long touchId) {
    return this->touchBeganIds.count(touchId) > 0;
}

bool TouchEventListener::isTouchEntered(unsigned long long touchId) {
    return this->touchEnteredIds.count(touchId) > 0;
}
