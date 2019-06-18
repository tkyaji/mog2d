#ifndef Scene_h
#define Scene_h

#include <memory>
#include <vector>
#include "mog/base/Drawable.h"
#include "mog/base/DrawableGroup.h"
#include "mog/core/PubSub.h"

namespace mog {
    class AppBase;
    class Engine;
    
    class Scene : public std::enable_shared_from_this<Scene> {
        friend class AppBase;
        friend class Drawable;
    public:
        virtual void updateFrame(const std::shared_ptr<Engine> &engine, float delta, unsigned char parentDirtyFlag = 0);
        virtual void drawFrame(float delta);
        void add(const std::shared_ptr<Drawable> &drawable);
        void remove(const std::shared_ptr<Drawable> &drawable);
        void removeAll();
        std::shared_ptr<AppBase> getApp();
        std::shared_ptr<PubSub> getPubSub();
        std::vector<std::shared_ptr<Drawable>> getChildDrawables();

    protected:
        Scene();
        void setApp(const std::shared_ptr<AppBase> &app);

        virtual void onLoad() {};
        virtual void onDispose() {};
        virtual void onEnable() {};
        virtual void onDisable() {};
        virtual void onUpdate(float delta) {};
        
        std::shared_ptr<DrawableGroup> drawableGroup;
        std::shared_ptr<PubSub> pubsub;
        std::weak_ptr<AppBase> app;
        float matrix[20] = {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1,
            1, 1, 1, 1,
        };
        unsigned char dirtyFlag = 0;
        bool sortOrderDirty = false;
    };
}

#endif /* Scene_h */
