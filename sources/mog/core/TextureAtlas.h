#ifndef TextureAtlas_h
#define TextureAtlas_h

#include <memory>
#include <vector>
#include <unordered_map>
#include "mog/core/Texture2D.h"

namespace mog {
    class TextureAtlasCell {
    public:
        std::shared_ptr<Texture2D> texture;
        int x = 0;
        int y = 0;
        int width = 0;
        int height = 0;
        
        TextureAtlasCell(const std::shared_ptr<Texture2D> &texture);
    };
    
    
    class TextureAtlas {
    public:
        int width = 0;
        int height = 0;
        std::shared_ptr<Texture2D> texture;
        
        void addTexture(const std::shared_ptr<Texture2D> &tex2d);
//        shared_ptr<TextureAtlasCell> addTexture(const shared_ptr<Texture2D> &tex2d);
//        void apply();
        
//        static vector<shared_ptr<TextureAtlas>> createTextureAtlas(const vector<shared_ptr<Texture2D>> &textures);
        
        TextureAtlas() {};
//        TextureAtlas(const vector<shared_ptr<Texture2D>> &textures);
//        TextureAtlas(const vector<shared_ptr<TextureAtlasCell>> &cells, int width, int height);
        std::shared_ptr<Texture2D> createTexture();
        std::shared_ptr<TextureAtlasCell> getCell(const std::shared_ptr<Texture2D> &tex2d);
        
        void bindTexture();
        void bindTextureSub(const std::shared_ptr<TextureAtlasCell> &cell);
//        void bindTextureSub(shared_ptr<Texture2D> tex2d);

        
    private:
        std::vector<std::shared_ptr<TextureAtlasCell>> cells;
        std::unordered_map<std::shared_ptr<Texture2D>, std::shared_ptr<TextureAtlasCell>> cellMap;
        
        /*
        int _x = 0;
        int _y = 0;
        int _w = 0;
        int _rh = 0;
        bool applyed = false;
         */
        
        void mapTextureCells();
        void readTexturePixels(unsigned char *dst, const std::shared_ptr<Texture2D> &tex2d, int x, int y, int width, int height);
    };
}

#endif /* TextureAtlas_h */
