#ifndef Sprite_h
#define Sprite_h

#include <memory>
#include <string>
#include <unordered_map>
#include "mog/base/Scene.h"
#include "mog/base/Entity.h"
#include "mog/core/plain_objects.h"
#include "mog/core/Screen.h"

namespace mog {
    class Sprite : public Entity {
        friend class EntityCreator;
    public:
        static std::shared_ptr<Sprite> create(std::string filename, const Rect &rect = Rect::zero);
        static std::shared_ptr<Sprite> createWithFilePath(std::string filepath, Density density = Density::x1_0);
        static std::shared_ptr<Sprite> createWithFilePath(std::string filepath, const Rect &rect, Density density = Density::x1_0);
        static std::shared_ptr<Sprite> createWithImage(const std::shared_ptr<ByteArray> &bytes);
        static std::shared_ptr<Sprite> createWithRGBA(unsigned char *data, int width, int height);
        static std::shared_ptr<Sprite> createWithTexture(const std::shared_ptr<Texture2D> &texture, const Rect &rect = Rect::zero);

        static void registerCache(std::string filename);
        static void removeCache(std::string filename);
        static void clearCache();
        
        std::string getFilename();
        void setFilename(std::string filename);
        Rect getRect();
        void setRect(const Rect &rect);
        std::shared_ptr<Sprite> clone();
        virtual std::shared_ptr<Dictionary> serialize() override;

        ~Sprite();
        
    protected:
        Sprite() {}
        
        static std::unordered_map<std::string, std::weak_ptr<Texture2D>> cachedTexture2d;
        static std::unordered_map<std::string, std::weak_ptr<Texture2D>> globalCachedTexture2d;
        std::string filename;
        Rect rect = Rect::zero;
        
        virtual void init() override;
        void initWithFilePath(std::string filepath, const Rect &rect, Density density);
        void initWithImage(const std::shared_ptr<ByteArray> &bytes);
        void initWithRGBA(unsigned char *data, int width, int height);
        void initWithTexture(const std::shared_ptr<Texture2D> &texture, const Rect &rect);

        virtual void bindVertexTexCoords(const std::shared_ptr<Renderer> &renderer, int *idx, int texIdx, float x, float y, float w, float h) override;
        virtual std::shared_ptr<Entity> cloneEntity() override;
        virtual void deserializeData(const std::shared_ptr<Dictionary> &dict, const std::unordered_map<std::string, std::unordered_map<std::string, std::shared_ptr<Data>>> &params) override;
    };
}

#endif /* Sprite_h */
