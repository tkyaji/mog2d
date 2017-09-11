#include "mog/core/mog_functions_native.h"
#include <android/log.h>

void _log(const char* format, va_list args) {
    __android_log_vprint(ANDROID_LOG_DEBUG, "MOG", format, args);
}
