#ifndef RoundedRectangle_h
#define RoundedRectangle_h

#include <memory>
#include <unordered_map>
#include "mog/base/Sprite.h"
#include "mog/core/plain_objects.h"

namespace mog {
    class RoundedRectangle : public Sprite {
    public:
        static shared_ptr<RoundedRectangle> create(const Size &size, float cornerRadius);
        static shared_ptr<RoundedRectangle> create(float width, float height, float cornerRadius);

        float getCornerRadius();
        
    protected:
        float cornerRadius = 0;
        
        RoundedRectangle();
        
        void init(const Size &size, float cornerRadius);
        virtual void bindVertices(const std::shared_ptr<Renderer> &renderer, int *verticesIdx, int *indicesIdx, bool bakeTransform) override;
        virtual void bindVertexTexCoords(const std::shared_ptr<Renderer> &renderer, int *idx, float x, float y, float w, float h) override;
    };
}

#endif /* RoundedRectangle_h */
