#ifndef Constants_h
#define Constants_h

#define LOG_TAG "MOG"

#define PLATFORM_ANDROID "android"
#define PLATFORM_IOS     "ios"
#define PLATFORM_OSX     "osx"
#define PLATFORM_QT      "qt"

#ifdef MOG_IOS
#define PLATFORM PLATFORM_IOS
#endif
#ifdef MOG_OSX
#define PLATFORM PLATFORM_OSX
#endif
#ifdef MOG_ANDROID
#define PLATFORM PLATFORM_ANDROID
#endif
#ifdef MOG_QT
#define PLATFORM PLATFORM_QT
#endif

#define DEFAULT_FPS 60
#define BASE_SCREEN_HEIGHT 640

#define LOG_DEBUG       1
#define LOG_INFO        2
#define LOG_WARN        3
#define LOG_ERROR       4

#define MAX_TEXTURE_SIZE 4096
#define TEXTURE_MARGIN 1

#define RERENDER_VERTEX     0b00000001
#define RERENDER_COLOR      0b00000010
#define RERENDER_TEXTURE    0b00000100
#define RERENDER_TEX_COORDS 0b00001000
#define RERENDER_ALL        (RERENDER_VERTEX | RERENDER_COLOR | RERENDER_TEXTURE)

#define DIRTY_POSITION      0b00000001
#define DIRTY_SIZE          0b00000010

#ifndef LOG_LEVEL
#ifdef MOG_DEBUG
#define LOG_LEVEL LOG_DEBUG
#else
#define LOG_LEVEL LOG_INFO
#endif
#endif

#include "mog/ConstantsNative.h"
#include "mog/core/mog_functions.h"

#endif /* Constants_h */
