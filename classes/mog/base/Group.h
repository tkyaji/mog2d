#ifndef Group_h
#define Group_h

#include <memory>
#include <map>
#include <set>
#include <vector>
#include "mog/core/plain_objects.h"
#include "mog/core/TextureAtlas.h"
#include "mog/base/Entity.h"

namespace mog {
    class Sprite;
    
    class Group : public Entity {
    public:
        static shared_ptr<Group> create(bool enableBatching = false);
        
        void add(const shared_ptr<Entity> &entity);
        void remove(const shared_ptr<Entity> &entity);
        void removeAll();
        vector<shared_ptr<Entity>> getChildEntities();
        void setEnableBatching(bool enableBatching);
        bool isEnableBatching();
        
        shared_ptr<Entity> findChildByName(string name, bool recursive = true);
        vector<shared_ptr<Entity>> findChildrenByTag(string tag, bool recursive = true);

        void enableSortOrderDirty();
        shared_ptr<Group> clone();
        
        virtual void updateFrame(const shared_ptr<Engine> &engine, float delta) override;
        virtual void drawFrame(float delta) override;
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
        virtual EntityType getEntityType() override;

    protected:
        bool sortOrderDirty = false;
        bool enableBatching = false;
        unordered_map<unsigned long, shared_ptr<TextureAtlasCell>> cellMap;
        shared_ptr<TextureAtlas> textureAtlas;

        vector<shared_ptr<Entity>> childEntities;
        vector<shared_ptr<Entity>> childEntitiesToDraw;
        set<uintptr_t> entityIdSet;

        Group();
        
        void sortChildEntitiesToDraw();
        vector<shared_ptr<Entity>> getSortedChildEntitiesToDraw();
        shared_ptr<Sprite> createTextureSprite();

        virtual void bindVertex() override;
        virtual void copyFrom(const shared_ptr<Entity> &src) override;
        
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

#endif /* Group_h */

