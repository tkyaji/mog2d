#ifndef SpriteSheet_h
#define SpriteSheet_h

#include "mog/base/Sprite.h"
#include "mog/core/Tween.h"

namespace mog {
    class SpriteSheet : public Entity {
        friend class EntityCreator;
    public:
        static std::shared_ptr<SpriteSheet> create(std::string filename, const Size &frameSize, unsigned int frameCount = 0, unsigned int margin = 0, const Rect &rect = Rect::zero);
        static std::shared_ptr<SpriteSheet> createWithTexture(const std::shared_ptr<Texture2D> &texture, const Size &frameSize, unsigned int frameCount = 0, unsigned int margin = 0, const Rect &rect = Rect::zero);
        static std::shared_ptr<SpriteSheet> createWithSprite(const std::shared_ptr<Sprite> &sprite, const Size &frameSize, unsigned int frameCount = 0, unsigned int margin = 0);
        void selectFrame(unsigned int frame);
        
        virtual void updateFrame(const std::shared_ptr<Engine> &engine, float delta, float *parentMatrix, unsigned char parentDirtyFlag) override;
        virtual void bindVertexTexCoords(const std::shared_ptr<Renderer> &renderer, int *idx, int texIdx, float x, float y, float w, float h) override;
        
        unsigned int getCurrentFrame();
        unsigned int getFrameCount();
        unsigned int getMargin();
        Size getFrameSize();
        void setFrameCount(unsigned int frameCount);
        void setMargin(unsigned int margin);
        Rect getRect();

        void startAnimation(float timePerFrame, LoopType loopType = LoopType::None, int loopCount = 0, int startFrame = 0, int endFrame = 0);
        void startAnimation(const std::vector<float> &timePerFrames, LoopType loopType = LoopType::None, int loopCount = 0, int startFrame = 0, int endFrame = 0);
        void stopAnimation();
        void setOnFinishEvent(std::function<void(const std::shared_ptr<SpriteSheet> &spriteSheet)> onFinishEvent);
        std::shared_ptr<SpriteSheet> clone();
        
    protected:
        SpriteSheet() {}
        
        std::string filename = "";
        unsigned int frame = 0;
        unsigned int frameCount = 0;
        Size frameSize = Size::zero;
        unsigned int margin = 0;
        Rect rect = Rect::zero;

        std::vector<Point> framePoints;
        std::vector<float> timePerFrames;
        int startFrame = 0;
        int endFrame = 0;
        bool animating = false;
        LoopType loopType = LoopType::None;
        int loopCount = 0;
        float tmpTime = 0;
        int currentLoopCount = 0;
        int nextFrame = 0;
        std::function<void(const std::shared_ptr<SpriteSheet> &spriteSheet)> onFinishEvent;

        void updateSpriteFrame(float delta);
        virtual void init() override;
        void initWithTexture(const std::shared_ptr<Texture2D> &texture);
        void initFrames(unsigned int frameCount, unsigned int margin);
        virtual std::shared_ptr<Entity> cloneEntity() override;
    };
}

#endif /* SpriteSheet_h */
