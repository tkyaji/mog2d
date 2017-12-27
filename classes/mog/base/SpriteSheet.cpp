#include "mog/base/SpriteSheet.h"

using namespace mog;

shared_ptr<SpriteSheet> SpriteSheet::create(const shared_ptr<Sprite> sprite, const Size &frameSize, unsigned int frameCount, unsigned int margin) {
    auto spriteSheet =  shared_ptr<SpriteSheet>(new SpriteSheet());
    spriteSheet->init(sprite, frameSize, frameCount, margin);
    return spriteSheet;
}

SpriteSheet::SpriteSheet() {
    this->dynamicDraw = true;
}

void SpriteSheet::init(const shared_ptr<Sprite> sprite, const Size &frameSize, unsigned int frameCount, unsigned int margin) {
    this->texture = sprite->getTexture();
    this->filename = sprite->getFilename();
    this->transform->size = frameSize;
    this->size = frameSize;
    this->rect = sprite->getRect();
    this->frameSize = frameSize;
    
    this->initFrames(frameCount, margin);
}

void SpriteSheet::initFrames(unsigned int frameCount, unsigned int margin) {
    float _margin = (float)margin / this->texture->density.value;
    int cols = (this->rect.size.width + _margin) / (frameSize.width + _margin);
    int rows = (this->rect.size.height + _margin) / (frameSize.height + _margin);
    
    if (frameCount == 0) {
        frameCount = (unsigned int)(cols, rows);
    }

    this->frameCount = frameCount;
    this->margin = margin;

    this->framePoints.clear();
    this->framePoints.reserve(frameCount);
    
    int r = 0;
    int c = 0;
    for (int i = 0; i < frameCount; i++) {
        float x = c * (frameSize.width + _margin);
        float y = r * (frameSize.height + _margin);
        this->framePoints.emplace_back(Point(x, y));
        
        if (++c >= cols) {
            if (++r >= rows) {
                break;
            };
            c = 0;
        }
    }
    if (frameCount == 0) {
        this->framePoints.emplace_back(Point(0, 0));
    }
    
    this->frameCount = (unsigned int)framePoints.size();
}

void SpriteSheet::setFrameCount(unsigned int frameCount) {
    this->initFrames(frameCount, this->margin);
}

void SpriteSheet::setMargin(unsigned int margin) {
    this->initFrames(this->frameCount, margin);
}

void SpriteSheet::updateFrame(const shared_ptr<Engine> &engine, float delta) {
    this->updateSpriteFrame(delta);
    Sprite::updateFrame(engine, delta);
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
    if (!this->visible) return;

    Size texSize = Size(this->texture->width, this->texture->height) / this->texture->density.value;
    x += this->rect.position.x / texSize.width;
    y += this->rect.position.y / texSize.height;
    
    auto p = this->framePoints[this->frame];
    x += (p.x / texSize.width) * w;
    y += (p.y / texSize.height) * h;
    w = (this->frameSize.width / texSize.width) * w;
    h = (this->frameSize.height / texSize.height) * h;
    
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

unsigned int SpriteSheet::getMargin() {
    return this->margin;
}

Size SpriteSheet::getFrameSize() {
    return this->frameSize;
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
    auto srcSprite = static_pointer_cast<SpriteSheet>(src);
    this->texture = srcSprite->getTexture();
    this->filename = srcSprite->getFilename();
    this->transform->size = srcSprite->getSize();
    this->rect = srcSprite->getRect();
    this->frameSize = srcSprite->getFrameSize();
    this->frameCount = srcSprite->getFrameCount();
    this->margin = srcSprite->getMargin();
}

EntityType SpriteSheet::getEntityType() {
    return EntityType::SpriteSheet;
}

