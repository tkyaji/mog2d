#ifndef Slice9Sprite_h
#define Slice9Sprite_h

#include <string>
#include "mog/base/Sprite.h"

namespace mog {
    class Slice9Sprite : public Sprite {
    public:
        static shared_ptr<Slice9Sprite> create(const shared_ptr<Sprite> sprite, const Rect &centerRect);

//        virtual void getVerticesNum(int *verticesNum, int *indiciesNum) override;
        virtual void bindVertices(const std::shared_ptr<Renderer> &renderer, int *verticesIdx, int *indicesIdx, bool bakeTransform = false) override;
        virtual void bindVertexTexCoords(const std::shared_ptr<Renderer> &renderer, int *idx, float x, float y, float w, float h) override;
//        virtual void setReRenderFlag(unsigned char flag) override;
        
        Rect getCenterRect();

    protected:
        Slice9Sprite();
        
        void init(const shared_ptr<Sprite> sprite, const Rect &centerRect);
        Rect centerRect = Rect::zero;
    };
}


#endif /* Slice9Sprite_h */
