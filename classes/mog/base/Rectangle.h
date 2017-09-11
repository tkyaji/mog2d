#ifndef Rectangle_h
#define Rectangle_h

#include <memory>
#include "mog/base/Polygon.h"
#include "mog/core/plain_objects.h"

namespace mog {
    class Rectangle : public Polygon {
    public:
        static shared_ptr<Rectangle> create(const Size &size);
        
        shared_ptr<Rectangle> clone();
        virtual shared_ptr<Entity> cloneEntity() override;
        
    protected:
        Rectangle();
        
        void init(const Size &size);
    };
}

#endif /* Rectangle_h */
