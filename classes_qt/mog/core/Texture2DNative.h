#ifndef Texture2DNative_h
#define Texture2DNative_h

#include <memory>
#include <vector>
#include <unordered_map>

using namespace std;

namespace mog {
    class Texture2D;
    
    class Texture2DNative {
    public:
        static void loadFontTexture(Texture2D *tex2d, const char *text, float fontSize, const char *fontFilename, float height = 0);
        static string getLocalizedTextNative(const char *textKey, va_list args);

    private:
        static unordered_map<string, string> registeredFontNames;
        static string registerCustomFont(const char *fontFilename);
    };
}

#endif /* Texture2DNative_h */
