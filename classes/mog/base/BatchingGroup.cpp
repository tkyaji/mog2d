#include <set>
#include <vector>
#include "mog/base/BatchingGroup.h"
#include "mog/core/TextureAtlas.h"

using namespace mog;

shared_ptr<BatchingGroup> BatchingGroup::create() {
    return shared_ptr<BatchingGroup>(new BatchingGroup());
}

BatchingGroup::BatchingGroup() {
}

void BatchingGroup::drawFrame(float delta) {
    if (!this->visible) return;

    auto childEntitiesToDraw = this->getSortedChildEntitiesToDraw();

    if ((this->reRenderFlag & RERENDER_ALL) == RERENDER_ALL) {
        this->bindVertex();
        this->reRenderFlag = 0;
    }
    if (this->reRenderFlag > 0) {
        this->bindVertexSub();
        this->reRenderFlag = 0;
    }
    
    Group::updateMatrix();
    
    this->renderer->drawFrame(this->transform);
}

void BatchingGroup::bindVertex() {
    int indiciesNum = 0;
    this->getIndiciesNum(&indiciesNum);
    int verticesNum = 0;
    this->getVerticesNum(&verticesNum);
    
    auto indices = new short[indiciesNum];
    int idx = 0;
    this->bindIndices(indices, &idx, 0);
    auto vertices = new float[verticesNum * 2];
    idx = 0;
    
    this->bindVertices(vertices, &idx, false);
    this->renderer->bindVertex(vertices, verticesNum * 2, indices, indiciesNum, true);
    
    safe_delete_arr(indices);
    safe_delete_arr(vertices);
    
    vector<shared_ptr<Texture2D>> textures;
    this->textureAtlas = make_shared<TextureAtlas>();
    this->addTextureTo(this->textureAtlas);
    this->texture = this->textureAtlas->createTexture();

    float *vertexTexCoords = new float[verticesNum * 2];
    idx = 0;
    this->bindVertexTexCoords(this->textureAtlas, vertexTexCoords, &idx, 0, 0, 1.0f, 1.0f);
    this->textureAtlas->bindTexture();
    this->renderer->bindTextureVertex(this->texture->textureId, vertexTexCoords, verticesNum * 2, true);

    safe_delete_arr(vertexTexCoords);

    float *vertexColors = new float[verticesNum * 4];
    idx = 0;
    this->bindVertexColors(vertexColors, &idx, this->getParentColor());
    this->renderer->bindColorsVertex(vertexColors, verticesNum * 4, true);
    
    safe_delete_arr(vertexColors);
}

void BatchingGroup::bindVertices(float *vertices, int *idx, bool bakeTransform) {
    if (bakeTransform) {
        Group::bindVertices(vertices, idx, bakeTransform);
    } else {
        this->renderer->getMatrix(this->tmpMatrix);
        this->renderer->setMatrix(Renderer::identityMatrix);
        Group::bindVertices(vertices, idx, bakeTransform);
        this->renderer->setMatrix(this->tmpMatrix);
    }
}

void BatchingGroup::bindVertexSub() {
    int verticesNum = 0;
    this->getVerticesNum(&verticesNum);
    
    if ((this->reRenderFlag & RERENDER_VERTEX) == RERENDER_VERTEX) {
        float *vertices = new float[verticesNum * 2];
        int idx = 0;
        this->bindVertices(vertices, &idx, false);
        this->renderer->bindVertexSub(vertices, verticesNum * 2);
        safe_delete_arr(vertices);
        this->reRenderFlag &= ~RERENDER_VERTEX;
    }
    
    if ((this->reRenderFlag & RERENDER_COLOR) == RERENDER_COLOR) {
        float *vertexColors = new float[verticesNum * 4];
        int idx = 0;
        this->bindVertexColors(vertexColors, &idx, this->getParentColor());
        this->renderer->bindColorsVertexSub(vertexColors, verticesNum * 4);
        this->reRenderFlag &= ~RERENDER_COLOR;
        safe_delete_arr(vertexColors);
        this->reRenderFlag &= ~RERENDER_COLOR;
    }
    
    if ((this->reRenderFlag & RERENDER_TEX_COORDS) == RERENDER_TEX_COORDS) {
        float *vertexTexCoords = new float[verticesNum * 2];
        int idx = 0;
        this->bindVertexTexCoords(this->textureAtlas, vertexTexCoords, &idx, 0, 0, 1.0f, 1.0f);
        this->renderer->bindTextureVertex(this->texture->textureId, vertexTexCoords, verticesNum * 2);
        safe_delete_arr(vertexTexCoords);
        this->reRenderFlag &= ~RERENDER_TEX_COORDS;
    }
}

Color BatchingGroup::getParentColor() {
    Color parentColor = Color::white;
    shared_ptr<Entity> g = static_pointer_cast<Entity>(shared_from_this());
    while ((g = g->getGroup())) {
        parentColor = parentColor * g->getColor();
    }
    return parentColor;
}

shared_ptr<Sprite> BatchingGroup::createTextureSprite() {
    if (!this->textureAtlas) return nullptr;
    return Sprite::createWithTexture(this->textureAtlas->texture);
}

shared_ptr<BatchingGroup> BatchingGroup::clone() {
    auto entity = this->cloneEntity();
    return static_pointer_cast<BatchingGroup>(entity);
}

shared_ptr<Entity> BatchingGroup::cloneEntity() {
    auto group = shared_ptr<BatchingGroup>(new BatchingGroup());
    group->copyFrom(shared_from_this());
    return group;
}
