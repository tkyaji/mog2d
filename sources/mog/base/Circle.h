#ifndef Circle_h
#define Circle_h

#include <memory>
#include "mog/base/Sprite.h"
#include "mog/core/plain_objects.h"
#include "mog/core/Collision.h"

namespace mog {
    class Circle : public Sprite {
    public:
        static shared_ptr<Circle> create(float radius);
        
        float getRadius();
        void setRadius(float radius);
        float getAbsoluteRadius();
        virtual shared_ptr<Collider> getCollider() override;

    protected:
        float radius = 0;
        
        Circle();
        void init(float radius);
        virtual void bindVertices(const std::shared_ptr<Renderer> &renderer, int *verticesIdx, int *indicesIdx, bool bakeTransform) override;
        virtual void bindVertexTexCoords(const std::shared_ptr<Renderer> &renderer, int *idx, float x, float y, float w, float h) override;
        virtual shared_ptr<CIRCLE> getCIRCLE();
    };
}

#endif /* Circle_h */
