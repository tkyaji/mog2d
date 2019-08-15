#ifndef DrawableContainer_h
#define DrawableContainer_h

#include <vector>
#include <set>
#include <memory>
#include <functional>
#include "mog/base/Drawable.h"

namespace mog {
    class Drawable;
    
    class DrawableContainer : public std::enable_shared_from_this<DrawableContainer> {
        friend class Drawable;
        
    public:
        void addChild(const std::shared_ptr<Drawable> &drawable);
        void insertChildBefore(const std::shared_ptr<Drawable> &drawable, const std::shared_ptr<Drawable> &baseDrawable);
        void insertChildAfter(const std::shared_ptr<Drawable> &drawable, const std::shared_ptr<Drawable> &baseDrawable);
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

#endif /* DrawableContainer_h */
