#ifndef Sprite_h
#define Sprite_h

#include <memory>
#include <string>
#include <unordered_map>
#include "mog/base/Scene.h"
#include "mog/base/Group.h"
#include "mog/base/DrawEntity.h"
#include "mog/core/plain_objects.h"

using namespace std;

namespace mog {
    class Sprite : public DrawEntity {
    public:
        static shared_ptr<Sprite> create(string filename);
        static shared_ptr<Sprite> create(string filename, const Point &framePosition, const Size &frameSize);
        static shared_ptr<Sprite> createWithImage(unsigned char *image, int length);
        static shared_ptr<Sprite> createWithImage(const Bytes &bytes);
        static shared_ptr<Sprite> createWithRGBA(unsigned char *data, int width, int height);
        static shared_ptr<Sprite> createWithTexture(const shared_ptr<Texture2D> &texture);
        
        static void registerCache(string filename);
        static void removeCache(string filename);
        static void clearCache();
        
        shared_ptr<Sprite> clone();
        virtual shared_ptr<Entity> cloneEntity() override;
        
        ~Sprite();
        
    protected:
        Sprite();
        
        static unordered_map<string, weak_ptr<Texture2D>> cachedTexture2d;
        static unordered_map<string, weak_ptr<Texture2D>> globalCachedTexture2d;
        string filename;
        Point framePosition = Point::zero;
        Size frameSize = Size::zero;
        
        void init(string filename);
        void init(string filename, const Point &framePosition, const Size &frameSize);
        void initWithImage(unsigned char *image, int length);
        void initWithRGBA(unsigned char *data, int width, int height);
        void initWithTexture(const shared_ptr<Texture2D> &texture);
        
        virtual void copyFrom(const shared_ptr<Entity> &src) override;
    };
}

#endif /* Sprite_h */
