#ifndef TiledSprite_h
#define TiledSprite_h

#include <string>
#include "mog/base/Sprite.h"

namespace mog {
    class TiledSprite : public Sprite {
    public:
        static shared_ptr<TiledSprite> create(const shared_ptr<Sprite> sprite, const Size &size);
        
        virtual void bindVertices(const std::shared_ptr<Renderer> &renderer, int *verticesIdx, int *indicesIdx, bool bakeTransform = false) override;
        virtual void bindVertexTexCoords(const std::shared_ptr<Renderer> &renderer, int *idx, float x, float y, float w, float h) override;
        
    protected:
        TiledSprite();
        
        void init(const shared_ptr<Sprite> sprite, const Size &size);
        
        mog::Size texSize = mog::Size::zero;
        int xCount = 0;
        int yCount = 0;
    };
}


#endif /* TiledSprite_h */
