#-------------------------------------------------
#
# Project created by QtCreator 2017-09-30T16:58:52
#
#-------------------------------------------------

QT       += core gui opengl
CONFIG   += c++11
QMAKE_CXXFLAGS += -std=c++11
DEFINES  += MOG_QT
debug {
    DEFINES += MOG_DEBUG
}
QMAKE_CXXFLAGS_WARN_ON -= -Wall

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Mog2d-Qt
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += ../classes/ ../classes_qt/
DEPENDPATH += ../classes/ ../classes_qt/

SOURCES += \
        main.cpp \
        mainwindow.cpp \
        mogglwidget.cpp \
        ../classes/mog/base/AppBase.cpp \
        ../classes/mog/base/BatchingGroup.cpp \
        ../classes/mog/base/Circle.cpp \
        ../classes/mog/base/DrawEntity.cpp \
        ../classes/mog/base/Entity.cpp \
        ../classes/mog/base/Group.cpp \
        ../classes/mog/base/Label.cpp \
        ../classes/mog/base/Polygon.cpp \
        ../classes/mog/base/Rectangle.cpp \
        ../classes/mog/base/RoundedRectangle.cpp \
        ../classes/mog/base/Scene.cpp \
        ../classes/mog/base/Slice9Sprite.cpp \
        ../classes/mog/base/Sprite.cpp \
        ../classes/mog/base/SpriteSheet.cpp \
        ../classes/mog/core/AudioPlayer.cpp \
        ../classes/mog/core/Collision.cpp \
        ../classes/mog/core/Data.cpp \
        ../classes/mog/core/DataStore.cpp \
        ../classes/mog/core/Engine.cpp \
        ../classes/mog/core/FileUtils.cpp \
        ../classes/mog/core/Http.cpp \
        ../classes/mog/core/mog_functions.cpp \
        ../classes/mog/core/NativePlugin.cpp \
        ../classes/mog/core/plain_objects.cpp \
        ../classes/mog/core/Preference.cpp \
        ../classes/mog/core/PubSub.cpp \
        ../classes/mog/core/Renderer.cpp \
        ../classes/mog/core/Texture2D.cpp \
        ../classes/mog/core/TextureAtlas.cpp \
        ../classes/mog/core/TouchEventListener.cpp \
        ../classes/mog/core/Tween.cpp \
        ../classes/mog/core/MogStats.cpp \
        ../classes/mog/core/Density.cpp \
        ../classes/mog/libs/sha256.cpp \
        ../classes/mog/libs/aes.c \
        ../classes/mog/libs/json.c \
        ../classes_qt/mog/core/mog_functions_native.cpp \
        ../classes_qt/mog/core/FileUtilsNative.cpp \
        ../classes_qt/mog/core/PreferenceNative.cpp \
        ../classes_qt/mog/core/Texture2DNative.cpp \
        ../classes_qt/mog/core/AudioPlayerNative.cpp \
        ../classes_qt/mog/core/Device.cpp \
        ../classes/app/App.cpp \
        ../classes/app/SampleScene.cpp \
        ../classes_qt/mog/os/mogenginecontroller.cpp \

HEADERS += \
        mainwindow.h \
        mogglwidget.h \
        ../classes/mog/base/AppBase.h \
        ../classes_qt/mog/os/mogenginecontroller.h \
        ../classes/mog/base/BatchingGroup.h \
        ../classes/mog/base/Circle.h \
        ../classes/mog/base/DrawEntity.h \
        ../classes/mog/base/Entity.h \
        ../classes/mog/base/Group.h \
        ../classes/mog/base/Label.h \
        ../classes/mog/base/Polygon.h \
        ../classes/mog/base/Rectangle.h \
        ../classes/mog/base/RoundedRectangle.h \
        ../classes/mog/base/Scene.h \
        ../classes/mog/base/Slice9Sprite.h \
        ../classes/mog/base/Sprite.h \
        ../classes/mog/base/SpriteSheet.h \
        ../classes/mog/core/AudioPlayer.h \
        ../classes/mog/core/Collision.h \
        ../classes/mog/core/Data.h \
        ../classes/mog/core/DataStore.h \
        ../classes/mog/core/Engine.h \
        ../classes/mog/core/FileUtils.h \
        ../classes/mog/core/Http.h \
        ../classes/mog/core/KeyEvent.h \
        ../classes/mog/core/mog_functions.h \
        ../classes/mog/core/NativeClass.h \
        ../classes/mog/core/NativePlugin.h \
        ../classes/mog/core/plain_objects.h \
        ../classes/mog/core/Preference.h \
        ../classes/mog/core/PubSub.h \
        ../classes/mog/core/Renderer.h \
        ../classes/mog/core/Texture2D.h \
        ../classes/mog/core/TextureAtlas.h \
        ../classes/mog/core/Touch.h \
        ../classes/mog/core/TouchEventListener.h \
        ../classes/mog/core/TouchInput.h \
        ../classes/mog/core/Transform.h \
        ../classes/mog/core/Tween.h \
        ../classes/mog/core/MogStats.h \
        ../classes/mog/core/Density.h \
        ../classes/mog/libs/aes.h \
        ../classes/mog/libs/http.h \
        ../classes/mog/libs/json.h \
        ../classes/mog/libs/sha256.h \
        ../classes/mog/libs/stb_image.h \
        ../classes/mog/plugins/plugins.h \
        ../classes/mog/Constants.h \
        ../classes/mog/mog.h \
        ../classes_qt/mog/ConstantsNative.h \
        ../classes_qt/mog/core/mog_functions_native.h \
        ../classes_qt/mog/core/Device.h \

FORMS += \
        mainwindow.ui

RESOURCES += \
    assets.qrc


QMAKE_POST_LINK = $${PWD}/make_qrc.sh
