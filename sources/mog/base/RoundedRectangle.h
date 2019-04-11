#ifndef RoundedRectangle_h
#define RoundedRectangle_h

#include <memory>
#include <unordered_map>
#include "mog/base/Sprite.h"
#include "mog/core/plain_objects.h"

#define CORNER_TOP_LEFT     0b00000001
#define CORNER_TOP_RIGHT    0b00000010
#define CORNER_BOTTOM_LEFT  0b00000100
#define CORNER_BOTTOM_RIGHT 0b00001000
#define CORNER_TOP          0b00000011
#define CORNER_BOTTOM       0b00001100
#define CORNER_LEFT         0b00000101
#define CORNER_RIGHT        0b00001010
#define CORNER_ALL          0b00001111

namespace mog {
    class RoundedRectangle : public Sprite {
    public:
        static shared_ptr<RoundedRectangle> create(const Size &size, float cornerRadius, unsigned char cornerFlag = CORNER_ALL);
        static shared_ptr<RoundedRectangle> create(float width, float height, float cornerRadius, unsigned char cornerFlag = CORNER_ALL);
        
        float getCornerRadius();
        
    protected:
        float cornerRadius = 0;
        unsigned char cornerFlag = CORNER_ALL;
        
        RoundedRectangle();
        
        void init(const Size &size, float cornerRadius, unsigned char cornerFlag);
        virtual void bindVertices(const std::shared_ptr<Renderer> &renderer, int *verticesIdx, int *indicesIdx, bool bakeTransform) override;
        virtual void bindVertexTexCoords(const std::shared_ptr<Renderer> &renderer, int *idx, float x, float y, float w, float h) override;
    };
}

#endif /* RoundedRectangle_h */
