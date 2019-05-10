#include "app/App.h"
#include "app/SampleScene.h"

using namespace mog;

void App::onLoad() {
    this->loadScene(std::make_shared<SampleScene>());
}
