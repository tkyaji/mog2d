#include "mog/base/DrawableGroup.h"
#include <algorithm>

using namespace mog;

void DrawableGroup::addChild(const std::shared_ptr<Drawable> &drawable) {
    if (this->drawableIdSet.count((uintptr_t)drawable.get()) > 0) return;
    this->childDrawables.emplace_back(drawable);
    this->drawableIdSet.insert((uintptr_t)drawable.get());
    this->sortOrderDirty = true;
    drawable->drawableGroup = shared_from_this();
    if (this->addChildListener) {
        this->addChildListener(drawable);
    }
}

void DrawableGroup::removeChild(const std::shared_ptr<Drawable> &drawable) {
    this->childDrawables.erase(std::remove(this->childDrawables.begin(), this->childDrawables.end(), drawable), this->childDrawables.end());
    this->drawableIdSet.erase((uintptr_t)drawable.get());
    this->sortOrderDirty = true;
    drawable->drawableGroup.reset();
    if (this->removeChildListener) {
        this->removeChildListener(drawable);
    }
}

void DrawableGroup::removeAllChildren() {
    this->childDrawables.clear();
    this->sortedChildDrawables.clear();
    this->drawableIdSet.clear();
    this->sortOrderDirty = true;
    for (auto d : this->childDrawables) {
        d->drawableGroup.reset();
        if (this->removeChildListener) {
            this->removeChildListener(d);
        }
    }
}

void DrawableGroup::sortChildDrawablesToDraw() {
    if (!this->sortOrderDirty) return;
    
    this->sortedChildDrawables.clear();
    this->sortedChildDrawables.reserve(this->childDrawables.size());
    unordered_map<uintptr_t, unsigned int> drawableIndexes;
    drawableIndexes.reserve(this->childDrawables.size());
    for (int i = 0; i < this->childDrawables.size(); i++) {
        auto drawable = this->childDrawables[i];
        this->sortedChildDrawables.emplace_back(drawable);
        drawableIndexes[(uintptr_t)drawable.get()] = i;
    }
    sort(this->sortedChildDrawables.begin(), this->sortedChildDrawables.end(),
         [&drawableIndexes](const shared_ptr<Drawable> &d1, const shared_ptr<Drawable> &d2) {
             if (d1->getZIndex() == d2->getZIndex()) {
                 return drawableIndexes[(uintptr_t)d1.get()] < drawableIndexes[(uintptr_t)d2.get()];
             }
             return d1->getZIndex() < d2->getZIndex();
         });
    
    this->sortOrderDirty = false;
}
