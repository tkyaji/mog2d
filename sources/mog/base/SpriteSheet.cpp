#include "mog/base/SpriteSheet.h"

using namespace mog;

std::shared_ptr<SpriteSheet> SpriteSheet::create(std::string filename, const Size &frameSize, unsigned int frameCount, unsigned int margin, const Rect &rect) {
    auto spriteSheet = std::shared_ptr<SpriteSheet>(new SpriteSheet());
    spriteSheet->frameSize = frameSize;
    spriteSheet->frameCount = frameCount;
    spriteSheet->margin = margin;
    spriteSheet->rect = rect;
    spriteSheet->init();
    return spriteSheet;
}

std::shared_ptr<SpriteSheet> SpriteSheet::createWithTexture(const std::shared_ptr<Texture2D> &texture, const Size &frameSize, unsigned int frameCount, unsigned int margin, const Rect &rect) {
    auto spriteSheet = std::shared_ptr<SpriteSheet>(new SpriteSheet());
    spriteSheet->frameSize = frameSize;
    spriteSheet->frameCount = frameCount;
    spriteSheet->margin = margin;
    spriteSheet->rect = rect;
    spriteSheet->initWithTexture(texture);
    return spriteSheet;
}

std::shared_ptr<SpriteSheet> SpriteSheet::createWithSprite(const std::shared_ptr<Sprite> &sprite, const Size &frameSize, unsigned int frameCount, unsigned int margin) {
    return SpriteSheet::createWithTexture(sprite->getTexture(), frameSize, frameCount, margin, sprite->getRect());
}

void SpriteSheet::init() {
    auto texture = Texture2D::createWithAsset(filename);
    return this->initWithTexture(texture);
}

void SpriteSheet::initWithTexture(const std::shared_ptr<Texture2D> &texture) {
    this->textures[0] = texture;
    if (this->rect.size == Size::zero) {
        this->rect.size = Size(this->textures[0]->width / this->textures[0]->density.value,
                          this->textures[0]->height / this->textures[0]->density.value);
    }
    this->transform->size = this->rect.size;
    
    this->initRendererVertices(4, 4);
    this->initFrames(this->frameCount, this->margin);
}

Rect SpriteSheet::getRect() {
    return this->rect;
}

void SpriteSheet::initFrames(unsigned int frameCount, unsigned int margin) {
    float _margin = (float)margin / this->textures[0]->density.value;
    int cols = (this->rect.size.width + _margin) / (frameSize.width + _margin);
    int rows = (this->rect.size.height + _margin) / (frameSize.height + _margin);
    
    if (frameCount == 0) {
        frameCount = (unsigned int)(cols * rows);
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

void SpriteSheet::updateFrame(const std::shared_ptr<Engine> &engine, float delta, float *parentMatrix, unsigned char parentDirtyFlag) {
    this->updateSpriteFrame(delta);
    Entity::updateFrame(engine, delta, parentMatrix, parentDirtyFlag);
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
                } else if (this->loopType == LoopType::PingPong) {
                    this->nextFrame -= offset;
                }
                
            } else {
                this->nextFrame += offset;
            }
        }
    }
}

void SpriteSheet::selectFrame(unsigned int frame) {
    LOGD("%d\n", frame);
    this->frame = frame % this->frameCount;
    this->dirtyFlag |= DIRTY_TEX_COORDS;
}

void SpriteSheet::startAnimation(float timePerFrame, LoopType loopType, int loopCount, int startFrame, int endFrame) {
    for (int i = 0; i < frameCount; i++) {
        this->timePerFrames.emplace_back(timePerFrame);
    }
    this->startAnimation(this->timePerFrames, loopType, startFrame, endFrame);
}

void SpriteSheet::startAnimation(const std::vector<float> &timePerFrames, LoopType loopType, int loopCount, int startFrame, int endFrame) {
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
        this->onFinishEvent(std::static_pointer_cast<SpriteSheet>(shared_from_this()));
    }
}

void SpriteSheet::bindVertexTexCoords(const std::shared_ptr<Renderer> &renderer, int *idx, int texIdx, float x, float y, float w, float h) {
    Size texSize = Size(this->textures[0]->width, this->textures[0]->height) / this->textures[0]->density.value;
    x += this->rect.position.x / texSize.width;
    y += this->rect.position.y / texSize.height;
    
    auto p = this->framePoints[this->frame];
    x += (p.x / texSize.width) * w;
    y += (p.y / texSize.height) * h;
    w = (this->frameSize.width / texSize.width) * w;
    h = (this->frameSize.height / texSize.height) * h;
    
    if (this->textures[0]->isFlip) {
        renderer->vertexTexCoords[texIdx][(*idx)++] = x;      renderer->vertexTexCoords[texIdx][(*idx)++] = y + h;
        renderer->vertexTexCoords[texIdx][(*idx)++] = x;      renderer->vertexTexCoords[texIdx][(*idx)++] = y;
        renderer->vertexTexCoords[texIdx][(*idx)++] = x + w;  renderer->vertexTexCoords[texIdx][(*idx)++] = y + h;
        renderer->vertexTexCoords[texIdx][(*idx)++] = x + w;  renderer->vertexTexCoords[texIdx][(*idx)++] = y;
        
    } else {
        renderer->vertexTexCoords[texIdx][(*idx)++] = x;      renderer->vertexTexCoords[texIdx][(*idx)++] = y;
        renderer->vertexTexCoords[texIdx][(*idx)++] = x;      renderer->vertexTexCoords[texIdx][(*idx)++] = y + h;
        renderer->vertexTexCoords[texIdx][(*idx)++] = x + w;  renderer->vertexTexCoords[texIdx][(*idx)++] = y;
        renderer->vertexTexCoords[texIdx][(*idx)++] = x + w;  renderer->vertexTexCoords[texIdx][(*idx)++] = y + h;
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

void SpriteSheet::setOnFinishEvent(std::function<void(const std::shared_ptr<SpriteSheet> &spriteSheet)> onFinishEvent) {
    this->onFinishEvent = onFinishEvent;
}

std::shared_ptr<SpriteSheet> SpriteSheet::clone() {
    auto e = this->cloneEntity();
    return std::static_pointer_cast<SpriteSheet>(e);
}

std::shared_ptr<Entity> SpriteSheet::cloneEntity() {
    auto spriteSheet = SpriteSheet::createWithTexture(this->textures[0], this->frameSize, this->frameCount, this->margin, this->rect);
    spriteSheet->copyProperties(std::static_pointer_cast<Entity>(shared_from_this()));
    return spriteSheet;
}
