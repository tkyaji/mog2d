#include "mog/core/TouchEventListener.h"
#include "mog/core/Engine.h"
#include "mog/base/Entity.h"

using namespace mog;

shared_ptr<TouchEventListener> TouchEventListener::create() {
    return shared_ptr<TouchEventListener>(new TouchEventListener());
}

TouchEventListener::TouchEventListener() {
    this->touchBegan = false;
    this->enabled = true;
    this->eventId = 0;
    this->touchId = -1;
}

TouchEventListener::~TouchEventListener() {
}

void TouchEventListener::touchBegin(const Touch &touch, const shared_ptr<Entity> &entity) {
    if (!this->enabled) return;
    
    if (this->onTouchBeginEvent) {
        if (!this->onTouchBeginEvent(touch, entity)) return;
    }
    
    this->touchBegan = true;
    this->touchId = touch.touchId;
}

void TouchEventListener::touchMove(const Touch &touch, const shared_ptr<Entity> &entity) {
    if (!this->enabled || !this->touchBegan || this->touchId != touch.touchId) return;

    if (this->onTouchMoveEvent) {
        this->onTouchMoveEvent(touch, entity);
    }
}

void TouchEventListener::touchEnd(const Touch &touch, const shared_ptr<Entity> &entity) {
    if (!this->enabled || !this->touchBegan || this->touchId != touch.touchId) return;
    
    if (this->onTouchEndEvent) {
        this->onTouchEndEvent(touch, entity);
    }
    if (entity->contains(touch.position)) {
        if (this->onTapEvent) {
            this->onTapEvent(touch, entity);
        }
    }
    this->touchBegan = false;
    this->touchId = -1;
}

void TouchEventListener::setOnTapEvent(function<void(const Touch &t, const shared_ptr<Entity> &e)> onTapEvent) {
    this->onTapEvent = onTapEvent;
}

void TouchEventListener::setOnTouchBeginEvent(function<bool(const Touch &t, const shared_ptr<Entity> &e)> onTouchBeginEvent) {
    this->onTouchBeginEvent = onTouchBeginEvent;
}

void TouchEventListener::setOnTouchMoveEvent(function<void(const Touch &t, const shared_ptr<Entity> &e)> onTouchMoveEvent) {
    this->onTouchMoveEvent = onTouchMoveEvent;
}

void TouchEventListener::setOnTouchEndEvent(function<void(const Touch &t, const shared_ptr<Entity> &e)> onTouchEndEvent) {
    this->onTouchEndEvent = onTouchEndEvent;
}

bool TouchEventListener::isTouchBegan() {
    return this->touchBegan;
}

void TouchEventListener::setEnable(bool enabled) {
    this->enabled = enabled;
}
