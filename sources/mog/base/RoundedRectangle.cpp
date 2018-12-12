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
    
    /*
    vector<Point> vertexPoints;
    int n = max((int)(cornerRadius * 0.375f), 8);
    
    float r1 = 270.0f * (M_PI / 180.0f);
    float r2 = 180.0f * (M_PI / 180.0f);
    float _r = (r2 - r1) / n;
    for (int i = 0; i < n+1; i++) {
        float r = r1 + _r * i;
        float y = cornerRadius * sin(r) + cornerRadius;
        float x1 = cornerRadius * cos(r) + cornerRadius;
        float x2 = size.width - x1;
        vertexPoints.emplace_back(Point(x1, y));
        vertexPoints.emplace_back(Point(x2, y));
    }
    for (int i = n; i >= 0; i--) {
        float r = r1 + _r * i;
        float y = size.height - (cornerRadius * sin(r) + cornerRadius);
        float x1 = cornerRadius * cos(r) + cornerRadius;
        float x2 = size.width - x1;
        vertexPoints.emplace_back(Point(x1, y));
        vertexPoints.emplace_back(Point(x2, y));
    }

    Polygon::init(vertexPoints);
     */
    
    
    float density = Device::getDeviceDensity();
    int texWidth = (int)(cornerRadius * density + 0.5f);
    int texHeight = texWidth;
    unsigned char *data = (unsigned char *)rpmalloc(sizeof(char) * texWidth * texHeight * 4);
    for (int y = 0; y < texHeight; y++) {
        for (int x = 0; x < texWidth; x++) {
            float l = Point::length(Point(x, y));
            float a = (cornerRadius * density) - l;
            if (a > 1.0f) a = 1.0f;
            if (a < 0) a = 0;
            data[(y * texWidth + x) * 4 + 0] = 255;
            data[(y * texWidth + x) * 4 + 1] = 255;
            data[(y * texWidth + x) * 4 + 2] = 255;
            data[(y * texWidth + x) * 4 + 3] = (unsigned char)(a * 255.0f + 0.5f);
        }
    }
    this->initWithRGBA(data, texWidth, texHeight);
    this->initRendererVertices(16, 12);
}
