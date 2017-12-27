#include <stdlib.h>
#include <typeinfo>
#include <unordered_map>
#include <math.h>
#include "mog/core/opengl.h"
#include "mog/core/Engine.h"
#include "mog/core/opengl.h"
#include "mog/base/Scene.h"
#include "mog/base/AppBase.h"
#include "mog/core/Device.h"
#include "mog/core/AudioPlayer.h"
#include "mog/core/DataStore.h"
#include "mog/core/NativePlugin.h"

using namespace mog;

#if defined(MOG_OSX) || defined(MOG_QT)
inline void glOrthof(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar) {
    glOrtho(left, right, bottom, top, zNear, zFar);
}
#endif

shared_ptr<Engine> Engine::create(const shared_ptr<AppBase> &app) {
    auto engine = shared_ptr<Engine>(new Engine());
    engine->app = app;
    app->setEngine(engine);
    return engine;
}

Engine::Engine() {
    AudioPlayer::initialize();
    NativeCallbackManager::initialize();
    this->renderer = make_shared<Renderer>();
}

Engine::~Engine() {
    if (this->app) {
        this->app->onDispose();
    }
}

void Engine::startEngine() {
    if (this->running) return;
    this->running = true;
    
    this->startTimer();
    this->lastElapsedSec = this->getTimerElapsedSec();
    
    if (!this->stats) {
#ifdef MOG_DEBUG
        bool statsEnable = true;
#else
        bool statsEnable = false;
#endif
        this->stats = MogStats::create(statsEnable);
    }
    if (!this->initialized) {
        this->initParameters();
        this->app->onLoad();
        this->initialized = true;
    }

    AudioPlayer::onResume();
    this->app->onResume();
}

void Engine::stopEngine() {
    if (!this->running) return;
    
    if (this->app) {
        this->app->onPause();
    }
    AudioPlayer::onPause();
    
    this->stopTimer();
    DataStore::save();
    
    this->running = false;
}

void Engine::onDrawFrame(map<unsigned int, TouchInput> touches) {
    if (!this->running) return;

    float elapsed = this->getTimerElapsedSec();
    float delta = elapsed - this->lastElapsedSec;
    this->lastElapsedSec = elapsed;

    this->initScreen();
    this->clearColor();

    this->stats->drawCallCount = 0;
    
    if (this->app) {
        this->app->drawFrame(delta);
    }
    
    this->stats->drawFrame(shared_from_this(), delta);
    
    this->frameCount++;
    
    this->fireTouchListeners(touches);
    NativeCallbackManager::getInstance()->invokeCallback();
    
    this->invokeOnUpdateFunc();
}

void Engine::onLowMemory() {
    if (!this->running) return;
    
    if (this->app) {
        this->app->onLowMemory();
    }
    return;
}

void Engine::initParameters() {
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_DITHER);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
}

void Engine::initScreen() {
    if (!this->displaySizeChanged) return;
    
    glViewport(0, 0, this->displaySize.width, this->displaySize.height);
    glMatrixMode(GL_PROJECTION);
    glOrthof(0, this->displaySize.width, this->displaySize.height, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    this->displaySizeChanged = false;
}

shared_ptr<AppBase> Engine::getApp() {
    return this->app;
}

shared_ptr<NativeObject> Engine::getNativeObject(string name) {
    return this->nativeObjects[name];
}

void Engine::setNativeObject(string name, const shared_ptr<NativeObject> &nObj) {
    this->nativeObjects[name] = nObj;
}

void Engine::removeNativeObject(string name) {
    this->nativeObjects.erase(name);
}

bool Engine::isRunning() {
    return this->running;
}

unsigned long long Engine::getFrameCount() {
    return this->frameCount;
}

Size Engine::getDisplaySize() {
    return this->displaySize;
}

Size Engine::getScreenSize() {
    return this->screenSize;
}

void Engine::setDisplaySize(const Size &size) {
    if (approximately(this->displaySize.width, size.width) &&
            approximately(this->displaySize.height, size.height)) {
        return;
    }
    this->displaySize = size;
    this->displaySizeChanged = true;
}

void Engine::setScreenSizeBasedOnHeight(float height) {
    auto displaySize = this->getDisplaySize();
    float scale = height / displaySize.height;
    float width = displaySize.width * scale;
    this->screenSize = Size(width, height);
}

void Engine::setScreenSizeBasedOnWidth(float width) {
    auto displaySize = this->getDisplaySize();
    float scale = width / displaySize.width;
    float height = displaySize.height * scale;
    this->screenSize = Size(width, height);
}

float Engine::getScreenScale() {
    return this->displaySize.width / this->screenSize.width;
}

Color Engine::getClearColor() {
    return this->color;
}

void Engine::setClearColor(const Color &color) {
    this->color = color;
}

void Engine::clearColor() {
    glClearColor(this->color.r, this->color.g, this->color.b, this->color.a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Engine::onKeyEvent(const KeyEvent &keyEvent) {
    
}

void Engine::setStatsEnable(bool enable) {
    this->stats->setEnable(enable);
}

void Engine::setStatsAlignment(Alignment alignment) {
    this->stats->setAlignment(alignment);
}

shared_ptr<MogStats> Engine::getStats() {
    return this->stats;
}

void Engine::startTimer() {
    if (this->timerRunning) return;
    this->timerStartTime = getTimestamp();
    this->timerRunning = true;
}

void Engine::stopTimer() {
    this->timerBackupTime += this->getTimerElapsed();
    this->timerRunning = false;
}

long long Engine::getTimerElapsed() {
    return getTimestamp() - this->timerStartTime + this->timerBackupTime;
}

float Engine::getTimerElapsedSec() {
    return this->getTimerElapsed() * 0.000001f;
}

void Engine::fireTouchListeners(map<unsigned int, TouchInput> touches) {
    float scale = this->getScreenScale();
    float density = Device::getDeviceDensity();
    float uptime = this->getTimerElapsedSec();
    
    for (auto pair : touches) {
        unsigned int touchId = pair.first;
        auto touchInput = pair.second;
        auto vp = Point(touchInput.x, touchInput.y);
        auto p = vp * density / scale;
        auto touch = Touch(touchId, p, vp, uptime);
        
        if (this->prevTouches.count(touchId) == 0) {
            touch.startTime = uptime;
            touch.startPosition = p;
            touch.startViewPosition = vp;
        } else {
            auto prevTouch = this->prevTouches[touchId];
            touch.startTime = prevTouch.startTime;
            touch.deltaTime = uptime - prevTouch.uptime;
            touch.startPosition = prevTouch.startPosition;
            touch.startViewPosition = prevTouch.startViewPosition;
            touch.deltaPosition = p - prevTouch.position;
            touch.deltaViewPosition = vp - prevTouch.viewPosition;
        }
        
        if (this->touchEnable) {
            bool isSwallowTouches = true;
            
            for (int i = (int)this->touchableEntities.size() - 1; i >= 0; i--) {
                auto entity = this->touchableEntities[i];
                
                if (touchInput.action == TouchAction::TouchDown || touchInput.action == TouchAction::TouchDownUp) {
                    if (isSwallowTouches && entity->contains(p)) {
                        entity->fireTouchBeginEvent(touch);
                        
                        if (!entity->isSwallowTouches()) {
                            isSwallowTouches = false;
                        }
                    }
                }
                if (touchInput.action == TouchAction::TouchMove) {
                    entity->fireTouchMoveEvent(touch);
                }
                if (touchInput.action == TouchAction::TouchUp || touchInput.action == TouchAction::TouchDownUp) {
                    entity->fireTouchEndEvent(touch);
                }
            }
        }
        
        if (touchInput.action == TouchAction::TouchUp || touchInput.action == TouchAction::TouchDownUp) {
            this->prevTouches.erase(touchId);
        } else {
            this->prevTouches[touchId] = touch;
        }
        
        if (!this->multiTouchEnable) {
            break;
        }
    }
    
    this->touchableEntities.clear();
}

void Engine::pushTouchableEntity(const shared_ptr<Entity> &entity) {
    this->touchableEntities.emplace_back(entity);
}

void Engine::setTouchEnable(bool enable) {
    this->touchEnable = enable;
}

void Engine::setMultiTouchEnable(bool enable) {
    this->multiTouchEnable = enable;
}

bool Engine::isTouchEnable() {
    return this->touchEnable;
}

bool Engine::isMultiTouchEnable() {
    return this->multiTouchEnable;
}

unsigned int Engine::registerOnUpdateFunc(function<void(unsigned int funcId)> onUpdateFunc) {
    unsigned int funcId = ++Engine::onUpdateFuncIdCounter;
    this->onUpdateFuncsToAdd[funcId] = onUpdateFunc;
    return funcId;
}

void Engine::removeOnUpdateFunc(unsigned int funcId) {
    this->onUpdateFuncIdsToRemove.emplace_back(funcId);
}

void Engine::invokeOnUpdateFunc() {
    if (this->onUpdateFuncsToAdd.size() > 0) {
        for (const auto &pair : this->onUpdateFuncsToAdd) {
            this->onUpdateFuncs[pair.first] = pair.second;
        }
        this->onUpdateFuncsToAdd.clear();
    }
    if (this->onUpdateFuncIdsToRemove.size() > 0) {
        for (unsigned int funcId : this->onUpdateFuncIdsToRemove) {
            this->onUpdateFuncs.erase(funcId);
        }
        this->onUpdateFuncIdsToRemove.clear();
    }
    
    if (this->onUpdateFuncs.size() == 0) return;
    for (const auto &pair : this->onUpdateFuncs) {
        pair.second(pair.first);
    }
}
