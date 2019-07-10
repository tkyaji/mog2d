#include "app/SampleScene.h"

using namespace mog;

void SampleScene::onLoad() {
    
    auto group = Group::create();
    group->setSize(Screen::getSize() * 0.5f);
    group->setAnchor(1.0f, 1.0f);
    group->setPivot(1.0f, 1.0f);

    
    auto sprite = Sprite::create("test.png");
    group->add(sprite);
    
    auto label = Label::create("test", 30);
    group->add(label);

    sprite->setAnchor(1.0f, 1.0f);
    sprite->setPivot(1.0f, 1.0f);

    this->add(group);
    
}
