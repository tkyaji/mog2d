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
#ifdef MOG_ANDROID
#define PLATFORM PLATFORM_ANDROID
#endif
#ifdef MOG_EMSCRIPTEN
#define PLATFORM PLATFORM_EMSCRIPTEN
#endif

#define DEFAULT_FPS 60
#define BASE_SCREEN_HEIGHT 640
#define BASE_SCREEN_WIDTH 0

#define LOG_DEBUG       1
#define LOG_INFO        2
#define LOG_WARN        3
#define LOG_ERROR       4

#define MAX_TEXTURE_SIZE 4096
#define TEXTURE_MARGIN 1
#define MULTI_TEXTURE_NUM 4

#define DIRTY_VERTEX        0b00000001
#define DIRTY_COLOR         0b00000010
#define DIRTY_TEXTURE       0b00000100
#define DIRTY_TEX_COORDS    0b00001000
#define DIRTY_RENDERER_ALL  (DIRTY_VERTEX | DIRTY_COLOR | DIRTY_TEXTURE | DIRTY_TEX_COORDS)
#define DIRTY_SIZE          0b00010000
#define DIRTY_ANCHOR        0b00100000
#define DIRTY_ALL           (DIRTY_RENDERER_ALL | DIRTY_SIZE | DIRTY_ANCHOR)
#define IN_BATCHING         0b01000000

#define SET_IN_FIXED_VALUE      0b00000000
#define SET_IN_RATIO_WIDTH      0b00000001
#define SET_IN_RATIO_HEIGHT     0b00000010
#define SET_IN_RATIO_BOTH       (SET_IN_RATIO_WIDTH | SET_IN_RATIO_HEIGHT)

#define PROP_KEY_ENTITY_TYPE        "entityType"
#define PROP_KEY_NAME               "name"
#define PROP_KEY_TAG                "tag"
#define PROP_KEY_POSITION_X         "positionX"
#define PROP_KEY_POSITION_Y         "positionY"
#define PROP_KEY_PIVOT_X            "pivotX"
#define PROP_KEY_PIVOT_Y            "pivotY"
#define PROP_KEY_ANCHOR_X           "anchorX"
#define PROP_KEY_ANCHOR_Y           "anchorY"
#define PROP_KEY_SIZE_WIDTH         "width"
#define PROP_KEY_SIZE_HEIGHT        "height"
#define PROP_KEY_SIZE_SET_IN_RATIO  "sizeSetInRatio"
#define PROP_KEY_COLOR              "color"
#define PROP_KEY_ALPHA              "alpha"
#define PROP_KEY_Z_INDEX            "zIndex"
#define PROP_KEY_RADIUS             "radius"
#define PROP_KEY_TEXT               "text"
#define PROP_KEY_FONT_SIZE          "fontSize"
#define PROP_KEY_FONT_FILENAME      "fontFilename"
#define PROP_KEY_FONT_HEIGHT        "fontHeight"
#define PROP_KEY_POINTS             "points"
#define PROP_KEY_POINT_X            "px"
#define PROP_KEY_POINT_Y            "py"
#define PROP_KEY_LINE_WIDTH         "lineWidth"
#define PROP_KEY_LINE_TYPE          "lineType"
#define PROP_KEY_CORNER_RADIUS      "cornerRadius"
#define PROP_KEY_CORNER_FLAG        "cornerFlag"
#define PROP_KEY_FILENAME           "filename"
#define PROP_KEY_RECT_X             "rect-x"
#define PROP_KEY_RECT_Y             "rect-y"
#define PROP_KEY_RECT_WIDTH         "rect-width"
#define PROP_KEY_RECT_HEIGHT        "rect-height"
#define PROP_KEY_CENTER_RECT_X      "centerRect-x"
#define PROP_KEY_CENTER_RECT_Y      "centerRect-y"
#define PROP_KEY_CENTER_RECT_WIDTH  "centerRect-width"
#define PROP_KEY_CENTER_RECT_HEIGHT "centerRect-height"
#define PROP_KEY_ENABLE_BATCHING    "enableBatching"
#define PROP_KEY_CHILD_ENTITIES     "childEntities"
#define PROP_KEY_PADDING            "padding"

#ifndef LOG_LEVEL
#ifdef MOG_DEBUG
#define LOG_LEVEL LOG_DEBUG
#else
#define LOG_LEVEL LOG_INFO
#endif
#endif

#include "mog/core/mogmalloc.h"
#include "mog/ConstantsNative.h"
#include "mog/core/mog_functions.h"

#endif /* Constants_h */
