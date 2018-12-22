#include "mog/base/RoundedRectangle.h"
#include "mog/core/Engine.h"
#include "mog/core/Device.h"
#include <math.h>

using namespace mog;

shared_ptr<RoundedRectangle> RoundedRectangle::create(const Size &size, float cornerRadius) {
    auto rectangle = shared_ptr<RoundedRectangle>(new RoundedRectangle());
    rectangle->init(size, cornerRadius);
    return rectangle;
}

shared_ptr<RoundedRectangle> RoundedRectangle::create(float width, float height, float cornerRadius) {
    return RoundedRectangle::create(Size(width, height), cornerRadius);
}

RoundedRectangle::RoundedRectangle() {
}

float RoundedRectangle::getCornerRadius() {
    return this->cornerRadius;
}

void RoundedRectangle::init(const Size &size, float cornerRadius) {
    this->cornerRadius = cornerRadius;
    this->transform->size = size;
    
    float density = Device::getDeviceDensity();
    int texWidth = (int)(cornerRadius * density + 0.5f);
    int texHeight = texWidth;
    unsigned char *data = (unsigned char *)mogmalloc(sizeof(char) * texWidth * texHeight * 4);
    for (int y = 0; y < texHeight; y++) {
        for (int x = 0; x < texWidth; x++) {
            float a;
            if (x == 0 || y == 0) {
                a = 1.0f;
            } else {
            }
            
            int _x = x - 1;
            int _y = y - 1;
            if (_x < 0) _x = 0;
            if (_y < 0) _y = 0;
            
            float l = Point::length(Point(_x, _y));
            a = (cornerRadius * density) - l;

            if (a > 1.0f) a = 1.0f;
            if (a < 0) a = 0;
            data[(_y * texWidth + _x) * 4 + 0] = 255;
            data[(_y * texWidth + _x) * 4 + 1] = 255;
            data[(_y * texWidth + _x) * 4 + 2] = 255;
            data[(_y * texWidth + _x) * 4 + 3] = (unsigned char)(a * 255.0f + 0.5f);
        }
    }
    
    this->texture = Texture2D::createWithRGBA(data, texWidth, texHeight);
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

void RoundedRectangle::bindVertexTexCoords(const std::shared_ptr<Renderer> &renderer, int *idx, float x, float y, float w, float h) {
    float p = 1.0f / (this->cornerRadius + 1.0f);

    renderer->vertexTexCoords[(*idx)++] = x + w;        renderer->vertexTexCoords[(*idx)++] = y + h;
    renderer->vertexTexCoords[(*idx)++] = x + p * w;    renderer->vertexTexCoords[(*idx)++] = y + h;
    renderer->vertexTexCoords[(*idx)++] = x;            renderer->vertexTexCoords[(*idx)++] = y + h;
    renderer->vertexTexCoords[(*idx)++] = x + p * w;    renderer->vertexTexCoords[(*idx)++] = y + h;
    renderer->vertexTexCoords[(*idx)++] = x + w;        renderer->vertexTexCoords[(*idx)++] = y + h;

    renderer->vertexTexCoords[(*idx)++] = x + w;        renderer->vertexTexCoords[(*idx)++] = y + p * h;
    renderer->vertexTexCoords[(*idx)++] = x + p * w;    renderer->vertexTexCoords[(*idx)++] = y + p * h;
    renderer->vertexTexCoords[(*idx)++] = x;            renderer->vertexTexCoords[(*idx)++] = y + p * h;
    renderer->vertexTexCoords[(*idx)++] = x + p * w;    renderer->vertexTexCoords[(*idx)++] = y + p * h;
    renderer->vertexTexCoords[(*idx)++] = x + w;        renderer->vertexTexCoords[(*idx)++] = y + p * h;

    renderer->vertexTexCoords[(*idx)++] = x + w;        renderer->vertexTexCoords[(*idx)++] = y;
    renderer->vertexTexCoords[(*idx)++] = x + p * w;    renderer->vertexTexCoords[(*idx)++] = y;
    renderer->vertexTexCoords[(*idx)++] = x;            renderer->vertexTexCoords[(*idx)++] = y;
    renderer->vertexTexCoords[(*idx)++] = x + p * w;    renderer->vertexTexCoords[(*idx)++] = y;
    renderer->vertexTexCoords[(*idx)++] = x + w;        renderer->vertexTexCoords[(*idx)++] = y;

    renderer->vertexTexCoords[(*idx)++] = x + w;        renderer->vertexTexCoords[(*idx)++] = y + p * h;
    renderer->vertexTexCoords[(*idx)++] = x + p * w;    renderer->vertexTexCoords[(*idx)++] = y + p * h;
    renderer->vertexTexCoords[(*idx)++] = x;            renderer->vertexTexCoords[(*idx)++] = y + p * h;
    renderer->vertexTexCoords[(*idx)++] = x + p * w;    renderer->vertexTexCoords[(*idx)++] = y + p * h;
    renderer->vertexTexCoords[(*idx)++] = x + w;        renderer->vertexTexCoords[(*idx)++] = y + p * h;

    renderer->vertexTexCoords[(*idx)++] = x + w;        renderer->vertexTexCoords[(*idx)++] = y + h;
    renderer->vertexTexCoords[(*idx)++] = x + p * w;    renderer->vertexTexCoords[(*idx)++] = y + h;
    renderer->vertexTexCoords[(*idx)++] = x;            renderer->vertexTexCoords[(*idx)++] = y + h;
    renderer->vertexTexCoords[(*idx)++] = x + p * w;    renderer->vertexTexCoords[(*idx)++] = y + h;
    renderer->vertexTexCoords[(*idx)++] = x + w;        renderer->vertexTexCoords[(*idx)++] = y + h;
}
