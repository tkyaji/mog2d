#include "mog/Constants.h"
#include "mog/core/Texture2DNative.h"
#define STB_TRUETYPE_IMPLEMENTATION
#include "mog/libs/stb_truetype.h"
#include "mog/core/FileUtils.h"

using namespace mog;

void Texture2DNative::loadFontTexture(Texture2D *tex2d, const char *text, float fontSize, const char *fontFilename, float height) {
    auto byteArr = FileUtils::readBytesAsset(fontFilename);
    unsigned char *fontBuffer;
    unsigned int length;
    byteArr->getValue(&fontBuffer, &length);
    
    int arrLen = (int)strlen(text);
    stbtt_fontinfo font;
    stbtt_InitFont(&font, fontBuffer, stbtt_GetFontOffsetForIndex(fontBuffer, 0));
    float scale = stbtt_ScaleForPixelHeight(&font, fontSize);
    
    int ascent, descent, lineGap;
    stbtt_GetFontVMetrics(&font, &ascent, &descent, &lineGap);
    int baseline = (int)(ascent * scale);
    
    int hMax = 0;
    int wSum = 0;
    int *pos = (int *)mogmalloc(sizeof(int) * arrLen * 3);
    for (int i = 0; i < arrLen; i++) {
        int uc = (int)text[i];
        int advanceWidth, leftSideBearing;
        stbtt_GetCodepointHMetrics(&font, uc, &advanceWidth, &leftSideBearing);
        
        int x0, y0, x1, y1;
        stbtt_GetCodepointBitmapBoxSubpixel(&font, uc, scale, scale, 0, 0, &x0, &y0, &x1, &y1);
        
        int w = (int)(advanceWidth*scale + 0.5);
        int h = (int)(ascent*scale - descent*scale + lineGap*scale + 0.5);
        if (h > hMax) hMax = h;
        wSum += w;
        
        pos[i*3+0] = x0;
        pos[i*3+1] = y0;
        pos[i*3+2] = w;
    }
    if (height == 0) height = hMax;
    
    int x = 0;
    unsigned char *tmpBitmap = (unsigned char *)mogcalloc(sizeof(char), wSum * height);
    for (int i = 0; i < arrLen; i++) {
        int uc = (int)text[i];
        int x0 = pos[i*3+0];
        int y0 = pos[i*3+1];
        int w = pos[i*3+2];
        stbtt_MakeCodepointBitmapSubpixel(&font, &tmpBitmap[(baseline + y0) * wSum + x0 + x], w - x0, height - (baseline + y0), wSum, scale, scale, 0, 0, uc);
        x += w;
    }
    free(pos);
    
    unsigned char *bitmap = (unsigned char *)mogmalloc(sizeof(char) * wSum * height * 4);
    int _i = 0;
    for (int  _y = 0; _y < height; _y++) {
        for (int _x = 0; _x < wSum; _x++) {
            bitmap[_i * 4 + 0] = 255;
            bitmap[_i * 4 + 1] = 255;
            bitmap[_i * 4 + 2] = 255;
            bitmap[_i * 4 + 3] = tmpBitmap[_i];
            _i++;
        }
    }
    
    tex2d->filename = "";
    tex2d->width = wSum;
    tex2d->height = height;
    tex2d->data = bitmap;
    tex2d->dataLength = wSum * height * 4;
    tex2d->textureType = TextureType::RGBA;
    tex2d->bitsPerPixel = 4;
    tex2d->isFlip = false;
}

std::string Texture2DNative::getLocalizedTextNative(const char *textKey, va_list args) {
    return "";
}
