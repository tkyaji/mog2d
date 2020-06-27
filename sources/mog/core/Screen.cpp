#include "mog/core/Screen.h"
#include "mog/Constants.h"
#include <math.h>

using namespace mog;

const Density Density::x1_0 = Density(Type::x1_0);
const Density Density::x1_5 = Density(Type::x1_5);
const Density Density::x2_0 = Density(Type::x2_0);
const Density Density::x3_0 = Density(Type::x3_0);
const Density Density::x4_0 = Density(Type::x4_0);

#ifdef MOG_IOS
std::vector<Density> Density::allDensities = {Density::x1_0, Density::x2_0, Density::x3_0};
#elif defined(MOG_ANDROID)
std::vector<Density> Density::allDensities = {Density::x1_0, Density::x1_5, Density::x2_0, Density::x3_0, Density::x4_0};
#elif defined(MOG_EMSCRIPTEN)
std::vector<Density> Density::allDensities = {Density::x1_0, Density::x1_5, Density::x2_0, Density::x3_0, Density::x4_0};
#else
std::vector<Density> Density::allDensities = {Density::x1_0};
#endif

Density Density::getDensity(float deviceDensity) {
    int scale = (int)ceil(deviceDensity * 10.0f);
    auto density = Density::x1_0;
    if (scale > 30) {
        density = Density::x4_0;
    } else if (scale > 20) {
        density = Density::x3_0;
    } else if (scale > 15) {
        density = Density::x2_0;
    } else if (scale > 10) {
        density = Density::x1_5;
    }
    return density;
}

Density::Density(Type type) {
    switch (type) {
        case Type::x1_0:
            this->value = 1.0f;
            this->directory = "@1x";
            break;
        case Type::x1_5:
            this->value = 1.5f;
            this->directory = "@1_5x";
            break;
        case Type::x2_0:
            this->value = 2.0f;
            this->directory = "@2x";
            break;
        case Type::x3_0:
            this->value = 3.0f;
            this->directory = "@3x";
            break;
        case Type::x4_0:
            this->value = 4.0f;
            this->directory = "@4x";
            break;
    }
    this->type = type;
}


std::weak_ptr<Screen> Screen::instance;

std::shared_ptr<Screen> Screen::create() {
    auto screen = std::shared_ptr<Screen>(new Screen());
    Screen::instance = screen;
    return screen;
}

Size Screen::getSize() {
    if (auto screen = instance.lock()) {
        return screen->screenSize;
    }
    return Size::zero;
}

Size Screen::getDisplaySize() {
    if (auto screen = instance.lock()) {
        return screen->displaySize;
    }
    return Size::zero;
}

Size Screen::getViewSize() {
    if (auto screen = instance.lock()) {
        return screen->viewSize;
    }
    return Size::zero;
}

Density Screen::getDensity() {
    if (auto screen = instance.lock()) {
        return screen->density;
    }
    return Density::x1_0;
}

float Screen::getDeviceDensityValue() {
    if (auto screen = instance.lock()) {
        return screen->deviceDensity;
    }
    return 1.0f;
}

float Screen::getScreenScale() {
    if (auto screen = instance.lock()) {
        return screen->displaySize.width / screen->screenSize.width;
    }
    return 1.0f;
}

void Screen::setDisplaySize(const Size &displaySize, const Size &viewSize, float deviceDensity){
    if (approximately(this->displaySize.width, displaySize.width) &&
        approximately(this->displaySize.height, displaySize.height)) {
        return;
    }
    
    this->displaySize = displaySize;
    this->viewSize = viewSize;
    this->deviceDensity = deviceDensity;
    this->density = Density::getDensity(deviceDensity);
    this->setScreenSize(BASE_SCREEN_WIDTH, BASE_SCREEN_HEIGHT);
}


void Screen::setSize(float width, float height) {
    if (auto screen = instance.lock()) {
        screen->setScreenSize(width, height);
    }
}

void Screen::setSizeBasedOnHeight(float height) {
    if (auto screen = instance.lock()) {
        float width = (screen->displaySize.width / screen->displaySize.height) * height;
        screen->setScreenSize(width, height);
    }
}

void Screen::setSizeBasedOnWidth(float width) {
    if (auto screen = instance.lock()) {
        float height = (screen->displaySize.height / screen->displaySize.width) * width;
        screen->setScreenSize(width, height);
    }
}

void Screen::setScreenSize(float width, float height) {
    if (width == 0) {
        float scale = height / this->displaySize.height;
        width = this->displaySize.width * scale;
    }
    if (height == 0) {
        float scale = width / this->displaySize.width;
        height = this->displaySize.height * scale;
    }
    this->screenSize = Size(width, height);
    this->invokeCallback();
}

void Screen::setResizeCallback(std::function<void()> callback) {
    this->resizeCallback = callback;
}

void Screen::invokeCallback() {
    if (this->resizeCallback) {
        this->resizeCallback();
    }
}
