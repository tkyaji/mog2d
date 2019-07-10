#ifndef Texture2D_h
#define Texture2D_h

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include "mog/core/opengl.h"
#include "mog/core/plain_objects.h"
#include "mog/core/Screen.h"
#include "mog/core/Data.h"

namespace mog {
    
    enum class TextureType {
        RGBA,
        RGB,
    };
    

    class Texture2D {
    public:
        GLuint textureId = 0;
        std::string filename;
        TextureType textureType = TextureType::RGBA;
        int width = 0;
        int height = 0;
        GLubyte* data = nullptr;
        int dataLength = 0;
        int bitsPerPixel = 0;
        bool isFlip = false;
        Density density = Density::x1_0;
        
        static std::shared_ptr<Texture2D> createWithAsset(std::string filename);
        static std::shared_ptr<Texture2D> createWithFile(std::string filepath, Density density = Density::x1_0);
        static std::shared_ptr<Texture2D> createWithImage(const std::shared_ptr<ByteArray> &bytes);
        static std::shared_ptr<Texture2D> createWithText(std::string text, float fontSize, std::string fontFilename = "", float height = 0);
        static std::shared_ptr<Texture2D> createWithColor(TextureType textureType, const Color &color, int width, int height, Density density);
        static std::shared_ptr<Texture2D> createWithRGBA(unsigned char *data, int width, int height, Density density);
        static void releaseAllBufferes();
        static GLenum getTextureEnum(int textureIdx);

        Texture2D();
        ~Texture2D();
        
        void bindTexture(int textureIdx = 0);
        void bindTextureSub(GLubyte* data, int x, int y, int width, int height);
        void loadImageFromBuffer(unsigned char *buffer, int len);
        
    private:
        static std::unordered_map<intptr_t, std::weak_ptr<Texture2D>> allTextures;
        
        void loadTextureAsset(std::string filename);
        std::shared_ptr<ByteArray> readBytesAsset(std::string filename, Density *density);
        void loadTextureFile(std::string filepath, Density density = Density::x1_0);
        void loadFontTexture(std::string text, float fontSize, std::string fontFilename = "", float height = 0);
        void loadColorTexture(TextureType textureType, const Color &color, int width, int height, Density density = Density::x1_0);
        void releaseBuffer();
    };
}

#endif /* Texture2D_hp */
