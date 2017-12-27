#include "mog/Constants.h"
#include "mog/core/Texture2D.h"
#include "mog/core/Texture2DNative.h"
#include "mog/core/FileUtils.h"
#include <stdlib.h>
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include "mog/libs/stb_image.h"

using namespace mog;

const Density Density::x1_0 = Density(0);
const Density Density::x1_5 = Density(1);
const Density Density::x2_0 = Density(2);
const Density Density::x3_0 = Density(3);
const Density Density::x4_0 = Density(4);

shared_ptr<Texture2D> Texture2D::createWithAsset(string filename) {
    auto tex2d = make_shared<Texture2D>();
    tex2d->loadTextureAsset(filename);
    return tex2d;
}

shared_ptr<Texture2D> Texture2D::createWithFile(string filepath, Density density) {
    auto tex2d = make_shared<Texture2D>();
    tex2d->loadTextureFile(filepath, density);
    return tex2d;
}

shared_ptr<Texture2D> Texture2D::createWithImage(unsigned char *image, int length) {
    auto tex2d = make_shared<Texture2D>();
    tex2d->loadImageFromBuffer(image, length);
    return tex2d;
}

shared_ptr<Texture2D> Texture2D::createWithText(string text, float fontSize, string fontFilename, float height) {
    auto tex2d = make_shared<Texture2D>();
    tex2d->loadFontTexture(text, fontSize, fontFilename, height);
    return tex2d;
}

shared_ptr<Texture2D> Texture2D::createWithColor(TextureType textureType, const Color &color, int width, int height, Density density) {
    auto tex2d = make_shared<Texture2D>();
    tex2d->loadColorTexture(textureType, color, width, height, density);
    return tex2d;
}

shared_ptr<Texture2D> Texture2D::createWithRGBA(unsigned char *data, int width, int height, Density density) {
    auto tex2d = make_shared<Texture2D>();
    tex2d->data = data;
    tex2d->dataLength = width * height * 4;
    tex2d->width = width;
    tex2d->height = height;
    tex2d->bitsPerPixel = 4;
    tex2d->textureType = TextureType::RGBA;
    tex2d->density = density;
    return tex2d;
}

Texture2D::Texture2D() {
}

Texture2D::~Texture2D() {
    free(this->data);
    if (this->textureId > 0) {
        glDeleteTextures(1, &this->textureId);
    }
}

void Texture2D::loadTextureAsset(string filename) {
    unsigned char *buffer = nullptr;
    int len = 0;
    Density den = Density::x1_0;
    
    if (!this->readBytesAsset(filename, &buffer, &len, &den)) {
        LOGE("Asset not found: %s", filename.c_str());
    }
    
    this->filename = filename;
    this->density = den;
    this->loadImageFromBuffer(buffer, len);
    safe_free(buffer);
}

void Texture2D::loadTextureFile(string filepath, Density density) {
    this->density = density;
    unsigned char *buffer = nullptr;
    int len = 0;
    FileUtils::readDataFromFile(filepath, &buffer, &len);
    this->loadImageFromBuffer(buffer, len);
    safe_free(buffer);
}

bool Texture2D::readBytesAsset(string filename, unsigned char **data, int *len, Density *density) {
    Density den = Density::getCurrent();
    if (FileUtils::readBytesAsset(den.directory + "/" + filename, data, len)) {
        *density = den;
        return true;
    }
    for (int i = den.idx + 1; i < Density::allDensities.size(); i++) {
        den = Density::allDensities[i];
        if (FileUtils::readBytesAsset(den.directory + "/" + filename, data, len)) {
            *density = den;
            return true;
        }
    }
    for (int i = den.idx - 1; i >= 0; i--) {
        den = Density::allDensities[i];
        if (FileUtils::readBytesAsset(den.directory + "/" + filename, data, len)) {
            *density = den;
            return true;
        }
    }
    if (FileUtils::readBytesAsset(filename, data, len)) {
        *density = den;
        return true;
    }
    return false;
}

void Texture2D::loadFontTexture(string text, float fontSize, string fontFilename, float height) {
    Density den = Density::getCurrent();
    Texture2DNative::loadFontTexture(this, text.c_str(), fontSize * den.value, fontFilename.c_str(), height * den.value);
    this->density = den;
}

GLenum toGLFormat(TextureType textureType) {
    GLenum format = GL_RGBA;
    switch (textureType){
        case TextureType::RGBA:
            format = GL_RGBA;
            break;
            
        case TextureType::RGB:
            format = GL_RGB;
            break;
    }
    return format;
}

void Texture2D::bindTexture() {
    if (this->textureId == 0) {
        glGenTextures(1, &this->textureId);
    }
    
    glBindTexture(GL_TEXTURE_2D, this->textureId);
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    GLenum format = toGLFormat(this->textureType);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->width, this->height, 0, format, GL_UNSIGNED_BYTE, this->data);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::bindTextureSub(GLubyte* data, int x, int y, int width, int height) {
    glBindTexture(GL_TEXTURE_2D, this->textureId);
    
    GLenum format = toGLFormat(this->textureType);
    
    glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, format, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::loadColorTexture(TextureType textureType, const Color &color, int width, int height, Density density) {
    this->textureType = textureType;
    this->width = width;
    this->height = height;
    this->density = density;
    
    if (textureType == TextureType::RGB) {
        this->bitsPerPixel = 3;
        this->dataLength = width * height * 3;
        this->data = (GLubyte *)calloc(width * height * 3, sizeof(GLubyte));
        for (int i = 0; i < width * height; i++) {
            this->data[i * 3 + 0] = (int)(color.r * 255.0f);
            this->data[i * 3 + 1] = (int)(color.g * 255.0f);
            this->data[i * 3 + 2] = (int)(color.b * 255.0f);
        }
        
    } else {
        this->bitsPerPixel = 4;
        this->dataLength = width * height * 4;
        this->data = (GLubyte *)calloc(width * height * 4, sizeof(GLubyte));
        for (int i = 0; i < width * height; i++) {
            this->data[i * 4 + 0] = (int)(color.r * 255.0f);
            this->data[i * 4 + 1] = (int)(color.g * 255.0f);
            this->data[i * 4 + 2] = (int)(color.b * 255.0f);
            this->data[i * 4 + 3] = (int)(color.a * 255.0f);
        }
    }
}

void Texture2D::loadImageFromBuffer(unsigned char *buffer, int len) {
    int x = 0;
    int y = 0;
    int _n = 0;
    int n = 4;  // fixed RGBA
    unsigned char *data = stbi_load_from_memory(buffer, len, &x, &y, &_n, n);
    if (data == nullptr) {
        LOGE("Texture2D::loadImageFromBuffer: This image format is not supported.");
        LOGE(filename.c_str());
        return;
    };
    
    if (n == 4) {
        this->textureType = TextureType::RGBA;
    } else if (n == 3) {
        this->textureType = TextureType::RGB;
    } else {
        LOGE("Texture2D::loadImageFromBuffer: This image format is not supported.");
        LOGE(filename.c_str());
        return;
    }
    
    this->data = data;
    this->width = x;
    this->height = y;
    this->bitsPerPixel = n;
    this->dataLength = x * y * n;
}
