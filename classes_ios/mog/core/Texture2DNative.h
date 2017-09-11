#ifndef Texture2DNative_h
#define Texture2DNative_h

#include <memory>
#include <vector>

using namespace std;

namespace mog {
    class Texture2D;
    
    class Texture2DNative {
    public:
//        static void loadPvrImage(Texture2D *tex2d, const char *filename);
        
        static void loadFontTexture(Texture2D *tex2d, const char *text, float fontSize, const char *fontFace, float height = 0);
        static string getLocalizedTextNative(const char *textKey, va_list args);
    };
}
#endif /* Texture2DNative_h */
