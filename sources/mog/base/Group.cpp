#include "mog/Constants.h"
#include "mog/base/Drawable.h"
#include "mog/base/Group.h"
#include "mog/base/Sprite.h"
#include "mog/core/Engine.h"
#include "mog/core/EntityCreator.h"
#include <algorithm>

#define VERTICES_IDX 0
#define INDICES_IDX 1
#define VERTEX_COLORS_IDX 2
#define VERTEX_TEX_COORDS_IDX 3

using namespace mog;

std::shared_ptr<Group> Group::create(bool enableBatching) {
    auto group = std::shared_ptr<Group>(new Group());
    group->enableBatching = enableBatching;
    group->init();
    return group;
}

Group::Group() {
    this->drawableContainer = std::make_shared<DrawableContainer>();
    this->drawableContainer->addChildListener = [this](const std::shared_ptr<Drawable> &drawable) {
        auto e = std::static_pointer_cast<Entity>(drawable);
        e->group = shared_from_this();
    };
    this->drawableContainer->removeChildListener = [this](const std::shared_ptr<Drawable> &drawable) {
        auto e = std::static_pointer_cast<Entity>(drawable);
        e->group.reset();
    };
}

void Group::init() {
    this->dirtyFlag |= (DIRTY_ALL | DIRTY_SIZE | DIRTY_ANCHOR);
}

void Group::setEnableBatching(bool enableBatching) {
    this->enableBatching = enableBatching;
    this->dirtyFlag = DIRTY_ALL;
}

bool Group::isEnableBatching() {
    return this->enableBatching;
}

/*
void Group::updateFrame(const std::shared_ptr<Engine> &engine, float delta, float *parentMatrix, unsigned char parentDirtyFlag) {
    this->drawableContainer->sortChildDrawablesToDraw();
    Entity::updateFrame(engine, delta, parentMatrix, parentDirtyFlag);
    int verticesNum = 0;
    int indicesNum = 0;
    for (const auto &drawable : this->drawableContainer->sortedChildDrawables) {
        auto entity = std::static_pointer_cast<Entity>(drawable);
        if (this->enableBatching) {
            parentDirtyFlag |= (parentDirtyFlag | this->dirtyFlag);
        } else {
            entity->dirtyFlag |= (this->dirtyFlag & (DIRTY_VERTEX | DIRTY_COLOR));
        }
        entity->updateFrame(engine, delta, this->matrix, parentDirtyFlag);

        verticesNum += entity->renderer->verticesNum;
        if (indicesNum > 0) indicesNum += 2;
        indicesNum += entity->renderer->indicesNum;

        if (auto g = std::dynamic_pointer_cast<Group>(entity)) {
            if (g->enableTexture) this->enableTexture = true;
            this->dirtyFlagChildren |= (entity->dirtyFlag | g->dirtyFlagChildren);
        } else {
            if (entity->textures[0]) this->enableTexture = true;
            this->dirtyFlagChildren |= entity->dirtyFlag;
        }
    }
    if (this->renderer->setVerticesNum(verticesNum)) {
        this->renderer->newVerticesArr();
        this->renderer->newVertexColorsArr();
        if (this->enableTexture) {
            this->renderer->newVertexTexCoordsArr();
        }
    }
    if (this->renderer->setIndicesNum(indicesNum)) {
        this->renderer->newIndicesArr();
    }
}
*/

void Group::updateFrame(const std::shared_ptr<Engine> &engine, float delta, float *parentMatrix, float *parentRendererMatrix, unsigned char parentDirtyFlag) {
    this->drawableContainer->sortChildDrawablesToDraw();
    Entity::updateFrame(engine, delta, parentMatrix, parentRendererMatrix, (parentDirtyFlag & ~IN_BATCHING));
    int verticesNum = 0;
    int indicesNum = 0;

    float *rendererMatrix = this->renderer->matrix;
    unsigned char dirtyFlag = (parentDirtyFlag | this->dirtyFlag);
    if (this->enableBatching && (parentDirtyFlag & IN_BATCHING) == 0) {
        rendererMatrix = Renderer::identityMatrix;
        dirtyFlag |= IN_BATCHING;
    }
    for (const auto &drawable : this->drawableContainer->sortedChildDrawables) {
        auto entity = std::static_pointer_cast<Entity>(drawable);
        this->updateFrameForChild(engine, delta, entity, this->matrix, rendererMatrix, dirtyFlag);
        
        verticesNum += entity->renderer->verticesNum;
        if (indicesNum > 0) indicesNum += 2;
        indicesNum += entity->renderer->indicesNum;
    }
    if (this->renderer->setVerticesNum(verticesNum)) {
        this->renderer->newVerticesArr();
        this->renderer->newVertexColorsArr();
        if (this->enableTexture) {
            this->renderer->newVertexTexCoordsArr();
        }
    }
    if (this->renderer->setIndicesNum(indicesNum)) {
        this->renderer->newIndicesArr();
    }
}

void Group::updateFrameForChild(const std::shared_ptr<Engine> &engine, float delta, const std::shared_ptr<Entity> &entity, float *parentMatrix, float *parentRendererMatrix, unsigned char parentDirtyFlag) {
    entity->updateFrame(engine, delta, parentMatrix, parentRendererMatrix, parentDirtyFlag);

    if (entity->isGroup()) {
        auto g = std::static_pointer_cast<Group>(entity);
        if (g->enableTexture) this->enableTexture = true;
        this->dirtyFlagChildren |= (entity->dirtyFlag | g->dirtyFlagChildren);
    } else {
        if (entity->textures[0]) this->enableTexture = true;
        this->dirtyFlagChildren |= entity->dirtyFlag;
    }
}

void Group::drawFrame(float delta, const std::map<unsigned int, TouchInput> &touches) {
    if (!this->active) return;
    
    if (this->enableBatching) {
        if ((this->dirtyFlag & DIRTY_VERTEX) == DIRTY_VERTEX) {
            this->renderer->shader->setUniformMatrix(this->renderer->matrix);
        }
        if ((this->dirtyFlag & DIRTY_COLOR) == DIRTY_COLOR) {
            this->renderer->shader->setUniformColor(this->renderer->matrix[16], this->renderer->matrix[17], this->renderer->matrix[18], this->renderer->matrix[19]);
        }
        if ((this->dirtyFlag & DIRTY_VERTEX) == DIRTY_VERTEX) {
            this->bindVertex();
            
        } else if ((this->dirtyFlagChildren & DIRTY_RENDERER_ALL) > 0) {
            if (this->enableTexture && (this->dirtyFlagChildren & DIRTY_TEXTURE) == DIRTY_TEXTURE) {
                this->bindVertex();
            } else {
                this->bindVertexSub();
            }
        }
        
        this->renderer->drawFrame();

    } else {
        for (const auto &drawable : this->drawableContainer->sortedChildDrawables) {
            auto entity = std::static_pointer_cast<Entity>(drawable);
//            this->multiplyChildEntityMatrix(entity, this->renderer->matrix);
            entity->drawFrame(delta, touches);
        }
    }
    if ((this->dirtyFlag & DIRTY_VERTEX) == DIRTY_VERTEX) {
        this->collider = nullptr;
    }
    this->dirtyFlag = 0;
    this->dirtyFlagChildren = 0;
}

void Group::updateMatrix(float *parentMatrix, unsigned char parentDirtyFlag) {
    Entity::updateMatrix(parentMatrix, parentDirtyFlag);
    unsigned char dirtyFlag = (this->dirtyFlag | parentDirtyFlag);
    float matrix[16];
    Transform::multiplyMatrix(this->matrix, parentMatrix, matrix);
    for (const auto &drawable : this->drawableContainer->sortedChildDrawables) {
        drawable->updateMatrix(matrix, dirtyFlag);
    }
}

void Group::bindVertex() {
    if (!this->enableBatching) return;
    
    int vertexIndices[4] = {0, 0, 0, 0};
    
    if (this->enableTexture) {
        this->textureAtlas = std::make_shared<TextureAtlas>();
        this->addTextureTo(this->textureAtlas);
        this->textures[0] = this->textureAtlas->createTexture();
        this->textureAtlas->bindTexture();
    }

    this->bindVertexRecursive(this->renderer, this->textureAtlas, vertexIndices);
    this->renderer->bindVertex(true);
    this->renderer->bindVertexColors(true);
    if (this->enableTexture) {
        this->renderer->bindVertexTexCoords();
        this->renderer->bindTexture(this->textures[0]);
    }
}

void Group::bindVertexRecursive(const std::shared_ptr<Renderer> &renderer, std::shared_ptr<TextureAtlas> &textureAtlas, int *vertexIndices) {
    for (const auto &drawable : this->drawableContainer->sortedChildDrawables) {
        auto entity = std::static_pointer_cast<Entity>(drawable);
//        this->multiplyChildEntityMatrix(entity, parentMatrix);

        if (entity->isGroup()) {
            auto g = std::static_pointer_cast<Group>(entity);
            g->bindVertexRecursive(renderer, textureAtlas, vertexIndices);
            
        } else {
            entity->bindVertices(renderer, &vertexIndices[VERTICES_IDX], &vertexIndices[INDICES_IDX], true);
            entity->bindVertexColors(renderer, &vertexIndices[VERTEX_COLORS_IDX]);
            std::shared_ptr<TextureAtlasCell> cell = nullptr;
            if (this->enableTexture) {
                if (entity->textures[0]) cell = textureAtlas->getCell(entity->textures[0]);
                float x = 0;
                float y = 0;
                float w = 0;
                float h = 0;
                if (cell) {
                    x = (float)cell->x / (float)textureAtlas->width;
                    y = (float)cell->y / (float)textureAtlas->height;
                    w = (float)cell->width / (float)textureAtlas->width;
                    h = (float)cell->height / (float)textureAtlas->height;
                }
                entity->bindVertexTexCoords(renderer, &vertexIndices[VERTEX_TEX_COORDS_IDX], 0, x, y, w, h);
            }
        }
        entity->dirtyFlag = 0;
    }
}

void Group::bindVertexSub() {
    int vertexIndices[4] = {0, 0, 0, 0};
    this->bindVertexSubRecursive(this->renderer, this->textureAtlas, vertexIndices);
}

void Group::bindVertexSubRecursive(const std::shared_ptr<Renderer> &renderer, std::shared_ptr<TextureAtlas> &textureAtlas, int *vertexIndices) {
    for (const auto &drawable : this->drawableContainer->sortedChildDrawables) {
        auto entity = std::static_pointer_cast<Entity>(drawable);
//        this->multiplyChildEntityMatrix(entity, parentMatrix);
        
        if (entity->isGroup()) {
            auto g = std::static_pointer_cast<Group>(entity);
            g->bindVertexSubRecursive(renderer, textureAtlas, vertexIndices);
            
        } else {
            if ((entity->dirtyFlag & DIRTY_VERTEX) == DIRTY_VERTEX) {
                int index = vertexIndices[VERTICES_IDX];
                entity->bindVertices(renderer, &vertexIndices[VERTICES_IDX], &vertexIndices[INDICES_IDX], true);
                renderer->bindVertexSub(index, entity->renderer->verticesNum);
            } else {
                vertexIndices[VERTICES_IDX] += entity->renderer->verticesNum * 2;
            }
            if ((entity->dirtyFlag & DIRTY_COLOR) == DIRTY_COLOR) {
                int index = vertexIndices[VERTEX_COLORS_IDX];
                entity->bindVertexColors(renderer, &vertexIndices[VERTEX_COLORS_IDX]);
                renderer->bindVertexColorsSub(index, entity->renderer->verticesNum);
            } else {
                vertexIndices[VERTEX_COLORS_IDX] += entity->renderer->verticesNum * 4;
            }
            if (this->enableTexture && entity->textures[0]) {
                if ((entity->dirtyFlag & DIRTY_TEX_COORDS) == DIRTY_TEX_COORDS) {
                    std::shared_ptr<TextureAtlasCell> cell = nullptr;
                    if (entity->textures[0]) cell = textureAtlas->getCell(entity->textures[0]);
                    float x = (cell) ? ((float)cell->x / (float)textureAtlas->width) : -1.0f;
                    float y = (cell) ? ((float)cell->y / (float)textureAtlas->height) : -1.0f;
                    float w = (cell) ? ((float)cell->width / (float)textureAtlas->width) : 1.0f;
                    float h = (cell) ? ((float)cell->height / (float)textureAtlas->height) : 1.0f;
                    int index = vertexIndices[VERTEX_TEX_COORDS_IDX];
                    entity->bindVertexTexCoords(renderer, &vertexIndices[VERTEX_TEX_COORDS_IDX], 0, x, y, w, h);
                    renderer->bindVertexTexCoordsSub(index, entity->renderer->verticesNum);
                } else {
                    vertexIndices[VERTEX_TEX_COORDS_IDX] += entity->renderer->verticesNum * 2;
                }
            }
        }
        entity->dirtyFlag = 0;
    }
}

void Group::add(const std::shared_ptr<Entity> &entity) {
    this->drawableContainer->addChild(entity);
    this->dirtyFlag |= DIRTY_ALL;
}

void Group::insertBefore(const std::shared_ptr<Entity> &entity, const std::shared_ptr<Entity> &baseEntity) {
    this->drawableContainer->insertChildBefore(entity, baseEntity);
    this->dirtyFlag |= DIRTY_ALL;
}

void Group::insertAfter(const std::shared_ptr<Entity> &entity, const std::shared_ptr<Entity> &baseEntity) {
    this->drawableContainer->insertChildAfter(entity, baseEntity);
    this->dirtyFlag |= DIRTY_ALL;
}

void Group::remove(const std::shared_ptr<Entity> &entity) {
    this->drawableContainer->removeChild(entity);
    this->dirtyFlag |= DIRTY_ALL;
}

void Group::removeAll() {
    this->drawableContainer->removeAllChildren();
    this->dirtyFlag |= DIRTY_ALL;
}

std::vector<std::shared_ptr<Entity>> Group::getChildEntities() {
    std::vector<std::shared_ptr<Entity>> entities;
    entities.reserve(this->drawableContainer->childDrawables.size());
    for (const auto &drawable : this->drawableContainer->childDrawables) {
        entities.emplace_back(std::static_pointer_cast<Entity>(drawable));
    }
    return entities;
}

std::shared_ptr<Entity> Group::findChildByName(std::string name, bool recursive) {
    for (const auto &drawable : this->drawableContainer->childDrawables) {
        auto entity = std::static_pointer_cast<Entity>(drawable);
        if (entity->getName() == name) return entity;
        if (!recursive) continue;
        
        if (entity->isGroup()) {
            auto g = std::static_pointer_cast<Group>(entity);
            if (auto e = g->findChildByName(name, recursive)) {
                return e;
            }
        }
    }
    return nullptr;
}

std::shared_ptr<Entity> Group::findFirstChildByTag(std::string tag, bool recursive) {
    for (const auto &drawable : this->drawableContainer->childDrawables) {
        auto entity = std::static_pointer_cast<Entity>(drawable);
        if (entity->getTag() == tag) return entity;
        if (!recursive) continue;
        
        if (entity->isGroup()) {
            auto g = std::static_pointer_cast<Group>(entity);
            if (auto e = g->findFirstChildByTag(tag, recursive)) {
                return e;
            }
        }
    }
    return nullptr;
}

std::vector<std::shared_ptr<Entity>> Group::findChildrenByTag(std::string tag, bool recursive) {
    std::vector<std::shared_ptr<Entity>> vec;
    for (const auto &drawable : this->drawableContainer->childDrawables) {
        auto entity = std::static_pointer_cast<Entity>(drawable);
        if (entity->getTag() == tag) {
            vec.emplace_back(entity);
        };
        if (!recursive) continue;
        
        if (auto g = std::dynamic_pointer_cast<Group>(entity)) {
            auto entities = g->findChildrenByTag(tag, recursive);
            if (entities.size() > 0) {
                vec.insert(vec.end(), entities.begin(), entities.end());
            }
        }
    }
    return vec;
}

void Group::addTextureTo(const std::shared_ptr<TextureAtlas> &textureAtlas) {
    for (const auto &drawable : this->drawableContainer->sortedChildDrawables) {
        auto entity = std::static_pointer_cast<Entity>(drawable);
        if (auto g = std::dynamic_pointer_cast<Group>(entity)) {
            g->addTextureTo(textureAtlas);
        } else {
            textureAtlas->addTexture(entity->getTexture());
        }
    }
}

std::shared_ptr<Sprite> Group::createTextureSprite() {
    if (!this->textureAtlas) return nullptr;
    return Sprite::createWithTexture(this->textureAtlas->texture);
}

/*
void Group::multiplyChildEntityMatrix(const std::shared_ptr<Entity> &entity, float *parentMatrix) {
    if (((this->dirtyFlag | entity->dirtyFlag) & DIRTY_VERTEX) == DIRTY_VERTEX) {
        Transform::multiplyMatrix(entity->transform->matrix, parentMatrix, entity->renderer->matrix);
        entity->dirtyFlag |= DIRTY_VERTEX;
    }
    if (((this->dirtyFlag | entity->dirtyFlag) & DIRTY_COLOR) == DIRTY_COLOR) {
        Transform::multiplyColor(entity->transform->matrix, parentMatrix, entity->renderer->matrix);
        entity->dirtyFlag |= DIRTY_COLOR;
    }
}
*/

std::shared_ptr<Group> Group::clone() {
    auto e = this->cloneEntity();
    return std::static_pointer_cast<Group>(e);
}

std::shared_ptr<Entity> Group::cloneEntity() {
    auto group = Group::create(this->enableBatching);
    for (const auto &drawable : this->drawableContainer->childDrawables) {
        auto entity = std::static_pointer_cast<Entity>(drawable);
        auto cloneEntity = entity->cloneEntity();
        group->add(cloneEntity);
    }
    group->copyProperties(std::static_pointer_cast<Entity>(shared_from_this()));
    return group;
}

std::shared_ptr<Dictionary> Group::serialize() {
    auto dict = Entity::serialize();
    dict->put(PROP_KEY_ENTITY_TYPE, Int::create((int)EntityType::Group));
    dict->put(PROP_KEY_ENABLE_BATCHING, Bool::create(this->enableBatching));
    
    auto childEntityDataList = List::create();
    auto childEntities = this->getChildEntities();
    for (auto &entity : childEntities) {
        auto entityData = entity->serialize();
        childEntityDataList->append(entityData);
    }
    dict->put(PROP_KEY_CHILD_ENTITIES, childEntityDataList);
    
    return dict;
}

void Group::deserializeData(const std::shared_ptr<Dictionary> &dict, const std::unordered_map<std::string, std::unordered_map<std::string, std::shared_ptr<Data>>> &params) {
    Entity::deserializeData(dict, params);
    this->enableBatching = this->getPropertyData<Bool>(dict, PROP_KEY_ENABLE_BATCHING, params)->getValue();
    auto childEntityDataList = dict->get<List>(PROP_KEY_CHILD_ENTITIES);
    for (int i = 0; i < childEntityDataList->size(); i++) {
        auto entityData = childEntityDataList->at<Dictionary>(i);
        auto entityType = (EntityType)entityData->get<Int>(PROP_KEY_ENTITY_TYPE)->getValue();
        auto entity = EntityCreator::create(entityType, entityData, params);
        this->add(entity);
    }
}

bool Group::isGroup() {
    return true;
}
