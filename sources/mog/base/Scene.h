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
    public:
        virtual void updateFrame(const std::shared_ptr<Engine> &engine, float delta, unsigned char parentDirtyFlag);
        virtual void drawFrame(float delta, const std::map<unsigned int, TouchInput> &touches);
        void add(const std::shared_ptr<Drawable> &drawable);
        void insertBefore(const std::shared_ptr<Drawable> &drawable, const std::shared_ptr<Drawable> &baseDrawable);
        void insertAfter(const std::shared_ptr<Drawable> &drawable, const std::shared_ptr<Drawable> &baseDrawable);
        void remove(const std::shared_ptr<Drawable> &drawable);
        void removeAll();
        std::shared_ptr<AppBase> getApp();
        std::shared_ptr<PubSub> getPubSub();
        std::shared_ptr<DrawableGroup> getRootDrawableGroup();

    protected:
        Scene();
        void setApp(const std::shared_ptr<AppBase> &app);

        virtual void onLoad() {};
        virtual void onDispose() {};
        virtual void onEnable() {};
        virtual void onDisable() {};
        virtual void onUpdate(float delta) {};
        
        std::shared_ptr<DrawableGroup> rootGroup;
        std::shared_ptr<PubSub> pubsub;
        std::weak_ptr<AppBase> app;
        float matrix[20] = {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1,
            1, 1, 1, 1,
        };
        unsigned char dirtyFlag = DIRTY_ALL;
        bool sortOrderDirty = false;
    };
}

#endif /* Scene_h */
