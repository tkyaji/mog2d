#ifndef Group_h
#define Group_h

#include <memory>
#include <map>
#include <vector>
#include "mog/core/plain_objects.h"
#include "mog/core/TextureAtlas.h"
#include "mog/base/Entity.h"

namespace mog {
    class Group : public Entity {
    public:
        static shared_ptr<Group> create();
        
        void add(const shared_ptr<Entity> &entity);
        void remove(const shared_ptr<Entity> &entity);
        void removeAll();
        vector<shared_ptr<Entity>> getChildEntities();
        
        void enableSortOrderDirty();
        shared_ptr<Group> clone();
        
        virtual void drawFrame(float delta) override;
        virtual void updateFrame(float delta) override;
        virtual void updateMatrix() override;
        virtual void getVerticesNum(int *num) override;
        virtual void getIndiciesNum(int *num) override;
        virtual void bindVertices(float *vertices, int *idx, bool bakeTransform = false) override;
        virtual void bindIndices(short *indices, int *idx, int start) override;
        virtual void bindVertexSub() override;
        virtual void bindVertexTexCoords(const shared_ptr<TextureAtlas> &textureAtlas, float *vertexTexCoords, int *idx, float x, float y, float w, float h);
        virtual void bindVertexColors(float *vertexColors, int *idx, const Color &parentColor = Color::white) override;
        virtual void setReRenderFlagToChild(unsigned char flag) override;

        virtual void addTextureTo(const shared_ptr<TextureAtlas> &textureAtlas) override;
        virtual shared_ptr<Entity> cloneEntity() override;

    protected:
        bool sortOrderDirty = false;
        vector<shared_ptr<Entity>> childEntities;
        vector<shared_ptr<Entity>> childEntitiesToDraw;
        vector<shared_ptr<Entity>> getSortedChildEntitiesToDraw();
        
        Group();
        
        void sortChildEntitiesToDraw();
        
        virtual void copyFrom(const shared_ptr<Entity> &src) override;
    };
}

#endif /* Group_h */
