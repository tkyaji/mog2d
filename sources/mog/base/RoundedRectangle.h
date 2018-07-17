#ifndef RoundedRectangle_h
#define RoundedRectangle_h

#include <memory>
#include <unordered_map>
#include "mog/base/Polygon.h"
#include "mog/core/plain_objects.h"

namespace mog {
    class RoundedRectangle : public Polygon {
    public:
        static shared_ptr<RoundedRectangle> create(const Size &size, float cornerRadius);
        static shared_ptr<RoundedRectangle> create(float width, float height, float cornerRadius);

        float getCornerRadius();
        
    protected:
        float cornerRadius = 0;
        
        RoundedRectangle();
        
        void init(const Size &size, float cornerRadius);
    };
}

#endif /* RoundedRectangle_h */
