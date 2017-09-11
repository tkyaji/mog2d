#ifndef TouchEventManager_h
#define TouchEventManager_h

#include <memory>
#include <vector>
#include <map>
#include <unordered_map>
#include "mog/core/TouchEventListener.h"
#include "mog/core/TouchInput.h"

using namespace std;

namespace mog {
    class TouchEventManager {
    public:
        static TouchEventManager * initialize();
        static TouchEventManager *getInstance();
        
        void setTouchEnable(bool enable);
        void setMultiTouchEnable(bool enable);
        bool isTouchEnable();
        bool isMultiTouchEnable();
        
        void pushEntity(const shared_ptr<Entity> &entity);
        void fireTouchListeners(map<unsigned int, TouchInput> touches);
        void cancelTouchListener(const shared_ptr<TouchEventListener> &listener, bool callTouchEnd);

    private:
        vector<shared_ptr<Entity>> touchableEntities;
        
        bool touchEnable;
        bool multiTouchEnable;
        unordered_map<int, Touch> prevTouches;
        TouchEventManager();
        static TouchEventManager *instance;
    };
}


#endif /* TouchEventManager_h */
