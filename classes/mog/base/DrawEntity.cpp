#include "mog/base/DrawEntity.h"
#include "mog/core/Engine.h"

using namespace mog;

DrawEntity::DrawEntity() {
}

DrawEntity::~DrawEntity() {
}

void DrawEntity::bindVertex() {
    Entity::bindVertex();

    if ((this->reRenderFlag & (RERENDER_TEXTURE | RERENDER_TEX_COORDS)) == 0) {
        return;
    }

    int verticesNum = 0;
    this->getVerticesNum(&verticesNum);
    float *vertexTexCoords = new float[verticesNum * 2];
    int idx = 0;
    this->bindVertexTexCoords(vertexTexCoords, &idx, 0, 0, 1.0f, 1.0f);
    if ((this->reRenderFlag & RERENDER_TEXTURE) == RERENDER_TEXTURE) {
        this->texture->bindTexture();
    }
    this->renderer->bindTextureVertex(this->texture->textureId, vertexTexCoords, verticesNum * 2, this->dynamicDraw);
    
    this->reRenderFlag &= ~(RERENDER_TEXTURE | RERENDER_TEX_COORDS);
    
    safe_delete_arr(vertexTexCoords);
}

void DrawEntity::bindVertexTexCoords(float *vertexTexCoords, int *idx, float x, float y, float w, float h) {
    if (!this->visible) return;
    if (this->texture->isFlip) {
        vertexTexCoords[(*idx)++] = x;      vertexTexCoords[(*idx)++] = y + h;
        vertexTexCoords[(*idx)++] = x;      vertexTexCoords[(*idx)++] = y;
        vertexTexCoords[(*idx)++] = x + w;  vertexTexCoords[(*idx)++] = y + h;
        vertexTexCoords[(*idx)++] = x + w;  vertexTexCoords[(*idx)++] = y;
        
    } else {
        vertexTexCoords[(*idx)++] = x;      vertexTexCoords[(*idx)++] = y;
        vertexTexCoords[(*idx)++] = x;      vertexTexCoords[(*idx)++] = y + h;
        vertexTexCoords[(*idx)++] = x + w;  vertexTexCoords[(*idx)++] = y;
        vertexTexCoords[(*idx)++] = x + w;  vertexTexCoords[(*idx)++] = y + h;
    }
}

void DrawEntity::bindVertexColors(float *vertexColors, int *idx, const Color &parentColor) {
    if (!this->visible) return;
    int verticesNum = 0;
    this->getVerticesNum(&verticesNum);
    Color color = Color(this->transform->color.r * parentColor.r,
                        this->transform->color.g * parentColor.g,
                        this->transform->color.b * parentColor.b,
                        this->transform->color.a * parentColor.a);
    for (int i = 0; i < verticesNum; i++) {
        vertexColors[(*idx)++] = color.r;
        vertexColors[(*idx)++] = color.g;
        vertexColors[(*idx)++] = color.b;
        vertexColors[(*idx)++] = color.a;
    }
}

