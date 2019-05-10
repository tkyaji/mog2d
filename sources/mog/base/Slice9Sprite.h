#ifndef Slice9Sprite_h
#define Slice9Sprite_h

#include <string>
#include "mog/base/Sprite.h"

namespace mog {
    class Slice9Sprite : public Entity {
    public:
        static std::shared_ptr<Slice9Sprite> create(std::string filename, const Rect &centerRect, const Rect &rect = Rect::zero);
        static std::shared_ptr<Slice9Sprite> create(const std::shared_ptr<Texture2D> &texture, const Rect &centerRect, const Rect &rect = Rect::zero);
        static std::shared_ptr<Slice9Sprite> create(const std::shared_ptr<Sprite> &sprite, const Rect &centerRect);

        virtual void bindVertices(const std::shared_ptr<Renderer> &renderer, int *verticesIdx, int *indicesIdx, bool bakeTransform = false) override;
        virtual void bindVertexTexCoords(const std::shared_ptr<Renderer> &renderer, int *idx, int texIdx, float x, float y, float w, float h) override;
        
        Rect getRect();
        Rect getCenterRect();

    protected:
        Slice9Sprite() {}
        
        void init(const std::shared_ptr<Texture2D> &texture, const Rect &centerRect, const Rect &rect);
        virtual std::shared_ptr<Entity> cloneEntity() override;
        
        Rect rect = Rect::zero;
        Rect centerRect = Rect::zero;
    };
}


#endif /* Slice9Sprite_h */
