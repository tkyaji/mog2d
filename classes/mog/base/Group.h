#ifndef Group_h
#define Group_h

#include <memory>
#include <map>
#include <set>
#include <vector>
#include "mog/core/plain_objects.h"
#include "mog/core/TextureAtlas.h"
#include "mog/base/Entity.h"
#include "mog/base/DrawableGroup.h"

namespace mog {
    class Sprite;
    
    class Group : public Entity {
        friend class Entity;
    public:
        static std::shared_ptr<Group> create(bool enableBatching = false);
        
        void add(const std::shared_ptr<Entity> &entity);
        void remove(const std::shared_ptr<Entity> &entity);
        void removeAll();
        std::vector<std::shared_ptr<Entity>> getChildEntities();
        void setEnableBatching(bool enableBatching);
        bool isEnableBatching();
        
        std::shared_ptr<Entity> findChildByName(std::string name, bool recursive = true);
        std::shared_ptr<Entity> findFirstChildByTag(std::string tag, bool recursive = true);
        std::vector<shared_ptr<Entity>> findChildrenByTag(std::string tag, bool recursive = true);

        virtual void updateFrame(const std::shared_ptr<Engine> &engine, float delta, float *parentMatrix, unsigned char parentReRenderFlag = 0) override;
        virtual void drawFrame(float delta) override;
        
    protected:
        std::shared_ptr<DrawableGroup> drawableGroup;
        bool sortOrderDirty = true;
        bool enableBatching = false;
        bool enableTexture = false;
        unsigned char reRenderFlagChildren = 0;
        std::unordered_map<unsigned long, std::shared_ptr<TextureAtlasCell>> cellMap;
        std::shared_ptr<TextureAtlas> textureAtlas;

        Group();
        
        std::shared_ptr<Sprite> createTextureSprite();

        virtual void bindVertex() override;
        virtual void bindVertexSub();

        void bindVertexRecursive(const std::shared_ptr<Renderer> &renderer, std::shared_ptr<TextureAtlas> &textureAtlas, int *vertexIndices, float *parentMatrix);
        void bindVertexSubRecursive(const std::shared_ptr<Renderer> &renderer, std::shared_ptr<TextureAtlas> &textureAtlas, int *vertexIndices, float *parentMatrix);

        virtual void addTextureTo(const std::shared_ptr<TextureAtlas> &textureAtlas);

    private:
        Color getParentColor();
    };
}

#endif /* Group_h */
