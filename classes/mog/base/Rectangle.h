#ifndef Rectangle_h
#define Rectangle_h

#include <memory>
#include "mog/base/Polygon.h"
#include "mog/core/plain_objects.h"

namespace mog {
    class Rectangle : public Polygon {
    public:
        static std::shared_ptr<Rectangle> create(const Size &size);
        static std::shared_ptr<Rectangle> create(float width, float height);
        
    protected:
        Rectangle();
        
        void init(const Size &size);
    };
}

#endif /* Rectangle_h */

