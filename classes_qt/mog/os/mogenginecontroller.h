#ifndef MOGENGINECONTROLLER_H
#define MOGENGINECONTROLLER_H

#include <memory>

namespace mog {
    class Engine;
}

class MogEngineController {
public:
    MogEngineController();

    void startEngine();
    void resize(float width, float height);
    void stopEngine();
    void drawFrame();

private:
    std::shared_ptr<mog::Engine> engine;
};

#endif // MOGENGINECONTROLLER_H
