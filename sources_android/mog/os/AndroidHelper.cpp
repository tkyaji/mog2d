#include "mog/os/AndroidHelper.h"

JavaVM *mog::AndroidHelper::vm = nullptr;
std::shared_ptr<mog::NativeObject> mog::AndroidHelper::mogActivity = nullptr;
std::shared_ptr<mog::NativeObject> mog::AndroidHelper::mogAAsetManager = nullptr;
