#include "mog/Constants.h"
#import "Texture2DNative.h"
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import "Texture2D.h"

using namespace mog;


/*
#define PVR_TEXTURE_FLAG_TYPE_MASK	0xff

static char gPVRTexIdentifier[] = "PVR!";

enum {
    kPVRTextureFlagTypePVRTC_2 = 24,
    kPVRTextureFlagTypePVRTC_4
};

typedef struct _PVRTexHeader {
    uint32_t headerLength;
    uint32_t height;
    uint32_t width;
    uint32_t numMipmaps;
    uint32_t flags;
    uint32_t dataLength;
    uint32_t bpp;
    uint32_t bitmaskRed;
    uint32_t bitmaskGreen;
    uint32_t bitmaskBlue;
    uint32_t bitmaskAlpha;
    uint32_t pvrTag;
    uint32_t numSurfs;
} PVRTexHeader;

void Texture2DNative::loadPvrImage(Texture2D *tex2d, const char *filename) {
    NSString *filenameStr = [NSString stringWithUTF8String:filename];
    NSString *path = [[NSBundle mainBundle] pathForResource:filenameStr ofType:nil];
    if (!path) {
        LOGE("Texture2DNative::loadPvrImage: Resource is not found.");
        LOGE(filename);
        return;
    }

    NSData* data = [NSData dataWithContentsOfFile:path];
    PVRTexHeader* header = (PVRTexHeader *)[data bytes];
    
    uint32_t pvrTag = CFSwapInt32LittleToHost(header->pvrTag);
    
    if (gPVRTexIdentifier[0] != ((pvrTag >>  0) & 0xff) ||
        gPVRTexIdentifier[1] != ((pvrTag >>  8) & 0xff) ||
        gPVRTexIdentifier[2] != ((pvrTag >> 16) & 0xff) ||
        gPVRTexIdentifier[3] != ((pvrTag >> 24) & 0xff)) {
        return;
    }
    
    uint32_t flags = CFSwapInt32LittleToHost(header->flags);
    uint32_t formatFlags = flags & PVR_TEXTURE_FLAG_TYPE_MASK;
    
    if (formatFlags == kPVRTextureFlagTypePVRTC_4 || formatFlags == kPVRTextureFlagTypePVRTC_2){
        tex2d->filename = filename;
        tex2d->width = CFSwapInt32LittleToHost(header->width);
        tex2d->height = CFSwapInt32LittleToHost(header->height);
        tex2d->dataLength = CFSwapInt32LittleToHost(header->dataLength);
        
        GLubyte* rawData = (GLubyte *)malloc(sizeof(GLubyte) * tex2d->dataLength);
        memcpy(rawData, (void *)((unsigned long)[data bytes] + sizeof(PVRTexHeader)), tex2d->dataLength);
        tex2d->data = rawData;

        tex2d->isFlip = true;
        if (formatFlags == kPVRTextureFlagTypePVRTC_4) {
            tex2d->textureType = TextureType::PVRTC4;
            tex2d->bitsPerPixel = 4;
        } else {
            tex2d->textureType = TextureType::PVRTC2;
            tex2d->bitsPerPixel = 2;
        }
    }
}
*/

void Texture2DNative::loadFontTexture(Texture2D *tex2d, const char *text, float fontSize, const char *fontFace, float height) {
    NSString *textStr = [NSString stringWithUTF8String:text];
    NSString *fontFaceStr = [NSString stringWithUTF8String:fontFace];
    if (!textStr || textStr.length == 0) return;
    
    UIFont* font = nil;
    if (fontSize == 0) {
        fontSize = (float)[UIFont systemFontSize];
    }
    
    if (fontFaceStr && fontFaceStr.length > 0) {
        font = [UIFont fontWithName:fontFaceStr size:fontSize];
    }
    if (!font) {
        font = [UIFont systemFontOfSize:fontSize];
    }
    
    CGSize textSize = [textStr sizeWithAttributes:@{NSFontAttributeName:font}];
    int textWidth  = (int)(textSize.width + 0.5f);
    int textHeight = (int)(textSize.height + 0.5f);
    if (height > 0) {
        textHeight = (int)(height + 0.5f);
    }

    GLubyte *bitmap = (GLubyte *)calloc(textWidth * textHeight * 4, sizeof(GLubyte));
    
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    CGContextRef context = CGBitmapContextCreate(bitmap, textWidth, textHeight,
                                                 8, textWidth * 4, colorSpace,
                                                 kCGImageAlphaPremultipliedLast);
    
    UIGraphicsPushContext(context);
    
    CGContextSetTextDrawingMode(context, kCGTextFill);
    
    [textStr drawAtPoint:CGPointZero withAttributes:@{NSFontAttributeName:font,
                                                      NSForegroundColorAttributeName:[UIColor whiteColor]}];
    
    UIGraphicsPopContext();
    
    CGColorSpaceRelease(colorSpace);
    CGContextRelease(context);
    
    // remove premultiplied alpha
    GLubyte rgb[3] = {255, 255, 255};
    for (int i = 0; i < textWidth * textHeight; i++) {
        memcpy(&bitmap[i * 4], rgb, sizeof(GLubyte) * 3);
    }
    
    tex2d->filename = "";
    tex2d->width = textWidth;
    tex2d->height = textHeight;
    tex2d->data = bitmap;
    tex2d->dataLength = textWidth * textHeight * 4;
    tex2d->textureType = TextureType::RGBA;
    tex2d->bitsPerPixel = 4;
    tex2d->isFlip = true;
}

string Texture2DNative::getLocalizedTextNative(const char *textKey, va_list args) {
    NSString *localizedStr = NSLocalizedString([NSString stringWithUTF8String:textKey], nil);
    if (!localizedStr) {
        return "";
    }
    
    const char *utf8Str = localizedStr.UTF8String;
    char *str = new char[strlen(utf8Str) + 4096];
    
    vsprintf(str, utf8Str, args);
    string ret = string(str);
    
    safe_delete_arr(str);
    
    return ret;
}
