#ifndef TiledSprite_h
#define TiledSprite_h

#include <string>
#include "mog/base/Sprite.h"

namespace mog {
    class TiledSprite : public Entity {
        friend class EntityCreator;
    public:
        static std::shared_ptr<TiledSprite> create(std::string filename, const Size &size, const Rect &rect = Rect::zero);
        static std::shared_ptr<TiledSprite> createWithTexture(const std::shared_ptr<Texture2D> &texture, const Size &size, const Rect &rect = Rect::zero);
        static std::shared_ptr<TiledSprite> createWithSprite(const std::shared_ptr<Sprite> &sprite, const Size &size);

        Rect getRect();
        virtual void bindVertices(const std::shared_ptr<Renderer> &renderer, int *verticesIdx, int *indicesIdx, bool bakeTransform = false) override;
        virtual void bindVertexTexCoords(const std::shared_ptr<Renderer> &renderer, int *idx, int texIdx, float x, float y, float w, float h) override;
        std::shared_ptr<TiledSprite> clone();
        
    protected:
        TiledSprite() {}
        
        virtual void init() override;
        void initWithTexture(const std::shared_ptr<Texture2D> texture);
        virtual std::shared_ptr<Entity> cloneEntity() override;
        
        std::string filename = "";
        Rect rect = Rect::zero;
        mog::Size texSize = mog::Size::zero;
        int xCount = 0;
        int yCount = 0;
    };
}


#endif /* TiledSprite_h */
