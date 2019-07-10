#include <android/asset_manager_jni.h>
#include <jni.h>
#include "mog/core/Engine.h"
#include "app/App.h"
#include "mog/Constants.h"
#include "mog/core/NativeClass.h"
#include "mog/os/AndroidHelper.h"
#include "mog/core/mogmalloc.h"

using namespace mog;

class MogRenderer {
public:
    static MogRenderer *initialize() {
        MogRenderer::instance = new MogRenderer();
        return MogRenderer::instance;
    }
    static MogRenderer *getInstance() {
        return MogRenderer::instance;
    }
    
    void onCreate(JNIEnv* env, jobject obj, jobject jActivity, jobject jAssetManager) {
        this->engine = Engine::create(std::make_shared<mog::App>());
        
        JavaVM *vm;
        env->GetJavaVM(&vm);
        AndroidHelper::vm = vm;
        AndroidHelper::mogActivity = NativeObject::create(jActivity);
        AndroidHelper::mogAAsetManager = NativeObject::create(jAssetManager);
        this->removeTouchIds.reserve(8);
    }
    
    void onDestroy(JNIEnv* env, jobject obj) {
    }
    
    void onPause(JNIEnv* env, jobject obj) {
        this->engine->stopEngine();
    }
    
    void onResume(JNIEnv* env, jobject obj) {
        if (!this->surfaceCreated) return;
        this->engine->startEngine();
    }
    
    void onStart(JNIEnv* env, jobject obj) {
    }
    
    void onStop(JNIEnv* env, jobject obj) {
    }
    
    void onDrawFrame(JNIEnv* env, jobject obj) {
        this->engine->onDrawFrame(this->touches);
        
        if (this->removeTouchIds.size() > 0) {
            for (unsigned int touchId : this->removeTouchIds) {
                this->touches.erase(touchId);
            }
            this->removeTouchIds.clear();
        }
    }
    
    void onSurfaceCreated(JNIEnv* env, jobject obj) {
        mogmalloc_initialize();
        this->surfaceCreated = true;
    }
    
    void onSurfaceChanged(JNIEnv* env, jobject obj, jint w, jint h, int vw, int vh, float scaleFactor) {
        this->scaleFactor = scaleFactor;
        this->engine->setDisplaySize(Size(w, h), Size(vw, vh), scaleFactor);
        this->engine->startEngine();
    }
    
    void onTouchEvent(JNIEnv* env, jobject obj, jint pointerId, jint touchAction, jfloat x, jfloat y) {
        NativeTouchAction action = (NativeTouchAction)touchAction;
        
        switch (action) {
            case NativeTouchAction::Down:
            case NativeTouchAction::Move:
                if (this->touches.count(pointerId) == 0) {
                    this->touches[pointerId] = TouchInput(TouchAction::TouchDown, pointerId, x, y);
                } else {
                    this->touches[pointerId] = TouchInput(TouchAction::TouchMove, pointerId, x, y);
                }
                break;
                
            case NativeTouchAction::Up:
                if (this->touches.count(pointerId) == 0) {
                    this->touches[pointerId] = TouchInput(TouchAction::TouchDownUp, pointerId, x, y);
                } else {
                    this->touches[pointerId] = TouchInput(TouchAction::TouchUp, pointerId, x, y);
                }
                this->removeTouchIds.emplace_back(pointerId);
                break;
        }
    }

private:
    enum NativeTouchAction {
        Down = 1,
        Up = 2,
        Move = 3,
    };
    
    static MogRenderer *instance;
    std::shared_ptr<mog::Engine> engine;
    std::map<unsigned int, TouchInput> touches;
    std::vector<unsigned int> removeTouchIds;
    bool surfaceCreated = false;
    float scaleFactor = 1.0f;
};
MogRenderer *MogRenderer::instance;


extern "C" {
JNIEXPORT void JNICALL Java_org_mog2d_MogJniBridge_onSurfaceCreated(JNIEnv* env, jobject obj) {
    MogRenderer::getInstance()->onSurfaceCreated(env, obj);
}

JNIEXPORT void JNICALL Java_org_mog2d_MogJniBridge_onSurfaceChanged(JNIEnv* env, jobject obj, jint w, jint h, jint vw, jint vh, jfloat scaleFactor) {
    MogRenderer::getInstance()->onSurfaceChanged(env, obj, w, h, vw, vh, scaleFactor);
}

JNIEXPORT void JNICALL Java_org_mog2d_MogJniBridge_onDrawFrame(JNIEnv* env, jobject obj) {
    MogRenderer::getInstance()->onDrawFrame(env, obj);
}

JNIEXPORT void JNICALL Java_org_mog2d_MogJniBridge_onTouchEvent(JNIEnv* env, jobject obj, jint pointerId, jint touchAction, jfloat x, jfloat y) {
    MogRenderer::getInstance()->onTouchEvent(env, obj, pointerId, touchAction, x, y);
}

JNIEXPORT jint JNICALL Java_org_mog2d_MogJniBridge_getDefaultFps(JNIEnv* env, jobject obj) {
    return DEFAULT_FPS;
}


JNIEXPORT void JNICALL Java_org_mog2d_MogJniBridge_onCreate(JNIEnv* env, jobject obj, jobject jActivity, jobject jAssetManager) {
    MogRenderer::initialize()->onCreate(env, obj, jActivity, jAssetManager);
}

JNIEXPORT void JNICALL Java_org_mog2d_MogJniBridge_onDestroy(JNIEnv* env, jobject obj) {
    MogRenderer::getInstance()->onDestroy(env, obj);
}

JNIEXPORT void JNICALL Java_org_mog2d_MogJniBridge_onPause(JNIEnv* env, jobject obj) {
    MogRenderer::getInstance()->onPause(env, obj);
}

JNIEXPORT void JNICALL Java_org_mog2d_MogJniBridge_onResume(JNIEnv* env, jobject obj) {
    MogRenderer::getInstance()->onResume(env, obj);
}

JNIEXPORT void JNICALL Java_org_mog2d_MogJniBridge_onStart(JNIEnv* env, jobject obj) {
    MogRenderer::getInstance()->onStart(env, obj);
}

JNIEXPORT void JNICALL Java_org_mog2d_MogJniBridge_onStop(JNIEnv* env, jobject obj) {
    MogRenderer::getInstance()->onStop(env, obj);
}

}
