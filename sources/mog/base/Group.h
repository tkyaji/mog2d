#ifndef Group_h
#define Group_h

#include <memory>
#include <map>
#include <set>
#include <vector>
#include "mog/core/plain_objects.h"
#include "mog/core/TextureAtlas.h"
#include "mog/base/Entity.h"
#include "mog/core/TouchInput.h"
#include "mog/base/DrawableGroup.h"

namespace mog {
    class Sprite;
    
    class Group : public Entity {
        friend class EntityCreator;
    public:
        static std::shared_ptr<Group> create(bool enableBatching = false);

        void setEnableBatching(bool enableBatching);
        bool isEnableBatching();

        virtual void add(const std::shared_ptr<Entity> &entity);
        virtual void remove(const std::shared_ptr<Entity> &entity);
        virtual void removeAll();
        std::vector<std::shared_ptr<Entity>> getChildEntities();
        std::shared_ptr<Entity> findChildByName(std::string name, bool recursive = true);
        std::shared_ptr<Entity> findFirstChildByTag(std::string tag, bool recursive = true);
        std::vector<std::shared_ptr<Entity>> findChildrenByTag(std::string tag, bool recursive = true);
        std::shared_ptr<Group> clone();

        virtual void updateFrame(const std::shared_ptr<Engine> &engine, float delta, float *parentMatrix, unsigned char parentDirtyFlag = 0) override;
        virtual void drawFrame(float delta, const std::map<unsigned int, TouchInput> &touches) override;
        
        virtual std::shared_ptr<Dictionary> serialize() override;

    protected:
        std::shared_ptr<DrawableGroup> drawableGroup;
        bool sortOrderDirty = true;
        bool enableBatching = false;
        bool enableTexture = false;
        unsigned char dirtyFlagChildren = 0;
        std::unordered_map<unsigned long, std::shared_ptr<TextureAtlasCell>> cellMap;
        std::shared_ptr<TextureAtlas> textureAtlas;

        Group();
        virtual void init() override;
        std::shared_ptr<Sprite> createTextureSprite();

        virtual void bindVertex() override;
        virtual void bindVertexSub();
        virtual std::shared_ptr<Entity> cloneEntity() override;
        virtual void deserializeData(const std::shared_ptr<Dictionary> &dict) override;
        
        void bindVertexRecursive(const std::shared_ptr<Renderer> &renderer, std::shared_ptr<TextureAtlas> &textureAtlas, int *vertexIndices, float *parentMatrix);
        void bindVertexSubRecursive(const std::shared_ptr<Renderer> &renderer, std::shared_ptr<TextureAtlas> &textureAtlas, int *vertexIndices, float *parentMatrix);

        virtual void addTextureTo(const std::shared_ptr<TextureAtlas> &textureAtlas);
    };
}

#endif /* Group_h */
