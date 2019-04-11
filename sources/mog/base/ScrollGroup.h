#ifndef ScrollGroup_h
#define ScrollGroup_h

#include "mog/mog.h"

#define SCROLL_VERTICAL     0x0001
#define SCROLL_HORIZONTAL   0x0010
#define SCROLL_BOTH         0x0011

class ScrollGroup : public mog::Group {
public:
    static std::shared_ptr<ScrollGroup> create(const mog::Size &scrollSize, const mog::Size &contentSize, unsigned char scrollFlag = SCROLL_BOTH);

    virtual void updateFrame(const std::shared_ptr<mog::Engine> &engine, float delta, float *parentMatrix, unsigned char parentReRenderFlag = 0) override;
    virtual void drawFrame(float delta) override;
    virtual void add(const std::shared_ptr<Entity> &entity) override;
    virtual void remove(const std::shared_ptr<Entity> &entity) override;
    virtual void removeAll() override;

    void setScrollPosition(const mog::Point &position);

protected:
    std::shared_ptr<mog::Group> contentGroup;
    mog::Point velocity = mog::Point::zero;
    bool dragging = false;
    unsigned char scrollFlag = SCROLL_BOTH;
    
    void init(const mog::Size &scrollSize, const mog::Size &contentSize, unsigned char scrollFlag);
};

#endif /* ScrollGroup_h */
