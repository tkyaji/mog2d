#ifndef TouchEventListener_h
#define TouchEventListener_h

#include <memory>
#include <functional>
#include "mog/core/plain_objects.h"
#include "mog/core/Touch.h"
#include "mog/base/Entity.h"

using namespace std;

namespace mog {
    class TouchEventListener : public enable_shared_from_this<TouchEventListener> {
    public:
        ~TouchEventListener();
        
        unsigned long eventId;
        static shared_ptr<TouchEventListener> create();
        
        void tap(const Touch &touch, const shared_ptr<Entity> &entity);
        void touchBegin(const Touch &touch, const shared_ptr<Entity> &entity);
        void touchMove(const Touch &touch, const shared_ptr<Entity> &entity);
        void touchEnd(const Touch &touch, const shared_ptr<Entity> &entity);

        void setOnTapEvent(function<void(const Touch &t, const shared_ptr<Entity> &e)> onTapEvent);
        void setOnTouchBeginEvent(function<bool(const Touch &t, const shared_ptr<Entity> &e)> onTouchBeginEvent);
        void setOnTouchMoveEvent(function<void(const Touch &t, const shared_ptr<Entity> &e)> onTouchMoveEvent);
        void setOnTouchEndEvent(function<void(const Touch &t, const shared_ptr<Entity> &e)> onTouchEndEvent);
        bool isTouchBegan();
        
        void setEnable(bool enabled);
        
    protected:
        TouchEventListener();
        bool touchBegan = false;
        bool enabled = true;
        int touchId = 0;

        function<void(const Touch &t, const shared_ptr<Entity> &e)> onTapEvent;
        function<bool(const Touch &t, const shared_ptr<Entity> &e)> onTouchBeginEvent;
        function<void(const Touch &t, const shared_ptr<Entity> &e)> onTouchMoveEvent;
        function<void(const Touch &t, const shared_ptr<Entity> &e)> onTouchEndEvent;
    };
}

#endif /* TouchEventListener_h */
