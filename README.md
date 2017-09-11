# mog2d

mog2d is a simple 2d game engine for iOS / android. It's based on OpenGL 1.x.


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

* mog::Rectangle
* mog::RoundedRectangle
* mog::Circle
* mog::Sprite
* mog::Label
* mog::Group
* mog::BatchingGroup

### Group, BatchingGroup

`mog::Group` class can grouping multiple Entities.

**Ex.**

```cpp
auto group = Group::create();

auto sprite = Sprite::create("test.png");
group->add(sprite);

auto rect = Rectangle::create(Size(100, 100));
group->add(rect);

this->add(group);
```

In addition, `BatchingGroup` combine draw calls into one.











