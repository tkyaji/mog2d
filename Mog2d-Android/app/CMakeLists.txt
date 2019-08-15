cmake_minimum_required(VERSION 3.4.1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(PROJ_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../..")

add_definitions(-DMOG_ANDROID)

IF(CMAKE_BUILD_TYPE MATCHES Debug)
add_definitions(-DMOG_DEBUG)
ENDIF()

add_library(
    native-lib SHARED

### SOURCE_LIST >> ###
    ${PROJ_DIR}/sources/app/SampleScene.cpp
    ${PROJ_DIR}/sources/app/App.cpp
    ${PROJ_DIR}/sources/mog/core/Tween.cpp
    ${PROJ_DIR}/sources/mog/core/PubSub.cpp
    ${PROJ_DIR}/sources/mog/core/MogStats.cpp
    ${PROJ_DIR}/sources/mog/core/Screen.cpp
    ${PROJ_DIR}/sources/mog/core/MogUILoader.cpp
    ${PROJ_DIR}/sources/mog/core/FileUtils.cpp
    ${PROJ_DIR}/sources/mog/core/EntityCreator.cpp
    ${PROJ_DIR}/sources/mog/core/Data.cpp
    ${PROJ_DIR}/sources/mog/core/Shader.cpp
    ${PROJ_DIR}/sources/mog/core/DataStore.cpp
    ${PROJ_DIR}/sources/mog/core/Transform.cpp
    ${PROJ_DIR}/sources/mog/core/Http.cpp
    ${PROJ_DIR}/sources/mog/core/Texture2D.cpp
    ${PROJ_DIR}/sources/mog/core/TouchEventListener.cpp
    ${PROJ_DIR}/sources/mog/core/Engine.cpp
    ${PROJ_DIR}/sources/mog/core/TextureAtlas.cpp
    ${PROJ_DIR}/sources/mog/core/Preference.cpp
    ${PROJ_DIR}/sources/mog/core/AudioPlayer.cpp
    ${PROJ_DIR}/sources/mog/core/Collision.cpp
    ${PROJ_DIR}/sources/mog/core/plain_objects.cpp
    ${PROJ_DIR}/sources/mog/core/Renderer.cpp
    ${PROJ_DIR}/sources/mog/core/mog_functions.cpp
    ${PROJ_DIR}/sources/mog/libs/json.c
    ${PROJ_DIR}/sources/mog/libs/aes.c
    ${PROJ_DIR}/sources/mog/libs/effekseer/EffekseerNative.cpp
    ${PROJ_DIR}/sources/mog/libs/effekseer/EffekseerRendererNative.cpp
    ${PROJ_DIR}/sources/mog/libs/effekseer/Effekseer.cpp
    ${PROJ_DIR}/sources/mog/libs/sha256.cpp
    ${PROJ_DIR}/sources/mog/base/Line.cpp
    ${PROJ_DIR}/sources/mog/base/SpriteSheet.cpp
    ${PROJ_DIR}/sources/mog/base/Scene.cpp
    ${PROJ_DIR}/sources/mog/base/DrawableContainer.cpp
    ${PROJ_DIR}/sources/mog/base/Entity.cpp
    ${PROJ_DIR}/sources/mog/base/ScrollGroup.cpp
    ${PROJ_DIR}/sources/mog/base/Sprite.cpp
    ${PROJ_DIR}/sources/mog/base/AlignmentGroup.cpp
    ${PROJ_DIR}/sources/mog/base/Drawable.cpp
    ${PROJ_DIR}/sources/mog/base/TiledSprite.cpp
    ${PROJ_DIR}/sources/mog/base/Slice9Sprite.cpp
    ${PROJ_DIR}/sources/mog/base/Graphics.cpp
    ${PROJ_DIR}/sources/mog/base/Label.cpp
    ${PROJ_DIR}/sources/mog/base/Polygon.cpp
    ${PROJ_DIR}/sources/mog/base/Rectangle.cpp
    ${PROJ_DIR}/sources/mog/base/Circle.cpp
    ${PROJ_DIR}/sources/mog/base/DrawableGroup.cpp
    ${PROJ_DIR}/sources/mog/base/AppBase.cpp
    ${PROJ_DIR}/sources/mog/base/HorizontalGroup.cpp
    ${PROJ_DIR}/sources/mog/base/Group.cpp
    ${PROJ_DIR}/sources/mog/base/RoundedRectangle.cpp
    ${PROJ_DIR}/sources/mog/base/VerticalGroup.cpp
    ${PROJ_DIR}/sources_android/mog/core/HttpNative.cpp
    ${PROJ_DIR}/sources_android/mog/core/AudioPlayerNative.cpp
    ${PROJ_DIR}/sources_android/mog/core/mog_functions_native.cpp
    ${PROJ_DIR}/sources_android/mog/core/NativeClass.cpp
    ${PROJ_DIR}/sources_android/mog/core/FileUtilsNative.cpp
    ${PROJ_DIR}/sources_android/mog/core/Texture2DNative.cpp
    ${PROJ_DIR}/sources_android/mog/core/PreferenceNative.cpp
    ${PROJ_DIR}/sources_android/mog/os/JNILocalFrame.cpp
    ${PROJ_DIR}/sources_android/mog/os/Java_MogJniBridge.cpp
    ${PROJ_DIR}/sources_android/mog/os/AndroidHelper.cpp
### << SOURCE_LIST ###
    )

target_include_directories(
    native-lib PRIVATE
    .
    ${PROJ_DIR}/sources
    ${PROJ_DIR}/sources_android
    )

# add lib dependencies
target_link_libraries(
    native-lib
    android
    GLESv2
    OpenSLES
    log
    )
