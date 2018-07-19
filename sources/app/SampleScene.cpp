#include "SampleScene.h"

using namespace mog;

void SampleScene::onLoad() {
    auto group = Group::create(true);
    group->setSize(200, 150);
    group->setAnchor(Point::half);
    group->setPosition(this->getApp()->getScreenSize() * 0.5f);

    auto rect = Rectangle::create(group->getSize());
    group->add(rect);
    
    auto label = Label::create("Touch", 30.0f);
    label->setColor(Color::black);
    label->setAnchor(Point::half);
    label->setPosition(group->getSize() * 0.5f);
    group->add(label);

    auto touchListener = TouchEventListener::create();
    touchListener->setOnTapEvent([](const mog::Touch &t, const shared_ptr<mog::Entity> &e) {
        int r = rand() % 256;
        int g = rand() % 256;
        int b = rand() % 256;
        e->setColor(r/255.0f, g/255.0f, b/255.0f);
    });
    group->addTouchEvent(touchListener);

    this->add(group);
}
