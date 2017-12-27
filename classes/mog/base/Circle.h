#ifndef Circle_h
#define Circle_h

#include <memory>
#include "mog/base/Sprite.h"
#include "mog/core/plain_objects.h"

namespace mog {
    class Circle : public Sprite {
    public:
        static shared_ptr<Circle> create(float radius);
        
        float getRadius();
        void setRadius(float radius);
        
        virtual void getVerticesNum(int *num) override;
        virtual void getIndiciesNum(int *num) override;
        virtual void bindVertices(float *vertices, int *idx, bool bakeTransform = false) override;
        virtual void bindIndices(short *indices, int *idx, int start) override;
        virtual void bindVertexTexCoords(float *vertexTexCoords, int *idx, float x, float y, float w, float h) override;
        
        virtual shared_ptr<Entity> cloneEntity() override;
        shared_ptr<Circle> clone();
        virtual EntityType getEntityType() override;

    protected:
        float radius = 0;
        
        Circle();
        
        void init(float radius);
        shared_ptr<CIRCLE> getCIRCLE();
        virtual shared_ptr<Collider> getCollider() override;
        virtual void copyFrom(const shared_ptr<Entity> &src) override;
    };
}

#endif /* Circle_h */
