#include <memory>
#include <map>
#include "mog/core/Engine.h"
#include "mog/core/TouchInput.h"

namespace mog {
    class MogEngineController {
    public:
        enum class EMTouchAction {
            Down,
            Up,
            Move,
        };

        static std::shared_ptr<MogEngineController> create(float width, float height);
        void startEngine();
        void stopEngine();
        void drawFrame();
        void fireTouchEvent(unsigned int touchId, float x, float y, EMTouchAction action);
        void clearTouchEvent();

    private:
        std::shared_ptr<Engine> engine;
        std::map<unsigned int, mog::TouchInput> touches;
        float width;
        float height;

        void init(float width, float height);
    };
}