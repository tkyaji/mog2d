#ifndef Texture2DNative_h
#define Texture2DNative_h

#include <memory>
#include <vector>
#include <unordered_map>
#include "mog/core/Texture2D.h"

namespace mog {
    class Texture2DNative {
    public:
        static void loadFontTexture(Texture2D *tex2d, const char *text, float fontSize, const char *fontFilename, float height = 0, TextDrawingMode textMode = TextDrawingMode::Fill, float strokeWidth = 0);
        static std::string getLocalizedTextNative(const char *textKey, va_list args);
        
    private:
        static std::unordered_map<std::string, std::string> registeredFontNames;
        static std::string registerCustomFont(const char *fontFilename);
    };
}

#endif /* Texture2DNative_h */
