#ifndef Scene_h
#define Scene_h

#include <memory>
#include <vector>
#include "mog/base/Entity.h"
#include "mog/base/Group.h"
#include "mog/core/PubSub.h"

namespace mog {
    
    class AppBase;
    
    class Scene : public enable_shared_from_this<Scene> {
    public:
        void add(const shared_ptr<Entity> &entity);
        void remove(const shared_ptr<Entity> &entity);
        void removeAll();

        virtual void onLoad() {};
        virtual void onDispose() {};
        virtual void onEnable() {};
        virtual void onDisable() {};
        virtual void onUpdate(float delta) {};
        
        shared_ptr<AppBase> getApp();
        void setApp(const shared_ptr<AppBase> &app);
        shared_ptr<Group> getRootGroup();
        void setRootGroup(const shared_ptr<Group> &rootGroup);
        shared_ptr<PubSub> getPubSub();
        void runTween(const shared_ptr<Tween> &tween);
        
    protected:
        Scene();
        
        shared_ptr<PubSub> pubsub;
        weak_ptr<AppBase> app;
        shared_ptr<Group> rootGroup;
    };
}


#endif /* Scene_h */
