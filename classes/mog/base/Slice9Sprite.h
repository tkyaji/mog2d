#ifndef Slice9Sprite_h
#define Slice9Sprite_h

#include <string>
#include "mog/base/Sprite.h"

namespace mog {
    class Slice9Sprite : public Sprite {
    public:
        static shared_ptr<Slice9Sprite> create(string filename, const Point &centerRectPosition, const Size &centerRectSize);
        static shared_ptr<Slice9Sprite> create(string filename, const Point &framePosition, const Size &frameSize, const Point &fixedRectPosition, const Size &fixedRectSize);

        virtual void getVerticesNum(int *num) override;
        virtual void getIndiciesNum(int *num) override;
        virtual void bindVertices(float *vertices, int *idx, bool bakeTransform = false) override;
        virtual void bindIndices(short *indices, int *idx, int start) override;
        virtual void bindVertexTexCoords(float *vertexTexCoords, int *idx, float x, float y, float w, float h) override;
        virtual void setReRenderFlag(unsigned char flag) override;

        shared_ptr<Slice9Sprite> clone();
        virtual shared_ptr<Entity> cloneEntity() override;
        
    protected:
        Slice9Sprite();
        
        void init (string filename, const Point &framePosition, const Size &frameSize, const Point &centerRectPosition, const Size &centerRectSize);
        
        Point centerRectPosition = Point::zero;
        Size centerRectSize = Size::zero;
        
        virtual void copyFrom(const shared_ptr<Entity> &src) override;
    };
}


#endif /* Slice9Sprite_h */
