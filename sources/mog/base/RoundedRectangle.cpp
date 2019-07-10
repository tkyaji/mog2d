#include "mog/base/RoundedRectangle.h"
#include "mog/core/Engine.h"
#include "mog/core/Screen.h"
#include <math.h>

using namespace mog;

std::shared_ptr<RoundedRectangle> RoundedRectangle::create(const Size &size, float cornerRadius, unsigned char cornerFlag) {
    auto roundedRectangle = std::shared_ptr<RoundedRectangle>(new RoundedRectangle());
    roundedRectangle->cornerRadius = cornerRadius;
    roundedRectangle->cornerFlag = cornerFlag;
    roundedRectangle->transform->size = size;
    roundedRectangle->init();
    return roundedRectangle;
}

std::shared_ptr<RoundedRectangle> RoundedRectangle::create(float width, float height, float cornerRadius, unsigned char cornerFlag) {
    return RoundedRectangle::create(Size(width, height), cornerRadius, cornerFlag);
}

float RoundedRectangle::getCornerRadius() {
    return this->cornerRadius;
}

void RoundedRectangle::init() {
    float scale = Screen::getScreenScale();
    int texWidth = (int)(this->cornerRadius * scale + 0.5f) + 2.0f;
    int texHeight = texWidth;
    unsigned char *data = (unsigned char *)mogmalloc(sizeof(char) * texWidth * texHeight * 4);
    for (int y = 0; y < texHeight; y++) {
        for (int x = 0; x < texWidth; x++) {
            float a;
            if (x < 2 || y < 2) {
                a = 1.0f;
                
            } else {
                int _x = x - 2;
                int _y = y - 2;
                if (_x < 0) _x = 0;
                if (_y < 0) _y = 0;
                
                float l = Point::length(Point(_x, _y));
                a = (this->cornerRadius * scale) - l;
            }
            
            if (a > 1.0f) a = 1.0f;
            if (a < 0) a = 0;
            data[(y * texWidth + x) * 4 + 0] = 255;
            data[(y * texWidth + x) * 4 + 1] = 255;
            data[(y * texWidth + x) * 4 + 2] = 255;
            data[(y * texWidth + x) * 4 + 3] = (unsigned char)(a * 255.0f + 0.5f);
        }
    }
    
    this->textures[0] = Texture2D::createWithRGBA(data, texWidth, texHeight, Screen::getDensity());
    this->rect = Rect(Point::zero, this->transform->size);
    this->initRendererVertices(25, 40);
}

void RoundedRectangle::bindVertices(const std::shared_ptr<Renderer> &renderer, int *verticesIdx, int *indicesIdx, bool bakeTransform) {
    float xx[5] = {
        0,
        this->cornerRadius,
        this->transform->size.width * 0.5f,
        this->transform->size.width - this->cornerRadius,
        this->transform->size.width,
    };
    float yy[5] = {
        0,
        this->cornerRadius,
        this->transform->size.height * 0.5f,
        this->transform->size.height - this->cornerRadius,
        this->transform->size.height,
    };
    
    int startN = *verticesIdx / 2;
    auto offset = Point(this->renderer->matrix[12], this->renderer->matrix[13]);
    for (int yi = 0; yi < 5; yi++) {
        for (int xi = 0; xi < 5; xi++) {
            float x = xx[xi];
            float y = yy[yi];
            if (!this->active) {
                x = 0;  y = 0;
            } else if (bakeTransform) {
                x += offset.x;
                y += offset.y;
            }
            renderer->vertices[(*verticesIdx)++] = x;  renderer->vertices[(*verticesIdx)++] = y;
        }
    }
    
    if (indicesIdx) {
        int startI = *indicesIdx;
        if (startI > 0) {
            renderer->indices[*indicesIdx] = renderer->indices[(*indicesIdx) - 1];
            (*indicesIdx)++;
            renderer->indices[(*indicesIdx)++] = startN;
        }
        int ii[40] = {
            0, 5, 1, 6, 2, 7, 3, 8, 4, 9,
            9, 14, 8, 13, 7, 12, 6, 11, 5, 10,
            10, 15, 11, 16, 12, 17, 13, 18, 14, 19,
            19, 24, 18, 23, 17, 22, 16, 21, 15, 20,
        };
        for (int i = 0; i < 40; i++) {
            renderer->indices[(*indicesIdx)++] = ii[i] + startN;
        }
    }
}

void RoundedRectangle::bindVertexTexCoords(const std::shared_ptr<Renderer> &renderer, int *idx, int texIdx, float x, float y, float w, float h) {
    float density = Screen::getDensity();
    float p1 = 1.0f / (this->cornerRadius * density + 2.0f);
    float p2 = 2.0f / (this->cornerRadius * density + 2.0f);
    
    float x0a = x + w;
    float x0b = x + p2 * w;
    float x1a = x + p2 * w;
    float x1b = x + p1 * w;
    float x2 = x;
    
    float y0 = y + h;
    float y1 = y + p2 * h;
    float y2 = y;
    
    float x0 = x0a;
    float x1 = x1a;
    float x3 = x1a;
    float x4 = x0a;
    if ((this->cornerFlag & CORNER_TOP_LEFT) == 0) {
        x0 = x0b;
        x1 = x1b;
    }
    if ((this->cornerFlag & CORNER_TOP_RIGHT) == 0) {
        x3 = x1b;
        x4 = x0b;
    }
    renderer->vertexTexCoords[texIdx][(*idx)++] = x0;   renderer->vertexTexCoords[texIdx][(*idx)++] = y0;
    renderer->vertexTexCoords[texIdx][(*idx)++] = x1;   renderer->vertexTexCoords[texIdx][(*idx)++] = y0;
    renderer->vertexTexCoords[texIdx][(*idx)++] = x2;   renderer->vertexTexCoords[texIdx][(*idx)++] = y0;
    renderer->vertexTexCoords[texIdx][(*idx)++] = x3;   renderer->vertexTexCoords[texIdx][(*idx)++] = y0;
    renderer->vertexTexCoords[texIdx][(*idx)++] = x4;   renderer->vertexTexCoords[texIdx][(*idx)++] = y0;
    
    renderer->vertexTexCoords[texIdx][(*idx)++] = x0;   renderer->vertexTexCoords[texIdx][(*idx)++] = y1;
    renderer->vertexTexCoords[texIdx][(*idx)++] = x1;   renderer->vertexTexCoords[texIdx][(*idx)++] = y1;
    renderer->vertexTexCoords[texIdx][(*idx)++] = x2;   renderer->vertexTexCoords[texIdx][(*idx)++] = y1;
    renderer->vertexTexCoords[texIdx][(*idx)++] = x3;   renderer->vertexTexCoords[texIdx][(*idx)++] = y1;
    renderer->vertexTexCoords[texIdx][(*idx)++] = x4;   renderer->vertexTexCoords[texIdx][(*idx)++] = y1;
    
    x0 = x0a;
    x1 = x1a;
    x3 = x1a;
    x4 = x0a;
    renderer->vertexTexCoords[texIdx][(*idx)++] = x0;   renderer->vertexTexCoords[texIdx][(*idx)++] = y2;
    renderer->vertexTexCoords[texIdx][(*idx)++] = x1;   renderer->vertexTexCoords[texIdx][(*idx)++] = y2;
    renderer->vertexTexCoords[texIdx][(*idx)++] = x2;   renderer->vertexTexCoords[texIdx][(*idx)++] = y2;
    renderer->vertexTexCoords[texIdx][(*idx)++] = x3;   renderer->vertexTexCoords[texIdx][(*idx)++] = y2;
    renderer->vertexTexCoords[texIdx][(*idx)++] = x4;   renderer->vertexTexCoords[texIdx][(*idx)++] = y2;
    
    if ((this->cornerFlag & CORNER_BOTTOM_LEFT) == 0) {
        x0 = x0b;
        x1 = x1b;
    }
    if ((this->cornerFlag & CORNER_BOTTOM_RIGHT) == 0) {
        x3 = x1b;
        x4 = x0b;
    }
    renderer->vertexTexCoords[texIdx][(*idx)++] = x0;   renderer->vertexTexCoords[texIdx][(*idx)++] = y1;
    renderer->vertexTexCoords[texIdx][(*idx)++] = x1;   renderer->vertexTexCoords[texIdx][(*idx)++] = y1;
    renderer->vertexTexCoords[texIdx][(*idx)++] = x2;   renderer->vertexTexCoords[texIdx][(*idx)++] = y1;
    renderer->vertexTexCoords[texIdx][(*idx)++] = x3;   renderer->vertexTexCoords[texIdx][(*idx)++] = y1;
    renderer->vertexTexCoords[texIdx][(*idx)++] = x4;   renderer->vertexTexCoords[texIdx][(*idx)++] = y1;
    
    renderer->vertexTexCoords[texIdx][(*idx)++] = x0;   renderer->vertexTexCoords[texIdx][(*idx)++] = y0;
    renderer->vertexTexCoords[texIdx][(*idx)++] = x1;   renderer->vertexTexCoords[texIdx][(*idx)++] = y0;
    renderer->vertexTexCoords[texIdx][(*idx)++] = x2;   renderer->vertexTexCoords[texIdx][(*idx)++] = y0;
    renderer->vertexTexCoords[texIdx][(*idx)++] = x3;   renderer->vertexTexCoords[texIdx][(*idx)++] = y0;
    renderer->vertexTexCoords[texIdx][(*idx)++] = x4;   renderer->vertexTexCoords[texIdx][(*idx)++] = y0;
}

std::shared_ptr<RoundedRectangle> RoundedRectangle::clone() {
    auto e = this->cloneEntity();
    return std::static_pointer_cast<RoundedRectangle>(e);
}

std::shared_ptr<Entity> RoundedRectangle::cloneEntity() {
    auto rectangle = RoundedRectangle::create(this->getSize(), this->getCornerRadius());
    rectangle->copyProperties(std::static_pointer_cast<Entity>(shared_from_this()));
    return rectangle;
}

std::shared_ptr<Dictionary> RoundedRectangle::serialize() {
    auto dict = Entity::serialize();
    dict->put("cornerRadius", Float::create(this->cornerRadius));
    dict->put("cornerFlag", Int::create(this->cornerFlag));
    return dict;
}

void RoundedRectangle::deserializeData(const std::shared_ptr<Dictionary> &dict) {
    this->cornerRadius = dict->get<Float>("cornerRadius")->getValue();
    this->cornerFlag = (unsigned char)dict->get<Float>("cornerFlag")->getValue();
}
