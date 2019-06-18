#include <emscripten.h>
#include <emscripten/html5.h>
#include <emscripten/fetch.h>
#include <memory>
#include "mog/core/Engine.h"
#include "app/App.h"
#include "mog/os/MogEngineController.h"

#include "mog/core/FileUtils.h"
#include "mog/core/AudioPlayer.h"
#include "mog/core/AudioPlayerNative.h"

#include <fstream>
#include <iostream>

#define FPS 30
#define WIDTH 320
#define HEIGHT 568

extern "C" {

static std::shared_ptr<mog::MogEngineController> _engineController;

static void initialize_audio() {
    if (auto audioPlayer = mog::AudioPlayer::instance.lock()) {
        audioPlayer->audioPlayerNative->initialize();
        auto channels = audioPlayer->getAllChannels();
        for (auto &pair : channels) {
            pair.second->audioChannelNative->initialize();
        }
    }
}

static EM_BOOL touch_callback(int eventType, const EmscriptenTouchEvent *e, void *userData) {
    mog::MogEngineController *engineController = reinterpret_cast<mog::MogEngineController *>(userData);

    mog::MogEngineController::EMTouchAction action = mog::MogEngineController::EMTouchAction::Down;
    if (eventType == EMSCRIPTEN_EVENT_TOUCHSTART) {
        action = mog::MogEngineController::EMTouchAction::Down;
    } else if (eventType == EMSCRIPTEN_EVENT_TOUCHEND) {
        initialize_audio();
        action = mog::MogEngineController::EMTouchAction::Up;
    } else if (eventType == EMSCRIPTEN_EVENT_TOUCHMOVE) {
        action = mog::MogEngineController::EMTouchAction::Move;
    } else if (eventType == EMSCRIPTEN_EVENT_TOUCHCANCEL) {
        action = mog::MogEngineController::EMTouchAction::Up;
    } else {
        printf("eventType is invalid. (%d)\n", eventType);
        return false;
    }

    for (int i = 0; i < e->numTouches; i++) {
        const EmscriptenTouchPoint *t = &e->touches[i];
        engineController->fireTouchEvent((unsigned int)t->identifier, (float)t->canvasX, (float)t->canvasY, action);
    }

    return true;
}

static EM_BOOL mouse_callback(int eventType, const EmscriptenMouseEvent *e, void* userData) {
    mog::MogEngineController *engineController = reinterpret_cast<mog::MogEngineController *>(userData);

    mog::MogEngineController::EMTouchAction action = mog::MogEngineController::EMTouchAction::Down;
    if (eventType == EMSCRIPTEN_EVENT_MOUSEDOWN) {
        action = mog::MogEngineController::EMTouchAction::Down;
    } else if (eventType == EMSCRIPTEN_EVENT_MOUSEUP) {
        initialize_audio();
        action = mog::MogEngineController::EMTouchAction::Up;
    } else if (eventType == EMSCRIPTEN_EVENT_MOUSEMOVE) {
        action = mog::MogEngineController::EMTouchAction::Move;
    } else {
        printf("eventType is invalid. (%d)\n", eventType);
        return false;
    }

    engineController->fireTouchEvent(0, (float)e->canvasX, (float)e->canvasY, action);

    return true;
}

int main(int argc, char **argv) {
    EmscriptenWebGLContextAttributes attr;
    emscripten_webgl_init_context_attributes(&attr);
    attr.majorVersion = 1;
    attr.minorVersion = 0;
    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE ctx = emscripten_webgl_create_context(0, &attr);
    emscripten_webgl_make_context_current(ctx);
    emscripten_set_canvas_size(WIDTH, HEIGHT);

    _engineController = mog::MogEngineController::create(WIDTH, HEIGHT);
    _engineController->startEngine();

    emscripten_set_mousedown_callback(NULL, _engineController.get(), true, mouse_callback);
    emscripten_set_mouseup_callback(NULL, _engineController.get(), true, mouse_callback);
    emscripten_set_mousemove_callback(NULL, _engineController.get(), true, mouse_callback);

    emscripten_set_touchstart_callback(NULL, _engineController.get(), true, touch_callback);
    emscripten_set_touchend_callback(NULL, _engineController.get(), true, touch_callback);
    emscripten_set_touchmove_callback(NULL, _engineController.get(), true, touch_callback);
    emscripten_set_touchcancel_callback(NULL, _engineController.get(), true, touch_callback);

    // TODO
    //emscripten_set_keydown_callback
    //emscripten_set_keyup_callback

    emscripten_set_main_loop([]() {
        _engineController->drawFrame();
    }, FPS, 1);

    return 0;
}

}
