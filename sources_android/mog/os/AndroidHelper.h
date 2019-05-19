#ifndef AndroidHelper_h
#define AndroidHelper_h

#include <jni.h>
#include <memory>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include "mog/core/NativeClass.h"
#include "mog/os/JNILocalFrame.h"

namespace mog {
    class AndroidHelper {
    public:
        static JavaVM *vm;
        static std::shared_ptr<NativeObject> mogActivity;
        static std::shared_ptr<NativeObject> mogAAsetManager;

        static JNIEnv *getEnv() {
            JNIEnv *env;
            vm->GetEnv((void **) &env, JNI_VERSION_1_6);
            return env;
        }

        static AAssetManager *getAssetManager() {
            return AAssetManager_fromJava(getEnv(), (jobject)mogAAsetManager->getValue());
        }

        static std::shared_ptr<NativeObject> getPluginObject(std::string className) {
            JNIEnv *env = getEnv();
            auto lf0 = JNILocalFrame::create(env, 16);
            jobject jActivity = (jobject)mogActivity->getValue();
            jclass jcls = env->GetObjectClass(jActivity);
            jmethodID getPluginObjectId = env->GetMethodID(jcls, "getPluginObject", "(Ljava/lang/String;)Ljava/lang/Object;");
            jstring jclassName = env->NewStringUTF(className.c_str());
            jobject pluginObj = env->CallObjectMethod(jActivity, getPluginObjectId, jclassName);
            return NativeObject::create(pluginObj);
        }

        static void runOnUiThread(std::function<void()> func) {
            auto f = [func](const std::shared_ptr<mog::List> &args) {
                func();
            };
            mogActivity->execute("runOnUiThread", NativeObject::create(f));
        }

        static void runOnGLThread(std::function<void()> func) {
            auto f = [func](const std::shared_ptr<mog::List> &args) {
                func();
            };
            mogActivity->execute("runOnGLThread", NativeObject::create(f));
        }
    };
}

#endif /* AndroidHelper_h */
