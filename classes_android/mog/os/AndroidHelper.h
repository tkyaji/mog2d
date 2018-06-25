#ifndef AndroidHelper_h
#define AndroidHelper_h

#include <jni.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include "mog/core/Engine.h"

#define MOG_ACTIVITY "Activity"
#define MOG_AASET_MANAGER "AAssetManager"

namespace mog {
    class AndroidHelper {
    public:
        static JavaVM *vm;
        static weak_ptr<Engine> engine;

        static JNIEnv *getEnv() {
            JNIEnv *env;
            vm->GetEnv((void **) &env, JNI_VERSION_1_6);
            return env;
        }

        static shared_ptr<NativeObject> getActivity() {
            return engine.lock()->getNativeObject(MOG_ACTIVITY);
        }

        static AAssetManager *getAssetManager() {
            auto assetManager = engine.lock()->getNativeObject(MOG_AASET_MANAGER);
            return AAssetManager_fromJava(getEnv(), (jobject)assetManager->getObject());
        }

        static shared_ptr<NativeObject> getPluginObject(string className) {
            JNIEnv *env = getEnv();
            env->PushLocalFrame(16);
            auto activity = engine.lock()->getNativeObject(MOG_ACTIVITY);
            jobject jActivity = (jobject)activity->getObject();
            jclass jcls = env->GetObjectClass(jActivity);
            jmethodID getPluginObjectId = env->GetMethodID(jcls, "getPluginObject", "(Ljava/lang/String;)Ljava/lang/Object;");
            jstring jclassName = env->NewStringUTF(className.c_str());
            jobject pluginObj = env->CallObjectMethod(jActivity, getPluginObjectId, jclassName);
            auto no = NativeObject::create(pluginObj);
            env->PopLocalFrame(NULL);
            return no;
        }

        static void runOnUiThread(std::function<void()> func) {
            auto f = [func](NArg *args, int len) {
                func();
            };
            getActivity()->execute("runOnUiThread", NArg(f));
        }

        static shared_ptr<NativeObject> toBundle(const Dictionary &dict) {
            JNIEnv *env = AndroidHelper::getEnv();
            env->PushLocalFrame(16);

            jclass bundleClass = env->FindClass("android/os/Bundle");
            jmethodID bundleInit = env->GetMethodID(bundleClass, "<init>", "()V");
            jmethodID putStringId = env->GetMethodID(bundleClass, "putString", "(Ljava/lang/String;Ljava/lang/String;)V");
            jmethodID putIntId = env->GetMethodID(bundleClass, "putInt", "(Ljava/lang/String;I)V");
            jmethodID putLongId = env->GetMethodID(bundleClass, "putLong", "(Ljava/lang/String;J)V");
            jmethodID putFloatId = env->GetMethodID(bundleClass, "putFloat", "(Ljava/lang/String;F)V");
            jmethodID putDoubleId = env->GetMethodID(bundleClass, "putDouble", "(Ljava/lang/String;D)V");
            jmethodID putBoolId = env->GetMethodID(bundleClass, "putBoolean", "(Ljava/lang/String;Z)V");
            jobject bundleObj = env->NewObject(bundleClass, bundleInit);

            auto keys = dict.getKeys();
            for (auto &key : keys) {
                env->PushLocalFrame(16);

                jvalue v;
                const char *keyCStr = key.c_str();
                jstring keyStr = env->NewStringUTF(key.c_str());
                auto type = dict.getType(key);

                switch (type) {
                    case DataType::Int: {
                        env->CallVoidMethod(bundleObj, putIntId, keyStr, (jint)dict.get<Int>(key).value);
                        break;
                    }
                    case DataType::Long: {
                        env->CallVoidMethod(bundleObj, putLongId, keyStr, (jlong)dict.get<Long>(key).value);
                        break;
                    }
                    case DataType::Float: {
                        env->CallVoidMethod(bundleObj, putFloatId, keyStr, (jfloat)dict.get<Float>(key).value);
                        break;
                    }
                    case DataType::Double: {
                        env->CallVoidMethod(bundleObj, putDoubleId, keyStr, (jdouble)dict.get<Double>(key).value);
                        break;
                    }
                    case DataType::Bool: {
                        env->CallVoidMethod(bundleObj, putBoolId, keyStr, (jboolean)dict.get<Bool>(key).value);
                        break;
                    }
                    case DataType::String: {
                        const char *s = dict.get<String>(key).value.c_str();
                        jstring str = env->NewStringUTF(s);
                        env->CallVoidMethod(bundleObj, putStringId, keyStr, str);
                        break;
                    }
                    case DataType::Array: {
                        break;
                    }
                    case DataType::Dictionary: {
                        break;
                    }
                    default:
                        break;
                }
                env->PopLocalFrame(NULL);
            }
            auto nativeObject = NativeObject::create(bundleObj);
            env->PopLocalFrame(NULL);

            return nativeObject;
        }

    };
}

#endif /* AndroidHelper_h */
