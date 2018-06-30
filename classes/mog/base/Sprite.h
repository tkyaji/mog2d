#ifndef Sprite_h
#define Sprite_h

#include <memory>
#include <string>
#include <unordered_map>
#include "mog/base/Scene.h"
#include "mog/base/Entity.h"
#include "mog/core/plain_objects.h"
#include "mog/core/Density.h"

using namespace std;

namespace mog {
    class Sprite : public Entity {
    public:
        static shared_ptr<Sprite> create(string filename);
        static shared_ptr<Sprite> create(string filename, const Rect &rect);
        static shared_ptr<Sprite> createWithFilePath(string filepath, Density density = Density::x1_0);
        static shared_ptr<Sprite> createWithFilePath(string filepath, const Rect &rect, Density density = Density::x1_0);
        static shared_ptr<Sprite> createWithImage(const Bytes &bytes);
        static shared_ptr<Sprite> createWithRGBA(unsigned char *data, int width, int height);
        static shared_ptr<Sprite> createWithTexture(const shared_ptr<Texture2D> &texture);
        
        static void registerCache(string filename);
        static void removeCache(string filename);
        static void clearCache();
        
        string getFilename();
        Rect getRect();
        shared_ptr<Sprite> clone();

        ~Sprite();
        
    protected:
        Sprite();
        
        static unordered_map<string, weak_ptr<Texture2D>> cachedTexture2d;
        static unordered_map<string, weak_ptr<Texture2D>> globalCachedTexture2d;
        string filename;
        Rect rect = Rect::zero;
        
        void init(string filename, const Rect &rect);
        void initWithFilePath(string filepath, const Rect &rect, Density density);
        void initWithImage(const Bytes &bytes);
        void initWithRGBA(unsigned char *data, int width, int height);
        void initWithTexture(const shared_ptr<Texture2D> &texture);

        virtual void bindVertexTexCoords(const std::shared_ptr<Renderer> &renderer, int *idx, float x, float y, float w, float h) override;
    };
}

#endif /* Sprite_h */
