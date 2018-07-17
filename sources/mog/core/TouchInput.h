#ifndef TouchInput_h
#define TouchInput_h

namespace mog {
    enum class TouchAction {
        TouchDown,
        TouchMove,
        TouchUp,
        TouchDownUp
    };
    
    
    class TouchInput {
    public:
        TouchAction action;
        unsigned int touchId;
        float x;
        float y;
        
        TouchInput() { }
        TouchInput(TouchAction action, int touchId, float x, float y) {
            this->action = action;
            this->touchId = touchId;
            this->x = x;
            this->y = y;
        }
    };
}

#endif /* TouchInput_h */
