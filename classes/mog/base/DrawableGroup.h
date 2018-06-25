#ifndef DrawableGroup_h
#define DrawableGroup_h

#include <vector>
#include <set>
#include <memory>
#include <functional>
#include "mog/base/Drawable.h"

namespace mog {
    class Drawable;
    
    class DrawableGroup : public enable_shared_from_this<DrawableGroup> {
        friend class Drawable;
        
    public:
        void addChild(const std::shared_ptr<Drawable> &drawable);
        void removeChild(const std::shared_ptr<Drawable> &drawable);
        void removeAllChildren();
        void sortChildDrawablesToDraw();
        
        std::function<void(const std::shared_ptr<Drawable> &drawable)> addChildListener;
        std::function<void(const std::shared_ptr<Drawable> &drawable)> removeChildListener;
        std::vector<std::shared_ptr<Drawable>> childDrawables;
        std::vector<std::shared_ptr<Drawable>> sortedChildDrawables;
        std::set<uintptr_t> drawableIdSet;
        bool sortOrderDirty = true;
    };
}

#endif /* DrawableGroup_h */
