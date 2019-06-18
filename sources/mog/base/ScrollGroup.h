#ifndef ScrollGroup_h
#define ScrollGroup_h

#include "mog/mog.h"

#define SCROLL_VERTICAL     0x0001
#define SCROLL_HORIZONTAL   0x0010
#define SCROLL_BOTH         0x0011

class ScrollGroup : public mog::Group {
public:
    static std::shared_ptr<ScrollGroup> create(const mog::Size &scrollSize, const mog::Size &contentSize, unsigned char scrollFlag = SCROLL_BOTH);

    virtual void updateFrame(const std::shared_ptr<mog::Engine> &engine, float delta, float *parentMatrix, unsigned char parentDirtyFlag = 0) override;
    virtual void drawFrame(float delta) override;
    virtual void add(const std::shared_ptr<Entity> &entity) override;
    virtual void remove(const std::shared_ptr<Entity> &entity) override;
    virtual void removeAll() override;
    std::vector<std::shared_ptr<Entity>> getChildEntities();
    std::shared_ptr<Entity> findChildByName(std::string name, bool recursive = true);
    std::shared_ptr<Entity> findFirstChildByTag(std::string tag, bool recursive = true);
    std::vector<std::shared_ptr<Entity>> findChildrenByTag(std::string tag, bool recursive = true);

    void setScrollPosition(const mog::Point &position);
    std::shared_ptr<ScrollGroup> clone();

protected:
    ScrollGroup() {}
    
    std::shared_ptr<mog::Group> contentGroup;
    mog::Point velocity = mog::Point::zero;
    bool dragging = false;
    unsigned char scrollFlag = SCROLL_BOTH;
    
    void init(const mog::Size &scrollSize, const mog::Size &contentSize, unsigned char scrollFlag);
    virtual std::shared_ptr<Entity> cloneEntity() override;
};

#endif /* ScrollGroup_h */
