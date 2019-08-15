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
#include "mog/base/DrawableContainer.h"

namespace mog {
    class Sprite;
    
    class Group : public Entity {
        friend class EntityCreator;
    public:
        static std::shared_ptr<Group> create(bool enableBatching = false);

        void setEnableBatching(bool enableBatching);
        bool isEnableBatching();

        virtual void add(const std::shared_ptr<Entity> &entity);
        virtual void insertBefore(const std::shared_ptr<Entity> &entity, const std::shared_ptr<Entity> &baseEntity);
        virtual void insertAfter(const std::shared_ptr<Entity> &entity, const std::shared_ptr<Entity> &baseEntity);
        virtual void remove(const std::shared_ptr<Entity> &entity);
        virtual void removeAll();
        std::vector<std::shared_ptr<Entity>> getChildEntities();
        std::shared_ptr<Entity> findChildByName(std::string name, bool recursive = true);
        std::shared_ptr<Entity> findFirstChildByTag(std::string tag, bool recursive = true);
        std::vector<std::shared_ptr<Entity>> findChildrenByTag(std::string tag, bool recursive = true);
        std::shared_ptr<Group> clone();

//        virtual void updateFrame(const std::shared_ptr<Engine> &engine, float delta, float *parentMatrix, unsigned char parentDirtyFlag = 0) override;
        virtual void updateFrame(const std::shared_ptr<Engine> &engine, float delta, float *parentMatrix, float *parentRendererMatrix, unsigned char parentDirtyFlag) override;
        virtual void updateFrameForChild(const std::shared_ptr<Engine> &engine, float delta, const std::shared_ptr<Entity> &entity, float *parentMatrix, float *parentRendererMatrix, unsigned char parentDirtyFlag);
        virtual void drawFrame(float delta, const std::map<unsigned int, TouchInput> &touches) override;
        virtual void updateMatrix(float *parentMatrix, unsigned char parentDirtyFlag) override;

        virtual std::shared_ptr<Dictionary> serialize() override;

    protected:
        std::shared_ptr<DrawableContainer> drawableContainer;
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
        virtual void deserializeData(const std::shared_ptr<Dictionary> &dict, const std::unordered_map<std::string, std::unordered_map<std::string, std::shared_ptr<Data>>> &params) override;
        virtual bool isGroup() override;
        
        void bindVertexRecursive(const std::shared_ptr<Renderer> &renderer, std::shared_ptr<TextureAtlas> &textureAtlas, int *vertexIndices);
        void bindVertexSubRecursive(const std::shared_ptr<Renderer> &renderer, std::shared_ptr<TextureAtlas> &textureAtlas, int *vertexIndices);
//        virtual void multiplyChildEntityMatrix(const std::shared_ptr<Entity> &entity, float *parentMatrix);

        virtual void addTextureTo(const std::shared_ptr<TextureAtlas> &textureAtlas);
    };
}

#endif /* Group_h */
