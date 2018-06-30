cmake_minimum_required(VERSION 3.4.1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(PROJ_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../..")

add_definitions(-DMOG_ANDROID)

IF(CMAKE_BUILD_TYPE MATCHES Debug)
add_definitions(-DMOG_DEBUG)
ENDIF()

add_library(
    native-lib SHARED

    ${PROJ_DIR}/classes/app/App.cpp
    ${PROJ_DIR}/classes/mog/core/Tween.cpp
    ${PROJ_DIR}/classes/mog/core/PubSub.cpp
    ${PROJ_DIR}/classes/mog/core/MogStats.cpp
    ${PROJ_DIR}/classes/mog/core/MogUILoader.cpp
    ${PROJ_DIR}/classes/mog/core/NativePlugin.cpp
    ${PROJ_DIR}/classes/mog/core/FileUtils.cpp
    ${PROJ_DIR}/classes/mog/core/Data.cpp
    ${PROJ_DIR}/classes/mog/core/Shader.cpp
    ${PROJ_DIR}/classes/mog/core/DataStore.cpp
    ${PROJ_DIR}/classes/mog/core/Transform.cpp
    ${PROJ_DIR}/classes/mog/core/Http.cpp
    ${PROJ_DIR}/classes/mog/core/Texture2D.cpp
    ${PROJ_DIR}/classes/mog/core/TouchEventListener.cpp
    ${PROJ_DIR}/classes/mog/core/Engine.cpp
    ${PROJ_DIR}/classes/mog/core/TextureAtlas.cpp
    ${PROJ_DIR}/classes/mog/core/Density.cpp
    ${PROJ_DIR}/classes/mog/core/Preference.cpp
    ${PROJ_DIR}/classes/mog/core/AudioPlayer.cpp
    ${PROJ_DIR}/classes/mog/core/Collision.cpp
    ${PROJ_DIR}/classes/mog/core/plain_objects.cpp
    ${PROJ_DIR}/classes/mog/core/Renderer.cpp
    ${PROJ_DIR}/classes/mog/core/mog_functions.cpp
    ${PROJ_DIR}/classes/mog/libs/json.c
    ${PROJ_DIR}/classes/mog/libs/aes.c
    ${PROJ_DIR}/classes/mog/libs/rpmalloc.c
    ${PROJ_DIR}/classes/mog/libs/sha256.cpp
    ${PROJ_DIR}/classes/mog/base/Line.cpp
    ${PROJ_DIR}/classes/mog/base/SpriteSheet.cpp
    ${PROJ_DIR}/classes/mog/base/Scene.cpp
    ${PROJ_DIR}/classes/mog/base/Entity.cpp
    ${PROJ_DIR}/classes/mog/base/Sprite.cpp
    ${PROJ_DIR}/classes/mog/base/Drawable.cpp
    ${PROJ_DIR}/classes/mog/base/Slice9Sprite.cpp
    ${PROJ_DIR}/classes/mog/base/Graphics.cpp
    ${PROJ_DIR}/classes/mog/base/Label.cpp
    ${PROJ_DIR}/classes/mog/base/Triangle.cpp
    ${PROJ_DIR}/classes/mog/base/Polygon.cpp
    ${PROJ_DIR}/classes/mog/base/Rectangle.cpp
    ${PROJ_DIR}/classes/mog/base/Circle.cpp
    ${PROJ_DIR}/classes/mog/base/DrawableGroup.cpp
    ${PROJ_DIR}/classes/mog/base/AppBase.cpp
    ${PROJ_DIR}/classes/mog/base/Group.cpp
    ${PROJ_DIR}/classes/mog/base/RoundedRectangle.cpp

    ${PROJ_DIR}/classes_android/mog/core/Device.cpp
    ${PROJ_DIR}/classes_android/mog/core/AudioPlayerNative.cpp
    ${PROJ_DIR}/classes_android/mog/core/mog_functions_native.cpp
    ${PROJ_DIR}/classes_android/mog/core/NativeClass.cpp
    ${PROJ_DIR}/classes_android/mog/core/FileUtilsNative.cpp
    ${PROJ_DIR}/classes_android/mog/core/Texture2DNative.cpp
    ${PROJ_DIR}/classes_android/mog/core/PreferenceNative.cpp
    ${PROJ_DIR}/classes_android/mog/os/Java_MogJniBridge.cpp
    )

target_include_directories(
    native-lib PRIVATE
    .
    ${PROJ_DIR}/classes
    ${PROJ_DIR}/classes_android
    )

# add lib dependencies
target_link_libraries(
    native-lib
    android
    GLESv2
    OpenSLES
    log
    )
