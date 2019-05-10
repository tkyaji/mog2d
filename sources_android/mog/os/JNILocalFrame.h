#ifndef JNILocalFrame_h
#define JNILocalFrame_h

#include <jni.h>
#include <memory>

namespace mog {
    class JNILocalFrame {
    public:
        static std::unique_ptr<JNILocalFrame> create(JNIEnv *env, int capacity);

        ~JNILocalFrame();

    private:
        JNILocalFrame(JNIEnv *env, int capacity);

        JNIEnv *env;
    };
}

#endif /* JNILocalFrame_h */
