#ifndef Texture2D_h
#define Texture2D_h

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include "mog/core/opengl.h"
#include "mog/core/plain_objects.h"
#include "mog/core/Density.h"

using namespace std;

namespace mog {
    
    enum class TextureType {
        RGBA,
        RGB,
    };
    
    
    class Texture2D : public enable_shared_from_this<Texture2D> {
    public:
        GLuint textureId = 0;
        string filename;
        TextureType textureType = TextureType::RGBA;
        int width = 0;
        int height = 0;
        GLubyte* data = nullptr;
        int dataLength = 0;
        int bitsPerPixel = 0;
        bool isFlip = false;
        Density density = Density::x1_0;
        
        static shared_ptr<Texture2D> createWithAsset(string filename);
        static shared_ptr<Texture2D> createWithFile(string filepath, Density density = Density::x1_0);
        static shared_ptr<Texture2D> createWithImage(unsigned char *image, int length);
        static shared_ptr<Texture2D> createWithText(string text, float fontSize, string fontFilename = "", float height = 0);
        static shared_ptr<Texture2D> createWithColor(TextureType textureType, const Color &color, int width, int height, Density density = Density::x1_0);
        static shared_ptr<Texture2D> createWithRGBA(unsigned char *data, int width, int height, Density density = Density::x1_0);
        
        Texture2D();
        ~Texture2D();
        
        void bindTexture();
        void bindTextureSub(GLubyte* data, int x, int y, int width, int height);
        void loadImageFromBuffer(unsigned char *buffer, int len);
        
    private:
        void loadTextureAsset(string filename);
        bool readBytesAsset(string filename, unsigned char **data, int *len, Density *density);
        void loadTextureFile(string filepath, Density density = Density::x1_0);
        void loadFontTexture(string text, float fontSize, string fontFilename = "", float height = 0);
        void loadColorTexture(TextureType textureType, const Color &color, int width, int height, Density density = Density::x1_0);
    };
}

#endif /* Texture2D_hp */
