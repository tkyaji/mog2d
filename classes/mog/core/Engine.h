#ifndef Engine_h
#define Engine_h

#include <memory>
#include <unordered_map>
#include "mog/Constants.h"
#include "mog/core/TouchInput.h"
#include "mog/core/Touch.h"
#include "mog/core/KeyEvent.h"
#include "mog/core/NativeClass.h"
#include "mog/core/MogStats.h"
#include "mog/base/AppBase.h"

extern void *enabler;

namespace mog {
    class Entity;
        
    class Engine : public std::enable_shared_from_this<Engine> {
    public:
        static std::shared_ptr<Engine> create(const std::shared_ptr<AppBase> &app);
        static std::shared_ptr<Engine> getInstance();

        ~Engine();

        void startEngine();
        void stopEngine();
        
        void onDrawFrame(std::map<unsigned int, TouchInput> touches);
        void onLowMemory();
        void onKeyEvent(const KeyEvent &keyEvent);

        std::shared_ptr<AppBase> getApp();
        
        std::shared_ptr<NativeObject> getNativeObject(string name);
        void setNativeObject(string name, const std::shared_ptr<NativeObject> &nObj);
        void removeNativeObject(string name);
        
        bool isRunning();
        int getCurrentOrientation();
        unsigned long long getFrameCount();
        
        Size getDisplaySize();
        Size getScreenSize();
        float getScreenScale();
        void setDisplaySize(const Size &displaySize, const Size &viewSize);
        void setScreenSizeBasedOnHeight(float height);
        void setScreenSizeBasedOnWidth(float width);
        
        Color getClearColor();
        void setClearColor(const Color &color);
        void clearColor();

        void startTimer();
        void stopTimer();
        long long getTimerElapsed();
        float getTimerElapsedSec();
        
        void setStatsEnable(bool enable);
        void setStatsAlignment(Alignment alignment);
        shared_ptr<MogStats> getStats();

        void setTouchEnable(bool enable);
        void setMultiTouchEnable(bool enable);
        bool isTouchEnable();
        bool isMultiTouchEnable();
        void pushTouchableEntity(const shared_ptr<Entity> &entity);

        unsigned int registerOnUpdateFunc(function<void(unsigned int funcId)> onUpdateFunc);
        void removeOnUpdateFunc(unsigned int funcId);

    protected:
        unsigned int onUpdateFuncIdCounter;
        
        std::shared_ptr<AppBase> app;
        unordered_map<string, std::shared_ptr<NativeObject>> nativeObjects;
        std::shared_ptr<Renderer> renderer;
        std::shared_ptr<MogStats> stats;
        bool running = false;
        unsigned long long frameCount = 0;
        Size displaySize = Size::zero;
        Size viewSize = Size::zero;
        Size screenSize = Size::zero;
        Color color = Color::black;

        Engine();

        int frameCountForFps = 0;
        bool timerRunning = false;
        long long timerStartTime = 0;
        long long timerBackupTime = 0;
        float lastElapsedSec = 0;
        
        std::unordered_map<unsigned int, std::function<void(unsigned int funcId)>> onUpdateFuncs;
        std::unordered_map<unsigned int, std::function<void(unsigned int funcId)>> onUpdateFuncsToAdd;
        std::vector<unsigned int> onUpdateFuncIdsToRemove;
        
        void invokeOnUpdateFunc();
        
    private:
        static std::weak_ptr<Engine> instance;
        
        bool initialized = false;
        bool displaySizeChanged = false;
        bool touchEnable = true;
        bool multiTouchEnable = true;
        std::vector<shared_ptr<Entity>> touchableEntities;
        std::unordered_map<int, Touch> prevTouches;

        void initParameters();
        void setViewPortScale();
        void initScreen();

        void fireTouchListeners(std::map<unsigned int, TouchInput> touches);
    };
}

#endif /* Engine_h */
