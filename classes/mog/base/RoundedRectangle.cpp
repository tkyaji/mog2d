#include "mog/base/RoundedRectangle.h"
#include "mog/core/Engine.h"
#include "mog/core/Device.h"
#include <math.h>

#define SCALABLE_RECT_SIZE 3

using namespace mog;

unordered_map<float, weak_ptr<Texture2D>> RoundedRectangle::cachedTexture2d;

shared_ptr<RoundedRectangle> RoundedRectangle::create(const Size &size, float cornerRadius) {
    auto rectangle = shared_ptr<RoundedRectangle>(new RoundedRectangle());
    rectangle->init(size, false, cornerRadius);
    return rectangle;
}

shared_ptr<RoundedRectangle> RoundedRectangle::create(const Size &size, bool isRatio, float cornerRadius) {
    auto rectangle = shared_ptr<RoundedRectangle>(new RoundedRectangle());
    rectangle->init(size, isRatio, cornerRadius);
    return rectangle;
}

RoundedRectangle::RoundedRectangle() {
}

RoundedRectangle::~RoundedRectangle() {
    if (RoundedRectangle::cachedTexture2d.count(this->cornerRadius)) {
        RoundedRectangle::cachedTexture2d.erase(this->cornerRadius);
    }
}

float RoundedRectangle::getCornerRadius() {
    return this->cornerRadius;
}

void RoundedRectangle::init(const Size &size, bool isRatio, float cornerRadius) {
    this->cornerRadius = cornerRadius;
    this->setSize(size, isRatio);

    if (RoundedRectangle::cachedTexture2d.count(cornerRadius) > 0) {
        this->texture = RoundedRectangle::cachedTexture2d[cornerRadius].lock();
    }
    if (this->texture == nullptr) {
        this->texture = this->createTexture(cornerRadius);
        if (this->texture) {
            RoundedRectangle::cachedTexture2d[cornerRadius] = this->texture;
        }
    }
    this->rect = Rect(0, 0, this->texture->width, this->texture->height);
}

shared_ptr<Texture2D> RoundedRectangle::createTexture(float cornerRadius) {
    unsigned char alpha1[4] = {255, 255, 255, 255};
    float den = Device::getDeviceDensity();
    int cr = (int)(cornerRadius * den + 0.5f);
    int s = SCALABLE_RECT_SIZE;
    int v = 1;
    int wh = cr + s;
    int length = wh * wh * 4;
    unsigned char *data = new unsigned char[length];
    for (int y = 0; y < s; y++) {
        for (int x = 0; x < wh; x++) {
            int i = y * wh * 4 + x * 4;
            memcpy(&data[i], alpha1, sizeof(unsigned char) * 4);
        }
    }
    for (int x = 0; x < s; x++) {
        for (int y = s; y < wh; y++) {
            int i = y * wh * 4 + x * 4;
            memcpy(&data[i], alpha1, sizeof(unsigned char) * 4);
        }
    }
    for (int y = 0; y < cr; y++) {
        int _y = y + s;
        for (int x = 0; x < cr; x++) {
            int _x = x + s;
            int i = _y * (cr + s) * 4 + _x * 4;
            data[i + 0] = 255;
            data[i + 1] = 255;
            data[i + 2] = 255;
            
            float x2 = x + 0.5f;
            float y2 = y + 0.5f;
            float l = sqrt(x2 * x2 + y2 * y2);
            if (l < cr - v) {
                data[i + 3] = 255;
            } else if (l > cr + v) {
                data[i + 3] = 0;
            } else {
                float dist = cr - l;
                float alpha = smoothstep(-v, v, dist);
                data[i + 3] = 255 * alpha;
            }
        }
    }
    
    return Texture2D::createWithRGBA(data, wh, wh, den);
}

void RoundedRectangle::getVerticesNum(int *num) {
    if (!this->visible) return;
    (*num) += 25;
}

void RoundedRectangle::getIndiciesNum(int *num) {
    if (!this->visible) return;
    if (*num > 0) {
        (*num) += 2;
    }
    (*num) += 46;
}

void RoundedRectangle::bindVertices(float *vertices, int *idx, bool bakeTransform) {
    if (!this->visible) return;
    float *m;
    if (bakeTransform) {
        this->renderer->pushMatrix();
        this->renderer->applyTransform(this->transform, this->screenScale, false);
        m = this->renderer->matrix;
        this->renderer->popMatrix();
    } else {
        m = Renderer::identityMatrix;
    }
    
    auto v1 = Point(m[0], m[1]);
    auto v2 = Point(m[4], m[5]);
    auto offset = Point(m[12], m[13]);
    
    float xx[5] = {
        0,
        this->cornerRadius * this->screenScale,
        this->transform->size.width * 0.5f * this->screenScale,
        (this->transform->size.width - this->cornerRadius) * this->screenScale,
        this->transform->size.width * this->screenScale,
    };
    float yy[5] = {
        0,
        this->cornerRadius * this->screenScale,
        this->transform->size.height * 0.5f * this->screenScale,
        (this->transform->size.height - this->cornerRadius) * this->screenScale,
        this->transform->size.height * this->screenScale,
    };
    
    for (int xi = 0; xi < 5; xi++) {
        for (int yi = 0; yi < 5; yi++) {
            float x = xx[xi];
            float y = yy[yi];
            
            Point p = v1 * x + v2 * y;
            vertices[(*idx)++] = (p.x + offset.x);
            vertices[(*idx)++] = (p.y + offset.y);
        }
    }
}

void RoundedRectangle::bindIndices(short *indices, int *idx, int start) {
    if (!this->visible) return;
    if (start > 0) {
        indices[*idx] = indices[(*idx) - 1];
        (*idx)++;
        indices[(*idx)++] = 0 + start;
    }
    
    short sliceIndices[46] = {
        0, 5, 1, 6, 2, 7, 3, 8, 4, 9,
        9, 5,
        5, 10, 6, 11, 7, 12, 8, 13, 9, 14,
        14, 10,
        10, 15, 11, 16, 12, 17, 13, 18, 14, 19,
        19, 15,
        15, 20, 16, 21, 17, 22, 18, 23, 19, 24,
    };
    
    for (int i = 0; i < 46; i++) {
        indices[(*idx)++] = sliceIndices[i] + start;
    }
}

void RoundedRectangle::bindVertexTexCoords(float *vertexTexCoords, int *idx, float x, float y, float w, float h) {
    if (!this->visible) return;
    
    float den = Device::getDeviceDensity();
    int cr = (int)(this->cornerRadius * den + 0.5f);
    float r = (float)(cr + 1) / (float)(cr + SCALABLE_RECT_SIZE);
    float s = 1.0f / (float)(cr + SCALABLE_RECT_SIZE);
    
    float xx[5] = {
        x + w,
        x + w - r * w,
        x + s * w,
        x + w - r * w,
        x + w,
    };
    float yy[5] = {
        y + h,
        y + h - r * h,
        y + s * w,
        y + h - r * h,
        y + h,
    };
    
    for (int xi = 0; xi < 5; xi++) {
        for (int yi = 0; yi < 5; yi++) {
            float x = xx[xi];
            float y = yy[yi];
            
            vertexTexCoords[(*idx)++] = x;
            vertexTexCoords[(*idx)++] = y;
        }
    }
}

shared_ptr<RoundedRectangle> RoundedRectangle::clone() {
    auto entity = this->cloneEntity();
    return static_pointer_cast<RoundedRectangle>(entity);
}

shared_ptr<Entity> RoundedRectangle::cloneEntity() {
    auto rectangle = shared_ptr<RoundedRectangle>(new RoundedRectangle());
    rectangle->copyFrom((shared_from_this()));
    return rectangle;
}

void RoundedRectangle::copyFrom(const shared_ptr<Entity> &src) {
    Sprite::copyFrom(src);
    auto srcRectangle = static_pointer_cast<RoundedRectangle>(src);
    this->cornerRadius = srcRectangle->cornerRadius;
}

EntityType RoundedRectangle::getEntityType() {
    return EntityType::RoundedRectangle;
}
