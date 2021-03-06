#ifndef Circle_h
#define Circle_h

#include <memory>
#include "mog/base/Sprite.h"
#include "mog/core/plain_objects.h"
#include "mog/core/Collision.h"

namespace mog {
    class Circle : public Sprite {
        friend class EntityCreator;
    public:
        static std::shared_ptr<Circle> create(float radius);
        
        float getRadius();
        void setRadius(float radius);
        virtual std::shared_ptr<Collider> getCollider() override;
        std::shared_ptr<Circle> clone();
        virtual std::shared_ptr<Dictionary> serialize() override;
        
    protected:
        float radius = 0;
        
        Circle() {}
        virtual void init() override;
        virtual void bindVertices(const std::shared_ptr<Renderer> &renderer, int *verticesIdx, int *indicesIdx, bool bakeTransform) override;
        virtual void bindVertexTexCoords(const std::shared_ptr<Renderer> &renderer, int *idx, int texIdx, float x, float y, float w, float h) override;
        virtual std::shared_ptr<CIRCLE> getCIRCLE();
        virtual std::shared_ptr<Entity> cloneEntity() override;
        virtual void deserializeData(const std::shared_ptr<Dictionary> &dict, const std::unordered_map<std::string, std::unordered_map<std::string, std::shared_ptr<Data>>> &params) override;
    };
}

#endif /* Circle_h */
