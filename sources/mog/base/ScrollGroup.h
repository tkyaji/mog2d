#ifndef ScrollGroup_h
#define ScrollGroup_h

#include "mog/base/Group.h"

#define SCROLL_VERTICAL     0x0001
#define SCROLL_HORIZONTAL   0x0010
#define SCROLL_BOTH         0x0011

namespace mog {
    
    class ScrollGroup : public Group {
        friend class EntityCreator;
    public:
        static std::shared_ptr<ScrollGroup> create(const Size &contentSize, unsigned char scrollFlag = SCROLL_BOTH);
        
        virtual void updateFrame(const std::shared_ptr<Engine> &engine, float delta, float *parentMatrix, float *parentRendererMatrix, unsigned char parentDirtyFlag) override;
        virtual void drawFrame(float delta, const std::map<unsigned int, TouchInput> &touches) override;
        virtual void add(const std::shared_ptr<Entity> &entity) override;
        virtual void remove(const std::shared_ptr<Entity> &entity) override;
        virtual void removeAll() override;
        std::vector<std::shared_ptr<Entity>> getChildEntities();
        std::shared_ptr<Entity> findChildByName(std::string name, bool recursive = true);
        std::shared_ptr<Entity> findFirstChildByTag(std::string tag, bool recursive = true);
        std::vector<std::shared_ptr<Entity>> findChildrenByTag(std::string tag, bool recursive = true);
        
        void setScrollPosition(const Point &position);
        std::shared_ptr<ScrollGroup> clone();
        
    protected:
        ScrollGroup() {}
        
        std::shared_ptr<Group> contentGroup;
        Size contentSize = Size::zero;
        Point velocity = Point::zero;
        bool dragging = false;
        unsigned char scrollFlag = SCROLL_BOTH;
        
        void init(const Size &scrollSize, const Size &contentSize, unsigned char scrollFlag);
        virtual void init() override;
        virtual std::shared_ptr<Entity> cloneEntity() override;
    };
    
}

#endif /* ScrollGroup_h */
