#ifndef Transform_h
#define Transform_h

#include <memory>
#include "mog/core/plain_objects.h"

using namespace std;

namespace mog {
    class Transform : public enable_shared_from_this<Transform> {
    public:
        Transform() {
        }
        
        Point anchor = Point::zero;
        Point position = Point::zero;
        Size size = Size::zero;
        Point scale = Point::one;
        float rotation = 0;
        Color color = Color::white;
        float screenScale;
        
        void copyFrom(const shared_ptr<Transform> &src) {
            this->anchor = src->anchor;
            this->position = src->position;
            this->size = src->size;
            this->scale = src->scale;
            this->rotation = src->rotation;
            this->color = src->color;
            this->screenScale = src->screenScale;
        }
    };
}


#endif /* Transform_h */
