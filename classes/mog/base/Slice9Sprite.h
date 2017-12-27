#ifndef Slice9Sprite_h
#define Slice9Sprite_h

#include <string>
#include "mog/base/Sprite.h"

namespace mog {
    class Slice9Sprite : public Sprite {
    public:
        static shared_ptr<Slice9Sprite> create(const shared_ptr<Sprite> sprite, const Rect &centerRect);

        virtual void getVerticesNum(int *num) override;
        virtual void getIndiciesNum(int *num) override;
        virtual void bindVertices(float *vertices, int *idx, bool bakeTransform = false) override;
        virtual void bindIndices(short *indices, int *idx, int start) override;
        virtual void bindVertexTexCoords(float *vertexTexCoords, int *idx, float x, float y, float w, float h) override;
        virtual void setReRenderFlag(unsigned char flag) override;
        
        Rect getCenterRect();

        shared_ptr<Slice9Sprite> clone();
        virtual shared_ptr<Entity> cloneEntity() override;
        virtual EntityType getEntityType() override;

    protected:
        Slice9Sprite();
        
        void init(const shared_ptr<Sprite> sprite, const Rect &centerRect);
        Rect centerRect = Rect::zero;
        
        virtual void copyFrom(const shared_ptr<Entity> &src) override;
    };
}


#endif /* Slice9Sprite_h */
