#include <stdlib.h>
#include <typeinfo>
#include <unordered_map>
#include <math.h>
#include "mog/core/opengl.h"
#include "mog/core/Engine.h"
#include "mog/core/opengl.h"
#include "mog/base/Scene.h"
#include "mog/base/AppBase.h"
#include "mog/core/AudioPlayer.h"
#include "mog/core/DataStore.h"
#include "mog/core/NativePlugin.h"
#include "mog/os/MogStatsView.h"

using namespace mog;

#ifdef MOG_OSX
inline void glOrthof(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar) {
    glOrtho(left, right, bottom, top, zNear, zFar);
}
#endif

Engine *Engine::instance;
unsigned int Engine::onUpdateFuncIdCounter = 0;

Engine *Engine::getInstance() {
    return Engine::instance;
}

Engine *Engine::initInstance() {
    if (Engine::instance == nullptr) {
        Engine::instance = new Engine();
    }
    return Engine::instance;
}

Engine::Engine() {
    this->touchEventManager = TouchEventManager::initialize();
    AudioPlayer::initialize();
    NativeCallbackManager::initialize();
    
    this->renderer = make_shared<Renderer>();
    
#ifdef MOG_DEBUG
    this->statsViewEnable = true;
#else
    this->statsViewEnable = false;
#endif
}

void Engine::initEngine(bool loadApp) {
    this->initParameters();
    
    if (this->statsViewEnable) {
        MogStatsView::setEnable(true);
    }

    if (this->app) {
        if (loadApp) {
            this->app->onLoad();

        } else {
            auto scene = this->app->getCurrentScene();
            if (scene) {
                scene->getRootGroup()->setReRenderFlagToChild(RERENDER_ALL);
            }
        }
    }
}

void Engine::terminateEngine() {
    if (this->app) {
        this->app->onDispose();
    }
    this->app = nullptr;
}

void Engine::startEngine() {
    if (this->running) return;
    this->running = true;
    
    this->startTimer();
    this->lastElapsedSec = this->getTimerElapsedSec();
    
    AudioPlayer::onResume();
    if (this->app) {
        this->app->onResume();
    }
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

#ifdef MOG_DEBUG
    Renderer::drawCallCounter = 0;
#endif

    if (this->app) {
        this->app->drawFrame(delta);
    }
    
#ifdef MOG_DEBUG
    if (this->statsViewEnable && this->frameCount % 4 == 0) {
        MogStatsView::setStats(delta, Renderer::drawCallCounter, Entity::instanceNumCounter);
    }
#endif
    
    this->frameCount++;
    
    TouchEventManager::getInstance()->fireTouchListeners(touches);
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

void Engine::setApp(const shared_ptr<AppBase> &app) {
    this->app = app;
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

void Engine::setDisplaySize(const Size &size, float deviceDensity) {
    this->displaySize = size;
    this->deviceDensity = deviceDensity;
    
    int scale = (int)ceil(deviceDensity * 10.0f);
    if (scale > 30) {
        this->density = Density::x4_0;
    } else if (scale > 20) {
        this->density = Density::x3_0;
    } else if (scale > 15) {
        this->density = Density::x2_0;
    } else if (scale > 10) {
        this->density = Density::x1_5;
    } else {
        this->density = Density::x1_0;
    }
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
    float scale = displaySize.width / width;
    float height = displaySize.height * scale;
    this->screenSize = Size(width, height);
}

float Engine::getScreenScale() {
    return this->displaySize.width / this->screenSize.width;
}

float Engine::getDeviceDensity() {
    return this->deviceDensity;
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

Density Engine::getDensity() {
    return this->density;
}

void Engine::onKeyEvent(const KeyEvent &keyEvent) {
    
}

void Engine::setStatsViewEnable(bool enable) {
    MogStatsView::setEnable(enable);
    this->statsViewEnable = enable;
}

void Engine::setStatsViewAlignment(Alignment alignment) {
    MogStatsView::setAlignment(alignment);
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
