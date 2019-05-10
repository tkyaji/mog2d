#include "mog/os/JNILocalFrame.h"
#include "mog/core/mog_functions.h"

using namespace mog;

std::unique_ptr<JNILocalFrame> JNILocalFrame::create(JNIEnv *env, int capacity) {
    return std::unique_ptr<JNILocalFrame>(new JNILocalFrame(env, capacity));
}

JNILocalFrame::JNILocalFrame(JNIEnv *env, int capacity) {
    this->env = env;
    int ret = env->PushLocalFrame(capacity);
    if (ret != 0) {
        LOGE("PushLocalFrame failed. ret=%d\n", ret);
    }
};

JNILocalFrame::~JNILocalFrame() {
    this->env->PopLocalFrame(NULL);
};
