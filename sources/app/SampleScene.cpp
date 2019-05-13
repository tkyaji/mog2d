#include "app/SampleScene.h"

using namespace mog;

void SampleScene::onLoad() {
    
    auto scrollSize = Screen::getSize()* 0.8f;
    auto contentSize = Screen::getSize();
    auto scrollGroup = ScrollGroup::create(scrollSize, contentSize, SCROLL_VERTICAL);
    scrollGroup->setSize(scrollSize);
    scrollGroup->setAnchor(Point::half);
    scrollGroup->setPosition(Screen::getSize() * 0.5f);
    
    auto rect = Rectangle::create(contentSize);
    rect->setColor(Color::blue);
    scrollGroup->add(rect);
    auto circle = Circle::create(100.0f);
    circle->setAnchor(Point::half);
    circle->setPosition(contentSize * 0.5f);
    scrollGroup->add(circle);
    
    scrollGroup->setScrollPosition((scrollSize - contentSize) * 0.5f);

    this->add(scrollGroup);
    
    
    auto sprite = Sprite::create("test.png");
    this->add(sprite);

    
    auto label = Label::create("ABCDE", 30.0f);
    label->setPosition(50, 100);
    this->add(label);
    label->setColor(Color::black);

    auto label2 = Label::create("ABCDE", 30.0f, TextDrawingMode::Stroke, 1.0f);
    label2->setPosition(50, 100);
    this->add(label2);
    label2->setColor(Color::white);

    
    mog::AudioPlayer::createChannel("bgm")->play("bgm_game01.ogg");
    
//    auto sprite = Sprite::create("test.png");
//    this->add(sprite);
    
    
    /*
    std::vector<Point> points = {
        Point(10.0f, 10.0f),
        Point(80.0f, 60.0f),
        Point(10.0f, 110.0f),
        Point(80.0f, 160.0f),
    };
    auto line = Line::create(points, 10.0f, LineType::LineLoop);
    this->add(line);
     */
    
    /*
    auto sprite = Sprite::create("bird_ss.png");
    auto spriteSheet = SpriteSheet::create(sprite, Size(299.0f, 201.0f));
    this->add(spriteSheet);
    
    spriteSheet->startAnimation(0.2f);
     */

    
//    auto circle = Circle::create(50);
//    this->add(circle);
    
    
    
    
    
    
//    auto sprite = Sprite::create("ninja.png");
//    this->add(sprite);
    
    
    /*
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
     */
}
