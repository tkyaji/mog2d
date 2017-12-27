#ifndef AppBase_h
#define AppBase_h

#include <memory>
#include "mog/core/plain_objects.h"
#include "mog/core/PubSub.h"
#include "mog/core/Tween.h"
#include "mog/core/KeyEvent.h"
#include "mog/base/Scene.h"

using namespace std;

namespace mog {
    class Engine;
    
    enum class Transition {
        None,
        Fade,
//        CrossFade,
        MoveInTop,
        MoveInBottom,
        MoveInLeft,
        MoveInRight,
        MoveOutTop,
        MoveOutBottom,
        MoveOutLeft,
        MoveOutRight,
        SlideInTop,
        SlideInBottom,
        SlideInLeft,
        SlideInRight,
    };

    enum class Alignment {
        TopLeft,
        TopCenter,
        TopRight,
        MiddleLeft,
        MiddleCenter,
        MiddleRight,
        BottomLeft,
        BottomCenter,
        BottomRight,
    };
    
    
    class AppBase : public enable_shared_from_this<AppBase> {
    public:
        AppBase();
        
        void setEngine(const shared_ptr<Engine> &engine);
        
        void loadScene(const shared_ptr<Scene> &scene);
        void loadScene(const shared_ptr<Scene> &scene, Transition transition, float duration, Easing easing = Easing::Linear);
        void pushScene(const shared_ptr<Scene> &scene);
        void pushScene(const shared_ptr<Scene> &scene, Transition transition, float duration, Easing easing = Easing::Linear);
        void popScene();
        void popScene(Transition transition, float duration, Easing easing = Easing::Linear);
        
        Color getBackgroundColor();
        void setBackgroundColor(const Color &color);
        shared_ptr<Scene> getCurrentScene();
        
        void setScreenSizeBasedOnHeight(float height);
        void setScreenSizeBasedOnWidth(float width);
        Size getScreenSize();
        float getScreenScale();
        Size getDisplaySize();
        
        void setTouchEnable(bool enable);
        void setMultiTouchEnable(bool enable);
        bool isTouchEnable();
        bool isMultiTouchEnable();
        
        void setStatsViewEnable(bool enable);
        void setStatsViewAlignment(Alignment alignment);

        shared_ptr<PubSub> getPubSub();
        unsigned int getSceneStackSize();

        virtual void drawFrame(float delta);
        
        virtual void onLoad() {};
        virtual void onDispose() {};
        virtual void onUpdate(float delta) {};
        virtual void onPause() {};
        virtual void onResume() {};
        virtual void onLowMemory() {};
        virtual void onKeyEvent(const KeyEvent &keyEvent) {};
        
    protected:
        enum class LoadMode {
            Load,
            Push,
            Pop,
        };

        shared_ptr<Scene> currentScene;

        void loadSceneWithTransition(const shared_ptr<Scene> &scene, float duration, Easing easing, LoadMode loadMode, float loadSceneValue,
                                     function<void(shared_ptr<Entity> current, shared_ptr<Entity> next, float value)> onModify);
        
    private:
        struct LoadSceneParams {
            shared_ptr<Scene> scene = nullptr;
            Transition transition = Transition::None;
            float duration = 0;
            Easing easing = Easing::Linear;
            LoadMode loadMode = LoadMode::Load;
            
            void setParams(const shared_ptr<Scene> &scene, Transition transition,
                           float duration, Easing easing, LoadMode loadMode) {
                this->scene = scene;
                this->transition = transition;
                this->duration = duration;
                this->easing = easing;
                this->loadMode = loadMode;
            }
            
            void clear() {
                this->scene = nullptr;
                this->transition = Transition::None;
                this->duration = 0;
                this->easing = Easing::Linear;
                this->loadMode = LoadMode::Load;
            }
        };
        
        weak_ptr<Engine> engine;
        shared_ptr<PubSub> pubsub;
        vector<shared_ptr<Scene>> sceneStack;
        
        bool isReservedLoadScene = false;
        LoadSceneParams loadSceneParams;
        
        void reserveLoadScene(const shared_ptr<Scene> &scene, Transition transition, float duration, Easing easing, LoadMode loadMode);
        bool doLoadScene();
        
        void loadSceneMain(const LoadSceneParams &params);
        void loadSceneMain(const shared_ptr<Scene> &scene, LoadMode loadMode = LoadMode::Load);
        void loadSceneMain(const shared_ptr<Scene> &scene, Transition transition, float duration, Easing easing = Easing::Linear, LoadMode loadMode = LoadMode::Load);
        void loadSceneWithFade(const shared_ptr<Scene> &scene, float duration, Easing easing, LoadMode loadMode);
//        void loadSceneWithCrossFade(const shared_ptr<Scene> &scene, float duration, Easing easing, LoadMode loadMode);
        void loadSceneWithMoveIn(const shared_ptr<Scene> &scene, Transition transition, float duration, Easing easing, LoadMode loadMode);
        void loadSceneWithMoveOut(const shared_ptr<Scene> &scene, Transition transition, float duration, Easing easing, LoadMode loadMode);
        void loadSceneWithSlideIn(const shared_ptr<Scene> &scene, Transition transition, float duration, Easing easing, LoadMode loadMode);
    };
}

#endif /* AppBase_h */
