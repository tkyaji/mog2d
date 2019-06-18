#include "app/SampleScene.h"

using namespace mog;

void SampleScene::onLoad() {
    
    auto group = Group::create();
    
    
    auto sprite = Sprite::create("test.png");
    group->add(sprite);

    
    auto label = Label::create("ABCDE", 30.0f, "Montserrat-Bold-Sub.otf");
    label->setPosition(50, 100);
    group->add(label);
    label->setColor(Color::white);
    
    this->add(group);

    
    auto l = TouchEventListener::create();
    l->setOnTapEvent([group](const Touch &t, const std::shared_ptr<Entity> &e) {
        group->setEnableBatching(!group->isEnableBatching());
    });
    sprite->addTouchEvent(l);
    
}
