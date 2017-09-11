#ifndef KeyEvent_h
#define KeyEvent_h

namespace mog {
    enum class KeyAction {
        TouchDown,
        TouchMove,
        TouchUp,
        TouchDownUp
    };
    
    
    class KeyEvent {
    public:
        KeyAction action;
        int keyCode;
        
        KeyEvent() { }
        KeyEvent(KeyAction action, int keyCode) {
            this->action = action;
            this->keyCode = keyCode;
        }
    };
}

#endif /* KeyEvent_h */
