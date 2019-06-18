#ifndef AppBase_h
#define AppBase_h

#include <memory>
#include "mog/core/plain_objects.h"
#include "mog/core/PubSub.h"
#include "mog/core/Tween.h"
#include "mog/core/KeyEvent.h"

namespace mog {
    class Engine;
    class Scene;
    class Entity;
    
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
    
    
    class AppBase : public std::enable_shared_from_this<AppBase> {
        friend class Engine;
    public:
        AppBase();
        
        void loadScene(const std::shared_ptr<Scene> &scene);
        void loadScene(const std::shared_ptr<Scene> &scene, Transition transition, float duration, Easing easing = Easing::Linear);
        void pushScene(const std::shared_ptr<Scene> &scene);
        void pushScene(const std::shared_ptr<Scene> &scene, Transition transition, float duration, Easing easing = Easing::Linear);
        void popScene();
        void popScene(Transition transition, float duration, Easing easing = Easing::Linear);
        
        Color getBackgroundColor();
        void setBackgroundColor(const Color &color);
        std::shared_ptr<Scene> getCurrentScene();
        
        void setTouchEnable(bool enable);
        void setMultiTouchEnable(bool enable);
        bool isTouchEnable();
        bool isMultiTouchEnable();
        
        void setStatsViewEnable(bool enable);
        void setStatsViewAlignment(Alignment alignment);

        std::shared_ptr<PubSub> getPubSub();
        unsigned int getSceneStackSize();

        virtual void drawFrame(float delta, unsigned char parentDirtyFlag = 0);
        
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

        std::weak_ptr<Engine> engine;
        std::shared_ptr<PubSub> pubsub;
        std::vector<std::shared_ptr<Scene>> sceneStack;
        std::shared_ptr<Scene> currentScene;

        void setEngine(const std::shared_ptr<Engine> &engine);

    private:
        struct LoadSceneParams {
            std::shared_ptr<Scene> scene = nullptr;
            Transition transition = Transition::None;
            float duration = 0;
            Easing easing = Easing::Linear;
            LoadMode loadMode = LoadMode::Load;
            
            void setParams(const std::shared_ptr<Scene> &scene, Transition transition,
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
        
        
        class SceneTransition {
        public:
            enum class SceneOrder {
                CurrentNext,
                NextCurrent,
            };
            static std::unique_ptr<SceneTransition> create(const std::shared_ptr<Engine> &engine, const std::shared_ptr<AppBase> &app,
                                                           const std::shared_ptr<Scene> &currentScene, const std::shared_ptr<Scene> &nextScene,
                                                           float duration, Easing easing, LoadMode loadMode, float loadSceneValue, SceneOrder sceneOrder,
                                                           std::function<void(std::shared_ptr<Scene> current, std::shared_ptr<Scene> next, float value)> onModify);
            void init();
            bool update(float delta);
            
            std::weak_ptr<Engine> engine;
            std::weak_ptr<AppBase> app;
            std::shared_ptr<Scene> currentScene;
            std::shared_ptr<Scene> nextScene;
            float duration;
            std::shared_ptr<EasingFunc> easingFunc;
            LoadMode loadMode;
            SceneOrder sceneOrder;
            float loadSceneValue;
            float elapsedTime;
            bool loaded;
            bool touchEnable;
            std::function<void(std::shared_ptr<Scene> current, std::shared_ptr<Scene> next, float value)> onModify;
        };

        
        bool isReservedLoadScene = false;
        LoadSceneParams loadSceneParams;
        std::shared_ptr<SceneTransition> sceneTransition = nullptr;
        
        void reserveLoadScene(const std::shared_ptr<Scene> &scene, Transition transition, float duration, Easing easing, LoadMode loadMode);
        bool doLoadScene();
        
        void loadSceneMain(const LoadSceneParams &params);
        void loadSceneMain(const std::shared_ptr<Scene> &scene, LoadMode loadMode = LoadMode::Load);
        void loadSceneMain(const std::shared_ptr<Scene> &scene, Transition transition, float duration, Easing easing = Easing::Linear, LoadMode loadMode = LoadMode::Load);
        void loadSceneWithFade(const std::shared_ptr<Scene> &scene, float duration, Easing easing, LoadMode loadMode);
        void loadSceneWithMoveIn(const std::shared_ptr<Scene> &scene, Transition transition, float duration, Easing easing, LoadMode loadMode);
        void loadSceneWithMoveOut(const std::shared_ptr<Scene> &scene, Transition transition, float duration, Easing easing, LoadMode loadMode);
        void loadSceneWithSlideIn(const std::shared_ptr<Scene> &scene, Transition transition, float duration, Easing easing, LoadMode loadMode);
    };
}

#endif /* AppBase_h */
