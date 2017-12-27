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
        static shared_ptr<RoundedRectangle> create(const Size &size, bool isRatio, float cornerRadius);

        ~RoundedRectangle();
        float getCornerRadius();
        virtual void getVerticesNum(int *num) override;
        virtual void getIndiciesNum(int *num) override;
        virtual void bindVertices(float *vertices, int *idx, bool bakeTransform = false) override;
        virtual void bindIndices(short *indices, int *idx, int start) override;
        virtual void bindVertexTexCoords(float *vertexTexCoords, int *idx, float x, float y, float w, float h) override;
        
        shared_ptr<RoundedRectangle> clone();
        virtual shared_ptr<Entity> cloneEntity() override;
        virtual EntityType getEntityType() override;

    protected:
        static unordered_map<float, weak_ptr<Texture2D>> cachedTexture2d;
        float cornerRadius = 0;
        
        RoundedRectangle();
        
        void init(const Size &size, bool isRatio, float cornerRadius);
        shared_ptr<Texture2D> createTexture(float cornerRadius);
        
        virtual void copyFrom(const shared_ptr<Entity> &src) override;
    };
}

#endif /* RoundedRectangle_h */
