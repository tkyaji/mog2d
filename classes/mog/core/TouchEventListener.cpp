#include "mog/core/TouchEventListener.h"
#include "mog/core/Engine.h"
#include "mog/base/Entity.h"

using namespace mog;

shared_ptr<TouchEventListener> TouchEventListener::create() {
    return shared_ptr<TouchEventListener>(new TouchEventListener());
}

TouchEventListener::TouchEventListener() {
}

TouchEventListener::~TouchEventListener() {
}

void TouchEventListener::touchBegin(const Touch &touch, const shared_ptr<Entity> &entity) {
    if (!this->enabled) return;
    
    unsigned long long tid = ((uintptr_t)entity.get() << 32) + touch.touchId;
    
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

void TouchEventListener::touchMove(const Touch &touch, const shared_ptr<Entity> &entity) {
    if (!this->enabled) return;
    
    unsigned long long tid = ((uintptr_t)entity.get() << 32) + touch.touchId;
    
    if (this->isTouchBegan(tid)) {
        if (this->onTouchDragEvent) {
            this->onTouchDragEvent(touch, entity);
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

void TouchEventListener::touchEnd(const Touch &touch, const shared_ptr<Entity> &entity) {
    if (!this->enabled) return;
    
    unsigned long long tid = ((uintptr_t)entity.get() << 32) + touch.touchId;
    
    if (this->isTouchBegan(tid)) {
        if (this->onTouchEndEvent) {
            this->onTouchEndEvent(touch, entity);
        }
        if (entity->contains(touch.position)) {
            if (this->onTapEvent) {
                this->onTapEvent(touch, entity);
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

void TouchEventListener::setOnTapEvent(function<void(const Touch &t, const shared_ptr<Entity> &e)> onTapEvent) {
    this->onTapEvent = onTapEvent;
}

void TouchEventListener::setOnTouchBeginEvent(function<bool(const Touch &t, const shared_ptr<Entity> &e)> onTouchBeginEvent) {
    this->onTouchBeginEvent = onTouchBeginEvent;
}

void TouchEventListener::setOnTouchDragEvent(function<void(const Touch &t, const shared_ptr<Entity> &e)> onTouchDragEvent) {
    this->onTouchDragEvent = onTouchDragEvent;
}

void TouchEventListener::setOnTouchEndEvent(function<void(const Touch &t, const shared_ptr<Entity> &e)> onTouchEndEvent) {
    this->onTouchEndEvent = onTouchEndEvent;
}

void TouchEventListener::setOnTouchEnterEvent(function<void(const Touch &t, const shared_ptr<Entity> &e)> onTouchEnterEvent) {
    this->onTouchEnterEvent = onTouchEnterEvent;
}

void TouchEventListener::setOnTouchOverEvent(function<void(const Touch &t, const shared_ptr<Entity> &e)> onTouchOverEvent) {
    this->onTouchOverEvent = onTouchOverEvent;
}

void TouchEventListener::setOnTouchExitEvent(function<void(const Touch &t, const shared_ptr<Entity> &e)> onTouchExitEvent) {
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
