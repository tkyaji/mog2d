#ifndef Slice9Sprite_h
#define Slice9Sprite_h

#include <string>
#include "mog/base/Sprite.h"

namespace mog {
    class Slice9Sprite : public Entity {
        friend class EntityCreator;
    public:
        static std::shared_ptr<Slice9Sprite> create(std::string filename, const Rect &centerRect, const Size &size = Size::zero, const Rect &rect = Rect::zero);
        static std::shared_ptr<Slice9Sprite> createWithTexture(const std::shared_ptr<Texture2D> &texture, const Rect &centerRect, const Size &size = Size::zero, const Rect &rect = Rect::zero);
        static std::shared_ptr<Slice9Sprite> createWithSprite(const std::shared_ptr<Sprite> &sprite, const Rect &centerRect, const Size &size, const Rect &rect = Rect::zero);

        virtual void bindVertices(const std::shared_ptr<Renderer> &renderer, int *verticesIdx, int *indicesIdx, bool bakeTransform = false) override;
        virtual void bindVertexTexCoords(const std::shared_ptr<Renderer> &renderer, int *idx, int texIdx, float x, float y, float w, float h) override;
        
        std::string getFilename();
        void setFilename(std::string filename, bool withReset = false);
        Rect getRect();
        void setRect(const Rect &rect);
        Rect getCenterRect();
        void setCenterRect(const Rect &centerRect);
        std::shared_ptr<Slice9Sprite> clone();
        virtual std::shared_ptr<Dictionary> serialize() override;

    protected:
        Slice9Sprite() {}
        
        virtual void init() override;
        void initWithTexture(const std::shared_ptr<Texture2D> &texture);
        virtual std::shared_ptr<Entity> cloneEntity() override;
        virtual void deserializeData(const std::shared_ptr<Dictionary> &dict, const std::unordered_map<std::string, std::unordered_map<std::string, std::shared_ptr<Data>>> &params) override;

        std::string filename = "";
        Rect rect = Rect::zero;
        Rect centerRect = Rect::zero;
    };
}


#endif /* Slice9Sprite_h */
