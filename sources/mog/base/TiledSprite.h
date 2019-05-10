#ifndef TiledSprite_h
#define TiledSprite_h

#include <string>
#include "mog/base/Sprite.h"

namespace mog {
    class TiledSprite : public Entity {
    public:
        static std::shared_ptr<TiledSprite> create(std::string filename, const Size &size, const Rect &rect = Rect::zero);
        static std::shared_ptr<TiledSprite> create(const std::shared_ptr<Texture2D> &texture, const Size &size, const Rect &rect = Rect::zero);
        static std::shared_ptr<TiledSprite> create(const std::shared_ptr<Sprite> &sprite, const Size &size);

        Rect getRect();
        virtual void bindVertices(const std::shared_ptr<Renderer> &renderer, int *verticesIdx, int *indicesIdx, bool bakeTransform = false) override;
        virtual void bindVertexTexCoords(const std::shared_ptr<Renderer> &renderer, int *idx, int texIdx, float x, float y, float w, float h) override;
        std::shared_ptr<TiledSprite> clone();
        
    protected:
        TiledSprite() {}
        
        void init(const std::shared_ptr<Texture2D> texture, const Size &size, const Rect &rect);
        virtual std::shared_ptr<Entity> cloneEntity() override;
        
        Rect rect = Rect::zero;
        mog::Size texSize = mog::Size::zero;
        int xCount = 0;
        int yCount = 0;
    };
}


#endif /* TiledSprite_h */
