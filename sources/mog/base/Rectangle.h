#ifndef Rectangle_h
#define Rectangle_h

#include <memory>
#include "mog/base/Polygon.h"
#include "mog/core/plain_objects.h"

namespace mog {
    class Rectangle : public Polygon {
        friend class EntityCreator;
    public:
        static std::shared_ptr<Rectangle> create(const Size &size);
        static std::shared_ptr<Rectangle> create(float width, float height);
        std::shared_ptr<Rectangle> clone();
        virtual std::shared_ptr<Collider> getCollider() override;

    protected:
        Rectangle() {}
        
        virtual void init() override;
        virtual std::shared_ptr<Entity> cloneEntity() override;
    };
}

#endif /* Rectangle_h */
