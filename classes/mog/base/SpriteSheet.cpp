#include "mog/base/SpriteSheet.h"

using namespace mog;

shared_ptr<SpriteSheet> SpriteSheet::create(string filename, unsigned int frameCount, const Size &frameSize, int margin, Point startPosition) {
    auto spriteSheet =  shared_ptr<SpriteSheet>(new SpriteSheet());
    spriteSheet->init(filename, frameCount, frameSize, margin, startPosition);
    return spriteSheet;
}

SpriteSheet::SpriteSheet() {
    this->dynamicDraw = true;
}

void SpriteSheet::init(string filename, unsigned int frameCount, const Size &frameSize, int margin, Point startPosition) {
    this->frameSize = frameSize;
    this->margin = margin;
    this->startPosition = startPosition;
    this->texture = Texture2D::createWithImage(filename);
    
    this->framePoints.reserve(frameCount);
    
    Size texSize = Size(this->texture->width, this->texture->height) / this->texture->density.value;
    int cols = (texSize.width - startPosition.x + margin) / (frameSize.width + margin);
    int rows = (texSize.height - startPosition.y + margin) / (frameSize.height + margin);
    
    int r = 0;
    int c = 0;
    for (int i = 0; i < frameCount; i++) {
        float x = c * (frameSize.width + margin) + startPosition.x;
        float y = r * (frameSize.height + margin) + startPosition.y;
        this->framePoints.emplace_back(Point(x, y));
        
        if (++c >= cols) {
            if (++r >= rows) {
                break;
            };
            c = 0;
        }
    }
    
    this->transform->size = frameSize;
    this->frameCount = (unsigned int)framePoints.size();
}

void SpriteSheet::updateFrame(float delta) {
    this->updateSpriteFrame(delta);
    Sprite::updateFrame(delta);
}

void SpriteSheet::updateSpriteFrame(float delta) {
    if (this->animating) {
        this->tmpTime += delta;
        while (this->tmpTime >= this->timePerFrames[this->nextFrame]) {
            this->tmpTime -= this->timePerFrames[this->nextFrame];
            this->selectFrame(this->nextFrame);
            
            int offset = 1;
            int endFrame = this->endFrame;
            if (this->loopType == LoopType::PingPong && this->currentLoopCount % 2 == 1) {
                endFrame = this->startFrame;
                offset = -1;
            }
            if (this->nextFrame == endFrame) {
                this->currentLoopCount++;
                if (this->loopType == LoopType::None || (this->loopCount > 0 && this->currentLoopCount >= this->loopCount)) {
                    this->stopAnimation();
                } else if (this->loopType == LoopType::Loop) {
                    this->nextFrame = 0;
                }
                
            } else {
                this->nextFrame += offset;
            }
        }
    }
}

void SpriteSheet::selectFrame(unsigned int frame) {
    this->frame = frame % this->frameCount;
    this->setReRenderFlag(RERENDER_TEX_COORDS);
}

void SpriteSheet::startAnimation(float timePerFrame, LoopType loopType, int loopCount, int startFrame, int endFrame) {
    for (int i = 0; i < frameCount; i++) {
        this->timePerFrames.emplace_back(timePerFrame);
    }
    this->startAnimation(this->timePerFrames, loopType, startFrame, endFrame);
}

void SpriteSheet::startAnimation(const vector<float> &timePerFrames, LoopType loopType, int loopCount, int startFrame, int endFrame) {
    this->timePerFrames = timePerFrames;
    
    this->startFrame = startFrame;
    if (this->endFrame <= 0) this->endFrame = this->frameCount - 1;
    if (this->endFrame <= this->startFrame) {
        return;
    }
    
    this->loopType = loopType;
    this->loopCount = loopCount;
    this->startFrame = this->frame;
    this->animating = true;
    
    this->tmpTime = 0;
    this->currentLoopCount = 0;
    this->nextFrame = startFrame + 1;
    this->selectFrame(startFrame);
}

void SpriteSheet::stopAnimation() {
    this->animating = false;
    if (this->onFinishEvent) {
        this->onFinishEvent(static_pointer_cast<SpriteSheet>(shared_from_this()));
    }
}

void SpriteSheet::bindVertexTexCoords(float *vertexTexCoords, int *idx, float x, float y, float w, float h) {
    auto p = this->framePoints[this->frame];
    Size texSize = Size(this->texture->width, this->texture->height) / this->texture->density.value;
    x += (p.x / texSize.width) * w;
    y += (p.y / texSize.height) * h;
    w = w * (this->frameSize.width / texSize.width);
    h = h * (this->frameSize.height / texSize.height);
    
    if (this->texture->isFlip) {
        vertexTexCoords[(*idx)++] = x;      vertexTexCoords[(*idx)++] = y + h;
        vertexTexCoords[(*idx)++] = x;      vertexTexCoords[(*idx)++] = y;
        vertexTexCoords[(*idx)++] = x + w;  vertexTexCoords[(*idx)++] = y + h;
        vertexTexCoords[(*idx)++] = x + w;  vertexTexCoords[(*idx)++] = y;
        
    } else {
        vertexTexCoords[(*idx)++] = x;      vertexTexCoords[(*idx)++] = y;
        vertexTexCoords[(*idx)++] = x;      vertexTexCoords[(*idx)++] = y + h;
        vertexTexCoords[(*idx)++] = x + w;  vertexTexCoords[(*idx)++] = y;
        vertexTexCoords[(*idx)++] = x + w;  vertexTexCoords[(*idx)++] = y + h;
    }
}

unsigned int SpriteSheet::getCurrentFrame() {
    return this->frame;
}

unsigned int SpriteSheet::getFrameCount() {
    return this->frameCount;
}

void SpriteSheet::setOnFinishEvent(function<void(const shared_ptr<SpriteSheet> &spriteSheet)> onFinishEvent) {
    this->onFinishEvent = onFinishEvent;
}

shared_ptr<SpriteSheet> SpriteSheet::clone() {
    auto entity = this->cloneEntity();
    return static_pointer_cast<SpriteSheet>(entity);
}

shared_ptr<Entity> SpriteSheet::cloneEntity() {
    auto spriteSheet =  shared_ptr<SpriteSheet>(new SpriteSheet());
    spriteSheet->copyFrom(shared_from_this());
    return spriteSheet;
}

void SpriteSheet::copyFrom(const shared_ptr<Entity> &src) {
    Sprite::copyFrom(src);
    auto srcSprite = static_pointer_cast<SpriteSheet>(src);
    this->frameCount = srcSprite->frameCount;
    this->frameSize = srcSprite->frameSize;
    this->margin = srcSprite->margin;
    this->startPosition = srcSprite->startPosition;
}
