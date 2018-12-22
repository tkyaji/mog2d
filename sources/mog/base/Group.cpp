#include "mog/Constants.h"
#include "mog/base/Drawable.h"
#include "mog/base/Group.h"
#include "mog/base/Sprite.h"
#include "mog/core/Engine.h"
#include <algorithm>

#define VERTICES_IDX 0
#define INDICES_IDX 1
#define VERTEX_COLORS_IDX 2
#define VERTEX_TEX_COORDS_IDX 3

using namespace mog;

shared_ptr<Group> Group::create(bool enableBatching) {
    auto group = shared_ptr<Group>(new Group());
    group->setEnableBatching(enableBatching);
    return group;
}

Group::Group() {
    this->drawableGroup = make_shared<DrawableGroup>();
    this->drawableGroup->addChildListener = [this](const std::shared_ptr<Drawable> &drawable) {
        auto e = static_pointer_cast<Entity>(drawable);
        e->group = static_pointer_cast<Group>(shared_from_this());
    };
    this->drawableGroup->removeChildListener = [this](const std::shared_ptr<Drawable> &drawable) {
        auto e = static_pointer_cast<Entity>(drawable);
        e->group.reset();
    };
}

void Group::setEnableBatching(bool enableBatching) {
    this->enableBatching = enableBatching;
    this->reRenderFlag = RERENDER_ALL;
}

bool Group::isEnableBatching() {
    return this->enableBatching;
}

void Group::updateFrame(const shared_ptr<Engine> &engine, float delta, float *parentMatrix, unsigned char parentReRenderFlag) {
    this->drawableGroup->sortChildDrawablesToDraw();
    Entity::updateFrame(engine, delta, parentMatrix, parentReRenderFlag);
    int verticesNum = 0;
    int indicesNum = 0;
    for (const auto &drawable : this->drawableGroup->sortedChildDrawables) {
        auto entity = static_pointer_cast<Entity>(drawable);
        if (this->enableBatching) {
            parentReRenderFlag |= (parentReRenderFlag | this->reRenderFlag);
        } else {
            entity->reRenderFlag |= (this->reRenderFlag & (RERENDER_VERTEX | RERENDER_COLOR));
        }
        entity->updateFrame(engine, delta, this->matrix, parentReRenderFlag);

        verticesNum += entity->renderer->verticesNum;
        if (indicesNum > 0) indicesNum += 2;
        indicesNum += entity->renderer->indicesNum;

        if (auto g = dynamic_cast<Group *>(entity.get())) {
            if (g->enableTexture) this->enableTexture = true;
            this->reRenderFlagChildren |= (entity->reRenderFlag | g->reRenderFlagChildren);
        } else {
            if (entity->texture) this->enableTexture = true;
            this->reRenderFlagChildren |= entity->reRenderFlag;
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

void Group::drawFrame(float delta) {
    if (!this->active) return;
    
    if (this->enableBatching) {
        if ((this->reRenderFlag & RERENDER_VERTEX) == RERENDER_VERTEX) {
            this->renderer->setUniformMatrix(this->renderer->matrix);
        }
        if ((this->reRenderFlag & RERENDER_COLOR) == RERENDER_COLOR) {
            this->renderer->setUniformColor(this->renderer->matrix[16], this->renderer->matrix[17], this->renderer->matrix[18], this->renderer->matrix[19]);
        }
        if ((this->reRenderFlag & RERENDER_ALL) == RERENDER_ALL) {
            this->bindVertex();
            
        } else if (this->reRenderFlagChildren > 0) {
            if (this->enableTexture && (this->reRenderFlagChildren & RERENDER_TEXTURE) == RERENDER_TEXTURE) {
                this->bindVertex();
            } else {
                this->bindVertexSub();
            }
            this->reRenderFlagChildren = 0;
        }
        
        this->renderer->drawFrame();

    } else {
        for (const auto &drawable : this->drawableGroup->sortedChildDrawables) {
            auto entity = static_pointer_cast<Entity>(drawable);
            if (((this->reRenderFlag | entity->reRenderFlag) & RERENDER_VERTEX) == RERENDER_VERTEX) {
                Transform::multiplyMatrix(entity->transform->matrix, this->renderer->matrix, entity->renderer->matrix);
            }
            if (((this->reRenderFlag | entity->reRenderFlag) & RERENDER_COLOR) == RERENDER_COLOR) {
                Transform::multiplyColor(entity->transform->matrix, this->renderer->matrix, entity->renderer->matrix);
            }
            entity->drawFrame(delta);
        }
    }
    if ((this->reRenderFlag & RERENDER_VERTEX) == RERENDER_VERTEX) {
        this->collider = nullptr;
    }
    this->reRenderFlag = 0;
}

void Group::bindVertex() {
    if (!this->enableBatching) return;
    
    int vertexIndices[4] = {0, 0, 0, 0};
    
    if (this->enableTexture) {
        this->textureAtlas = make_shared<TextureAtlas>();
        this->addTextureTo(this->textureAtlas);
        this->texture = this->textureAtlas->createTexture();
        this->textureAtlas->bindTexture();
    }

    this->bindVertexRecursive(this->renderer, this->textureAtlas, vertexIndices, Renderer::identityMatrix);
    this->renderer->bindVertex(true);
    this->renderer->bindVertexColors(true);
    if (this->enableTexture) {
        this->renderer->bindVertexTexCoords(this->texture->textureId);
    }
}

void Group::bindVertexRecursive(const std::shared_ptr<Renderer> &renderer, std::shared_ptr<TextureAtlas> &textureAtlas, int *vertexIndices, float *parentMatrix) {
    for (const auto &drawable : this->drawableGroup->sortedChildDrawables) {
        auto entity = static_pointer_cast<Entity>(drawable);
        if ((this->reRenderFlagChildren & RERENDER_VERTEX) == RERENDER_VERTEX) {
            Transform::multiplyMatrix(entity->transform->matrix, parentMatrix, entity->renderer->matrix);
        }
        if ((this->reRenderFlagChildren & RERENDER_COLOR) == RERENDER_COLOR) {
            Transform::multiplyColor(entity->transform->matrix, parentMatrix, entity->renderer->matrix);
        }

        if (auto g = dynamic_cast<Group *>(entity.get())) {
            g->bindVertexRecursive(renderer, textureAtlas, vertexIndices, entity->renderer->matrix);
        } else {
            entity->bindVertices(renderer, &vertexIndices[VERTICES_IDX], &vertexIndices[INDICES_IDX], true);
            entity->bindVertexColors(renderer, &vertexIndices[VERTEX_COLORS_IDX]);
            std::shared_ptr<TextureAtlasCell> cell = nullptr;
            if (this->enableTexture) {
                if (entity->texture) cell = textureAtlas->getCell(entity->texture);
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
                entity->bindVertexTexCoords(renderer, &vertexIndices[VERTEX_TEX_COORDS_IDX], x, y, w, h);
            }
        }
        entity->reRenderFlag = 0;
    }
}

void Group::bindVertexSub() {
    int vertexIndices[4] = {0, 0, 0, 0};
    this->bindVertexSubRecursive(this->renderer, this->textureAtlas, vertexIndices, Renderer::identityMatrix);
}

void Group::bindVertexSubRecursive(const std::shared_ptr<Renderer> &renderer, std::shared_ptr<TextureAtlas> &textureAtlas, int *vertexIndices, float *parentMatrix) {
    for (const auto &drawable : this->drawableGroup->sortedChildDrawables) {
        auto entity = static_pointer_cast<Entity>(drawable);
        Transform::multiplyMatrix(entity->transform->matrix, parentMatrix, entity->renderer->matrix);
        Transform::multiplyColor(entity->transform->matrix, parentMatrix, entity->renderer->matrix);
        
        if (auto g = dynamic_cast<Group *>(entity.get())) {
            g->bindVertexSubRecursive(renderer, textureAtlas, vertexIndices, entity->renderer->matrix);
        } else {
            if ((entity->reRenderFlag & RERENDER_VERTEX) == RERENDER_VERTEX) {
                int index = vertexIndices[VERTICES_IDX];
                entity->bindVertices(renderer, &vertexIndices[VERTICES_IDX], nullptr, true);
                renderer->bindVertexSub(index, entity->renderer->verticesNum);
            } else {
                vertexIndices[VERTICES_IDX] += entity->renderer->verticesNum * 2;
            }
            if ((entity->reRenderFlag & RERENDER_COLOR) == RERENDER_COLOR) {
                int index = vertexIndices[VERTEX_COLORS_IDX];
                entity->bindVertexColors(renderer, &vertexIndices[VERTEX_COLORS_IDX]);
                renderer->bindVertexColorsSub(index, entity->renderer->verticesNum);
            } else {
                vertexIndices[VERTEX_COLORS_IDX] += entity->renderer->verticesNum * 4;
            }
            if (this->enableTexture && entity->texture) {
                if ((entity->reRenderFlag & RERENDER_TEX_COORDS) == RERENDER_TEX_COORDS) {
//                    entity->bindVertexColors(renderer, &vertexIndices[VERTEX_COLORS_IDX]);
                    std::shared_ptr<TextureAtlasCell> cell = nullptr;
                    if (this->enableTexture) {
                        if (entity->texture) cell = textureAtlas->getCell(entity->texture);
                        float x = (cell) ? ((float)cell->x / (float)textureAtlas->width) : -1.0f;
                        float y = (cell) ? ((float)cell->y / (float)textureAtlas->height) : -1.0f;
                        float w = (cell) ? ((float)cell->width / (float)textureAtlas->width) : 1.0f;
                        float h = (cell) ? ((float)cell->height / (float)textureAtlas->height) : 1.0f;
                        int index = vertexIndices[VERTEX_TEX_COORDS_IDX];
                        entity->bindVertexTexCoords(renderer, &vertexIndices[VERTEX_TEX_COORDS_IDX], x, y, w, h);
                        renderer->bindVertexTexCoordsSub(index, entity->renderer->verticesNum);
                    }
                } else {
                    vertexIndices[VERTEX_TEX_COORDS_IDX] += entity->renderer->verticesNum * 2;
                }
            }
        }
        entity->reRenderFlag = 0;
    }
}

void Group::add(const shared_ptr<Entity> &entity) {
    this->drawableGroup->addChild(entity);
    this->reRenderFlag |= RERENDER_ALL;
}

void Group::remove(const shared_ptr<Entity> &entity) {
    this->drawableGroup->removeChild(entity);
    this->reRenderFlag |= RERENDER_ALL;
}

void Group::removeAll() {
    this->drawableGroup->removeAllChildren();
    this->reRenderFlag |= RERENDER_ALL;
}

vector<shared_ptr<Entity>> Group::getChildEntities() {
    vector<shared_ptr<Entity>> entities;
    entities.reserve(this->drawableGroup->childDrawables.size());
    for (const auto &drawable : this->drawableGroup->childDrawables) {
        entities.emplace_back(static_pointer_cast<Entity>(drawable));
    }
    return entities;
}

shared_ptr<Entity> Group::findChildByName(string name, bool recursive) {
    for (const auto &drawable : this->drawableGroup->childDrawables) {
        auto entity = static_pointer_cast<Entity>(drawable);
        if (entity->getName() == name) return entity;
        if (!recursive) break;
        if (auto g = dynamic_pointer_cast<Group>(entity)) {
            if (auto e = g->findChildByName(name, recursive)) {
                return e;
            }
        }
    }
    return nullptr;
}

shared_ptr<Entity> Group::findFirstChildByTag(string tag, bool recursive) {
    for (const auto &drawable : this->drawableGroup->childDrawables) {
        auto entity = static_pointer_cast<Entity>(drawable);
        if (entity->getTag() == tag) return entity;
        if (!recursive) break;
        if (auto g = dynamic_pointer_cast<Group>(entity)) {
            if (auto e = g->findFirstChildByTag(tag, recursive)) {
                return e;
            }
        }
    }
    return nullptr;
}

vector<shared_ptr<Entity>> Group::findChildrenByTag(string tag, bool recursive) {
    vector<shared_ptr<Entity>> vec;
    for (const auto &drawable : this->drawableGroup->childDrawables) {
        auto entity = static_pointer_cast<Entity>(drawable);
        if (entity->getTag() == tag) {
            vec.emplace_back(entity);
        };
        if (!recursive) break;
        if (auto g = dynamic_pointer_cast<Group>(entity)) {
            auto entities = g->findChildrenByTag(tag, recursive);
            if (entities.size() > 0) {
                vec.insert(vec.end(), entities.begin(), entities.end());
            }
        }
    }
    return vec;
}

void Group::addTextureTo(const shared_ptr<TextureAtlas> &textureAtlas) {
    for (const auto &drawable : this->drawableGroup->sortedChildDrawables) {
        auto entity = static_pointer_cast<Entity>(drawable);
        if (auto g = dynamic_pointer_cast<Group>(entity)) {
            g->addTextureTo(textureAtlas);
        } else {
            textureAtlas->addTexture(entity->getTexture());
        }
    }
}

Color Group::getParentColor() {
    Color parentColor = Color::white;
    shared_ptr<Entity> g = static_pointer_cast<Entity>(shared_from_this());
    while ((g = g->getGroup())) {
        parentColor = parentColor * g->getColor();
    }
    return parentColor;
}

shared_ptr<Sprite> Group::createTextureSprite() {
    if (!this->textureAtlas) return nullptr;
    return Sprite::createWithTexture(this->textureAtlas->texture);
}
