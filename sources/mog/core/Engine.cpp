#include <stdlib.h>
#include <typeinfo>
#include <unordered_map>
#include <math.h>
#include "mog/Constants.h"
#include "mog/core/opengl.h"
#include "mog/core/Engine.h"
#include "mog/base/Entity.h"
#include "mog/base/Scene.h"
#include "mog/base/AppBase.h"
#include "mog/core/Screen.h"
#include "mog/core/DataStore.h"

using namespace mog;

std::weak_ptr<Engine> Engine::instance;

std::shared_ptr<Engine> Engine::create(const std::shared_ptr<AppBase> &app) {
    auto engine = std::shared_ptr<Engine>(new Engine());
    Engine::instance = engine;
    engine->app = app;
    engine->screen = Screen::create();
    engine->audioPlayer = AudioPlayer::create();
    app->setEngine(engine);
    return engine;
}

std::shared_ptr<Engine> Engine::getInstance() {
    return Engine::instance.lock();
}

Engine::Engine() {
    this->renderer = Renderer::create();
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
    this->releaseAllBuffers();

    this->running = false;
}

void Engine::onDrawFrame(const std::map<unsigned int, TouchInput> &touches) {
    if (!this->running) return;

    if (this->dirtyFlag == DIRTY_ALL) {
        this->initParameters();
    }
    
    float elapsed = this->getTimerElapsedSec();
    float delta = elapsed - this->lastElapsedSec;
    this->lastElapsedSec = elapsed;

    this->initScreen();
    this->clearColor();

    this->stats->drawCallCount = 0;
    
    if (this->app) {
        this->app->drawFrame(delta, touches, this->dirtyFlag);
    }
    
    this->stats->drawFrame(delta, this->dirtyFlag);
    
    this->frameCount++;
    
    this->fireTouchListeners(touches);
    
    this->invokeOnUpdateFunc();
    
    this->dirtyFlag = 0;
}

void Engine::onLowMemory() {
    if (!this->running) return;
    
    if (this->app) {
        this->app->onLowMemory();
    }
    return;
}

void Engine::initParameters() {
    glEnable(GL_BLEND);
    glDisable(GL_CULL_FACE);
}

void Engine::initScreen() {
    auto displaySize = this->screen->getDisplaySize();
    glViewport(0, 0, displaySize.width, displaySize.height);
}

std::shared_ptr<AppBase> Engine::getApp() {
    return this->app;
}

bool Engine::isRunning() {
    return this->running;
}

unsigned long long Engine::getFrameCount() {
    return this->frameCount;
}

void Engine::setDisplaySize(const Size &displaySize, const Size &viewSize, float deviceDensity) {
    this->screen->setDisplaySize(displaySize, viewSize, deviceDensity);
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

std::shared_ptr<MogStats> Engine::getStats() {
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

void Engine::fireTouchListeners(std::map<unsigned int, TouchInput> touches) {
    float uptime = this->getTimerElapsedSec();
    
    for (auto pair : touches) {
        unsigned int touchId = pair.first;
        auto touchInput = pair.second;
        auto p = Point(touchInput.x, touchInput.y);
        auto touch = Touch(touchId, p, uptime);
        
        if (this->prevTouches.count(touchId) == 0) {
            touch.startTime = uptime;
            touch.startPosition = p;
            
            if (!this->multiTouchEnable && this->prevTouches.size() > 0) {
                continue;
            }
        } else {
            auto prevTouch = this->prevTouches[touchId];
            touch.startTime = prevTouch.startTime;
            touch.deltaTime = uptime - prevTouch.uptime;
            touch.startPosition = prevTouch.startPosition;
            touch.deltaPosition = p - prevTouch.position;
        }
        
        if (this->touchEnable) {
            bool isSwallowTouches = false;
            
            for (int i = (int)this->touchableEntities.size() - 1; i >= 0; i--) {
                auto entity = this->touchableEntities[i];
                
                if (touchInput.action == TouchAction::TouchDown || touchInput.action == TouchAction::TouchDownUp) {
                    if (!isSwallowTouches && entity->contains(p)) {
                        entity->fireTouchBeginEvent(touch);
                        
                        if (entity->isSwallowTouches()) {
                            isSwallowTouches = true;
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

void Engine::pushTouchableEntity(const std::shared_ptr<Entity> &entity) {
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

void Engine::releaseAllBuffers() {
    this->dirtyFlag = DIRTY_ALL;
    Texture2D::releaseAllBufferes();
    Renderer::releaseAllBufferes();
    Shader::releaseAllBufferes();
}

unsigned int Engine::registerOnUpdateFunc(std::function<void(unsigned int funcId)> onUpdateFunc) {
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
