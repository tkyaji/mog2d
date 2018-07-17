#ifndef Touch_h
#define Touch_h

#include "mog/core/plain_objects.h"

namespace mog {
    
    class Touch {
    public:
        Touch() {}
        Touch(unsigned int touchId, const Point &position, const Point &viewPosition, float uptime) {
            this->touchId = touchId;
            this->position = position;
            this->viewPosition = viewPosition;
            this->uptime = uptime;
        }
        unsigned int touchId = 0;
        float uptime = 0;
        float deltaTime = 0;
        float startTime = 0;
        Point position = Point::zero;
        Point deltaPosition = Point::zero;
        Point startPosition = Point::zero;
        Point viewPosition = Point::zero;
        Point deltaViewPosition = Point::zero;
        Point startViewPosition = Point::zero;
    };
}

#endif /* Touch_h */
