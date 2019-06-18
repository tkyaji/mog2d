#include "mog/os/MogEngineController.h"
#include "mog/core/plain_objects.h"
#include "app/App.h"
#include <vector>

using namespace mog;

std::shared_ptr<MogEngineController> MogEngineController::create(float width, float height) {
    auto engineController = std::shared_ptr<MogEngineController>(new MogEngineController());
    engineController->init(width, height);
    return engineController;
}

void MogEngineController::init(float width, float height) {
    this->engine = mog::Engine::create(std::make_shared<mog::App>());

    this->width = width;
    this->height = height;
    this->engine->setDisplaySize(mog::Size(width, height), mog::Size(width, height), 2.0f);
}

void MogEngineController::startEngine() {
    this->engine->startEngine();
}

void MogEngineController::stopEngine() {
    this->engine->stopEngine();
}

void MogEngineController::drawFrame() {
    this->engine->onDrawFrame(this->touches);
    this->clearTouchEvent();
}

void MogEngineController::fireTouchEvent(unsigned int touchId, float x, float y, EMTouchAction action) {
    if (x < 0) x = 0;
    if (x > this->width) x = this->width;
    if (y < 0) y = 0;
    if (y > this->height) y = this->height;
    
    switch (action) {
        case EMTouchAction::Down:
            this->touches[touchId] = mog::TouchInput(mog::TouchAction::TouchDown, touchId, x, y);
            break;
            
        case EMTouchAction::Move:
            if (this->touches.count(touchId) == 0 || this->touches[touchId].action == mog::TouchAction::TouchMove) {
                this->touches[touchId] = mog::TouchInput(mog::TouchAction::TouchMove, touchId, x, y);
            }
            break;
            
        case EMTouchAction::Up:
            if (this->touches.count(touchId) > 0 &&
                (this->touches[touchId].action == mog::TouchAction::TouchDown || this->touches[touchId].action == mog::TouchAction::TouchDownUp)) {
                this->touches[touchId] = mog::TouchInput(mog::TouchAction::TouchDownUp, touchId, x, y);
            } else {
                this->touches[touchId] = mog::TouchInput(mog::TouchAction::TouchUp, touchId, x, y);
            }
            break;
    }
}

void MogEngineController::clearTouchEvent() {
    std::vector<unsigned int> touchIdToRemove;
    for (auto &pair : this->touches) {
        if (pair.second.action == mog::TouchAction::TouchUp || pair.second.action == mog::TouchAction::TouchDownUp) {
            touchIdToRemove.emplace_back(pair.first);
        } else {
            pair.second.action = mog::TouchAction::TouchMove;
        }
    }
    for (unsigned int touchId : touchIdToRemove) {
        this->touches.erase(touchId);
    }
}
