#ifndef TouchEventListener_h
#define TouchEventListener_h

#include <memory>
#include <set>
#include <functional>
#include "mog/core/plain_objects.h"
#include "mog/core/Touch.h"
#include "mog/base/Entity.h"

namespace mog {
    class TouchEventListener : public std::enable_shared_from_this<TouchEventListener> {
    public:
        ~TouchEventListener();
        
        static std::shared_ptr<TouchEventListener> create();
        
        void touchBegin(const Touch &touch, const std::shared_ptr<Entity> &entity);
        void touchMove(const Touch &touch, const std::shared_ptr<Entity> &entity);
        void touchEnd(const Touch &touch, const std::shared_ptr<Entity> &entity);

        void setOnTapEvent(std::function<void(const Touch &t, const std::shared_ptr<Entity> &e)> onTapEvent);
        void setOnTouchBeginEvent(std::function<bool(const Touch &t, const std::shared_ptr<Entity> &e)> onTouchBeginEvent);
        void setOnTouchMoveEvent(std::function<void(const Touch &t, const std::shared_ptr<Entity> &e)> onTouchMoveEvent);
        void setOnTouchEndEvent(std::function<void(const Touch &t, const std::shared_ptr<Entity> &e)> onTouchEndEvent);
        void setOnTouchEnterEvent(std::function<void(const Touch &t, const std::shared_ptr<Entity> &e)> onTouchEnterEvent);
        void setOnTouchOverEvent(std::function<void(const Touch &t, const std::shared_ptr<Entity> &e)> onTouchOverEvent);
        void setOnTouchExitEvent(std::function<void(const Touch &t, const std::shared_ptr<Entity> &e)> onTouchExitEvent);

        void setEnable(bool enabled);
        
    protected:
        TouchEventListener();
        bool enabled = true;
        std::set<unsigned long long> touchBeganIds;
        std::set<unsigned long long> touchEnteredIds;

        bool isTouchBegan(unsigned long long touchId);
        bool isTouchEntered(unsigned long long touchId);

        std::function<void(const Touch &t, const std::shared_ptr<Entity> &e)> onTapEvent;
        std::function<bool(const Touch &t, const std::shared_ptr<Entity> &e)> onTouchBeginEvent;
        std::function<void(const Touch &t, const std::shared_ptr<Entity> &e)> onTouchMoveEvent;
        std::function<void(const Touch &t, const std::shared_ptr<Entity> &e)> onTouchEndEvent;
        std::function<void(const Touch &t, const std::shared_ptr<Entity> &e)> onTouchEnterEvent;
        std::function<void(const Touch &t, const std::shared_ptr<Entity> &e)> onTouchOverEvent;
        std::function<void(const Touch &t, const std::shared_ptr<Entity> &e)> onTouchExitEvent;
    };
}

#endif /* TouchEventListener_h */
