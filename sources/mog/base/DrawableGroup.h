#ifndef DrawableGroup_h
#define DrawableGroup_h

#define VERTICES_IDX 0
#define INDICES_IDX 1
#define VERTEX_COLORS_IDX 2
#define VERTEX_TEX_COORDS_IDX 3

#include "mog/base/Drawable.h"
#include "mog/base/DrawableContainer.h"

namespace mog {
    class DrawableGroup : public Drawable {
    public:
        static std::shared_ptr<DrawableGroup> create();
        
        void add(const std::shared_ptr<Drawable> &drawable);
        void insertBefore(const std::shared_ptr<Drawable> &drawable, const std::shared_ptr<Drawable> &baseDrawable);
        void insertAfter(const std::shared_ptr<Drawable> &drawable, const std::shared_ptr<Drawable> &baseDrawable);
        void remove(const std::shared_ptr<Drawable> &drawable);
        void removeAll();
        std::vector<std::shared_ptr<Drawable>> getChildDrawables();

        virtual void updateFrame(const std::shared_ptr<Engine> &engine, float delta, float *parentMatrix, unsigned char parentDirtyFlag) override;
        virtual void drawFrame(float delta, const std::map<unsigned int, TouchInput> &touches) override;
//        virtual void updateMatrix(float *parentMatrix, unsigned char parentDirtyFlag) override;

    protected:
        std::shared_ptr<DrawableContainer> drawableContainer;

        DrawableGroup();
        void init();
    };
}

#endif /* DrawableGroup_h */
