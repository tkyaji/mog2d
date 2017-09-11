#ifndef BatchingGroup_h
#define BatchingGroup_h

#include <memory>
#include "mog/base/Group.h"
#include "mog/base/Sprite.h"
#include "mog/core/Texture2D.h"
#include "mog/core/TextureAtlas.h"
#include "mog/core/opengl.h"
#include "mog/core/Renderer.h"

using namespace std;

namespace mog {
    class BatchingGroup : public Group {
    public:
        static shared_ptr<BatchingGroup> create();
        
        virtual void drawFrame(float delta) override;
        virtual void bindVertexSub() override;
        virtual void bindVertices(float *vertices, int *idx, bool bakeTransform = false) override;
        
        shared_ptr<Sprite> createTextureSprite();
        shared_ptr<BatchingGroup> clone();
        virtual shared_ptr<Entity> cloneEntity() override;

    protected:
        unordered_map<unsigned long, shared_ptr<TextureAtlasCell>> cellMap;
        shared_ptr<TextureAtlas> textureAtlas;

        BatchingGroup();
        
        virtual void bindVertex() override;

    private:
        Color getParentColor();
        float tmpMatrix[16] = {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1,
        };
    };
}


#endif /* BatchingGroup_h */
