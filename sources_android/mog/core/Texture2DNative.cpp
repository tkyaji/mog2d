#include <jni.h>
#include <string.h>
#include "mog/core/Texture2DNative.h"
#include "mog/core/Texture2D.h"
#include "mog/core/Engine.h"
#include "mog/os/AndroidHelper.h"
#include "mog/Constants.h"

using namespace mog;

void Texture2DNative::loadFontTexture(Texture2D *tex2d, const char *text, float fontSize, const char *fontFace, float height) {
    if (strlen(text) == 0) return;

    JNIEnv* env = AndroidHelper::getEnv();
    jobject activity = (jobject)AndroidHelper::getActivity()->getValue();
//    vm->AttachCurrentThread(&env, NULL);

    env->PushLocalFrame(16);

    jclass jcls = env->FindClass("org/mog2d/TextBitmap");
    jmethodID methodId = env->GetStaticMethodID(jcls, "createFontTexture", "(Landroid/app/Activity;Ljava/lang/String;FLjava/lang/String;ZZI)Lorg/mog2d/TextBitmap$Result;");
    jstring textStr = env->NewStringUTF(text);
    jobject result = env->CallStaticObjectMethod(jcls, methodId, activity, textStr,
                                                 (jfloat)fontSize, env->NewStringUTF(fontFace),
                                                 JNI_FALSE, JNI_FALSE, (int)(height + 0.5f));

    jclass jrcls = env->GetObjectClass(result);
    jfieldID bytesId = env->GetFieldID(jrcls, "bytes", "[B");
    jbyteArray byteArr = (jbyteArray)env->GetObjectField(result, bytesId);

    jfieldID widthId = env->GetFieldID(jrcls, "width", "I");
    int textWidth = env->GetIntField(result, widthId);

    jfieldID  heightId = env->GetFieldID(jrcls, "height", "I");
    int textHeight = env->GetIntField(result, heightId);

    jbyte *bytes = env->GetByteArrayElements(byteArr, NULL);
    int length = (int)env->GetArrayLength(byteArr);

    tex2d->data = (unsigned char *)mogmalloc(length * sizeof(unsigned char));
    memcpy(tex2d->data, (unsigned char *)bytes, length * sizeof(unsigned char));

    tex2d->width = textWidth;
    tex2d->height = textHeight;
    tex2d->dataLength = length;
    tex2d->textureType = TextureType ::RGBA;

    // remove premultiplied alpha
    GLubyte rgb[3] = {255, 255, 255};
    for (int i = 0; i < tex2d->width * tex2d->height; i++) {
        memcpy(&tex2d->data[i * 4], rgb, sizeof(GLubyte) * 3);
    }

    env->ReleaseByteArrayElements(byteArr, bytes, JNI_ABORT);

    env->PopLocalFrame(NULL);

//    vm->DetachCurrentThread();
}

string Texture2DNative::getLocalizedTextNative(const char *textKey, va_list args) {
    JNIEnv* env = AndroidHelper::getEnv();
    jobject activityObj = (jobject)AndroidHelper::getActivity()->getValue();
//    vm->AttachCurrentThread(&env, NULL);

    env->PushLocalFrame(16);

    jclass activityClass = env->GetObjectClass(activityObj);

    jmethodID getResourcesMethodId = env->GetMethodID(activityClass, "getResources", "()Landroid/content/res/Resources;");
    jobject resourcesObj = env->CallObjectMethod(activityObj, getResourcesMethodId);
    jclass  resourcesClass = env->GetObjectClass(resourcesObj);

    jmethodID getPackageNameMethodId = env->GetMethodID(activityClass, "getPackageName", "()Ljava/lang/String;");
    jobject packageNameObj = env->CallObjectMethod(activityObj, getPackageNameMethodId);

    jmethodID getIdentifierMethodId = env->GetMethodID(resourcesClass, "getIdentifier", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)I");
    int stringId = env->CallIntMethod(resourcesObj, getIdentifierMethodId, env->NewStringUTF(textKey), env->NewStringUTF("string"), packageNameObj);

    string ret = "";
    if (stringId != 0) {
        jmethodID getStringMethodId = env->GetMethodID(resourcesClass, "getString", "(I)Ljava/lang/String;");
        jstring localizedJStr = (jstring)env->CallObjectMethod(resourcesObj, getStringMethodId, stringId);
        int strLen = env->GetStringLength(localizedJStr);

        jboolean b;
        const char *localizedStr = env->GetStringUTFChars(localizedJStr, &b);

        char *str = new char[strlen(localizedStr) + 4096];
        vsprintf(str, localizedStr, args);

        ret = string(str);
        delete[] str;
    }

    env->PopLocalFrame(NULL);
//    vm->DetachCurrentThread();

    return ret;
}
