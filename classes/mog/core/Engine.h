#ifndef Engine_h
#define Engine_h

#include <memory>
#include <unordered_map>
#include "mog/Constants.h"
#include "mog/core/TouchInput.h"
#include "mog/core/KeyEvent.h"
#include "mog/core/NativeClass.h"
#include "mog/core/MogStats.h"
#include "mog/base/AppBase.h"

using namespace std;

extern void *enabler;

namespace mog {
        
    class Engine : public enable_shared_from_this<Engine> {
    public:
        static shared_ptr<Engine> create(const shared_ptr<AppBase> &app);
        
        ~Engine();

        void startEngine();
        void stopEngine();
        
        void onDrawFrame(map<unsigned int, TouchInput> touches);
        void onLowMemory();
        void onKeyEvent(const KeyEvent &keyEvent);

        shared_ptr<AppBase> getApp();
        
        shared_ptr<NativeObject> getNativeObject(string name);
        void setNativeObject(string name, const shared_ptr<NativeObject> &nObj);
        void removeNativeObject(string name);
        
        bool isRunning();
        int getCurrentOrientation();
        unsigned long long getFrameCount();
        
        Size getDisplaySize();
        Size getScreenSize();
        float getScreenScale();
        void setDisplaySize(const Size &size);
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
        
        shared_ptr<AppBase> app;
        unordered_map<string, shared_ptr<NativeObject>> nativeObjects;
        shared_ptr<Renderer> renderer;
        shared_ptr<MogStats> stats;
        bool running = false;
        unsigned long long frameCount = 0;
        Size displaySize = Size::zero;
        Size screenSize = Size::zero;
        Color color = Color::black;

        Engine();

        int frameCountForFps = 0;
        bool timerRunning = false;
        long long timerStartTime = 0;
        long long timerBackupTime = 0;
        float lastElapsedSec = 0;
        
        unordered_map<unsigned int, function<void(unsigned int funcId)>> onUpdateFuncs;
        unordered_map<unsigned int, function<void(unsigned int funcId)>> onUpdateFuncsToAdd;
        vector<unsigned int> onUpdateFuncIdsToRemove;
        
        void invokeOnUpdateFunc();
        
    private:
        bool initialized = false;
        bool displaySizeChanged = false;
        bool touchEnable = true;
        bool multiTouchEnable = true;
        vector<shared_ptr<Entity>> touchableEntities;
        unordered_map<int, Touch> prevTouches;

        void initParameters();
        void setViewPortScale();
        void initScreen();
        
        void fireTouchListeners(map<unsigned int, TouchInput> touches);
    };
}

#endif /* Engine_h */
