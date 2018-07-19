# mog2d

mog2d is a simple 2d game engine for iOS / android. It's based on OpenGL ES2.


## Getting Start

### 1. Open `Mog2d-iOS` in Xcode or open `Mog2d-Android` in AndroidStudio. 


### 2. Create first scene class.

**FirstScene.h**
```cpp
#include "mog/mog.h"

using namespace mog;

class FirstScene : public Scene {
    virtual void onLoad() override;
};
```

**FirstScene.cpp**
```cpp
#include "FirstScene.h"

void FirstScene::onLoad() {
    
}
```

### 3. Add a subclass of `mog::Entitiy` to the created class.

**FirstScene.cpp**
```cpp
#include "FirstScene.h"

void FirstScene::onLoad() {
    auto sprite = Sprite::create("test.png");
    this->add(sprite);
}
```

## Subclass of `mog::Entity`

* mog::Sprite
* mog::Slice9Sprite
* mog::SpriteSheet
* mog::Label
* mog::Rectangle
* mog::RoundedRectangle
* mog::Circle
* mog::Triangle
* mog::Group

### Group

`mog::Group` class can grouping multiple Entities.

**Ex.**

```cpp
// If you specify true for the argument, combine draw calls into one.
auto group = Group::create(true);

auto sprite = Sprite::create("test.png");
group->add(sprite);

auto rect = Rectangle::create(Size(100, 100));
group->add(rect);

this->add(group);
```









