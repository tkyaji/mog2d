#ifndef SpriteSheet_h
#define SpriteSheet_h

#include "mog/base/Sprite.h"
#include "mog/core/Tween.h"

namespace mog {
    class SpriteSheet : public Sprite {
    public:
        static shared_ptr<SpriteSheet> create(const shared_ptr<Sprite> sprite, const Size &frameSize, unsigned int frameCount = 0, unsigned int margin = 0);
        void selectFrame(unsigned int frame);
        
        virtual void updateFrame(const shared_ptr<Engine> &engine, float delta) override;
        virtual void bindVertexTexCoords(float *vertexTexCoords, int *idx, float x, float y, float w, float h) override;
        
        unsigned int getCurrentFrame();
        unsigned int getFrameCount();
        unsigned int getMargin();
        Size getFrameSize();
        void setFrameCount(unsigned int frameCount);
        void setMargin(unsigned int margin);

        void startAnimation(float timePerFrame, LoopType loopType = LoopType::None, int loopCount = 0, int startFrame = 0, int endFrame = 0);
        void startAnimation(const vector<float> &timePerFrames, LoopType loopType = LoopType::None, int loopCount = 0, int startFrame = 0, int endFrame = 0);
        void stopAnimation();
        void setOnFinishEvent(function<void(const shared_ptr<SpriteSheet> &spriteSheet)> onFinishEvent);
        
        shared_ptr<SpriteSheet> clone();
        virtual shared_ptr<Entity> cloneEntity() override;
        virtual EntityType getEntityType() override;

    protected:
        unsigned int frame = 0;
        unsigned int frameCount = 0;
        Size frameSize = Size::zero;
        unsigned int margin = 0;
        
        vector<Point> framePoints;
        vector<float> timePerFrames;
        int startFrame = 0;
        int endFrame = 0;
        bool animating = false;
        LoopType loopType = LoopType::None;
        int loopCount = 0;
        float tmpTime = 0;
        int currentLoopCount = 0;
        int nextFrame = 0;
        function<void(const shared_ptr<SpriteSheet> &spriteSheet)> onFinishEvent;

        SpriteSheet();
        
        void updateSpriteFrame(float delta);
        void init(const shared_ptr<Sprite> sprite, const Size &frameSize, unsigned int frameCount, unsigned int margin);
        void initFrames(unsigned int frameCount, unsigned int margin);
        virtual void copyFrom(const shared_ptr<Entity> &src) override;
    };
}

#endif /* SpriteSheet_h */
