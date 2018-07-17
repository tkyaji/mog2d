#ifndef Circle_h
#define Circle_h

#include <memory>
#include "mog/base/Polygon.h"
#include "mog/core/plain_objects.h"
#include "mog/core/Collision.h"

namespace mog {
    class Circle : public Polygon {
    public:
        static shared_ptr<Circle> create(float radius);
        
        float getRadius();
        void setRadius(float radius);
        
    protected:
        float radius = 0;
        
        Circle();
        
        void init(float radius);
        shared_ptr<CIRCLE> getCIRCLE();
        virtual shared_ptr<Collider> getCollider() override;
    };
}

#endif /* Circle_h */
