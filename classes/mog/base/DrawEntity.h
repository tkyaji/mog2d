#ifndef DrawEntity_h
#define DrawEntity_h

#include <memory>
#include "mog/base/Entity.h"
#include "mog/core/Texture2D.h"

namespace mog {
    class DrawEntity : public Entity {
    public:
        ~DrawEntity();

        virtual void bindVertexTexCoords(float *vertexTexCoords, int *idx, float x, float y, float w, float h) override;
        virtual void bindVertexColors(float *vertexColors, int *idx, const Color &parentColor = Color::white) override;
        
    protected:
        bool dynamicDraw = false;
        DrawEntity();
        virtual void bindVertex() override;
    };
}

#endif /* DrawEntity_h */
