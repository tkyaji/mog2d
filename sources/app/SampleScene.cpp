#include "app/SampleScene.h"

using namespace mog;

void SampleScene::onLoad() {
    auto label = Label::create("Hello Mog2d!", 40.0f);
    this->add(label);
}
