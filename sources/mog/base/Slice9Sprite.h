#ifndef Slice9Sprite_h
#define Slice9Sprite_h

#include <string>
#include "mog/base/Sprite.h"

namespace mog {
    class Slice9Sprite : public Entity {
        friend class EntityCreator;
    public:
        static std::shared_ptr<Slice9Sprite> create(std::string filename, const Rect &centerRect, const Rect &rect = Rect::zero);
        static std::shared_ptr<Slice9Sprite> createWithTexture(const std::shared_ptr<Texture2D> &texture, const Rect &centerRect, const Rect &rect = Rect::zero);
        static std::shared_ptr<Slice9Sprite> createWithSprite(const std::shared_ptr<Sprite> &sprite, const Rect &centerRect);

        virtual void bindVertices(const std::shared_ptr<Renderer> &renderer, int *verticesIdx, int *indicesIdx, bool bakeTransform = false) override;
        virtual void bindVertexTexCoords(const std::shared_ptr<Renderer> &renderer, int *idx, int texIdx, float x, float y, float w, float h) override;
        
        Rect getRect();
        Rect getCenterRect();
        std::shared_ptr<Slice9Sprite> clone();

    protected:
        Slice9Sprite() {}
        
        virtual void init() override;
        void initWithTexture(const std::shared_ptr<Texture2D> &texture);
        virtual std::shared_ptr<Entity> cloneEntity() override;
        
        std::string filename = "";
        Rect rect = Rect::zero;
        Rect centerRect = Rect::zero;
    };
}


#endif /* Slice9Sprite_h */
