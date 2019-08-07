#-------------------------------------------------
#
# Project created by QtCreator 2018-05-01T12:34:56
#
#-------------------------------------------------

# default CONFIG contains debug,release,debug_and_release
CONFIG -= debug
#CONFIG -= release
CONFIG -= debug_and_release

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app

TARGET          = hplayer
MOC_DIR         = tmp/moc
RCC_DIR         = tmp/rcc
UI_DIR          = tmp/ui
OBJECTS_DIR     = tmp/obj

#PRECOMPILED_HEADER  = src/ui/qtheaders.h

DEFINES += QT_DEPRECATED_WARNINGS
RESOURCES += rc/skin.qrc rc/image.qrc
TRANSLATIONS = rc/lang/app_zh_CN.ts rc/lang/app_zh_CN.qm

INCLUDEPATH += src

# hw
INCLUDEPATH += src/hw src/hw/base src/hw/utils
HEADERS += \
    src/hw/h.h \
    src/hw/base/hplatform.h \
    src/hw/base/hdef.h \
    src/hw/base/hbase.h \
    src/hw/base/hversion.h \
    src/hw/base/htime.h \
    src/hw/base/herr.h \
    src/hw/base/hbuf.h \
    src/hw/base/hgui.h \
    src/hw/base/hlog.h \
    src/hw/base/hobj.h \
    src/hw/base/hvar.h \
    src/hw/base/hscope.h \
    src/hw/base/hstring.h \
    src/hw/base/hmutex.h \
    src/hw/base/hthread.h \
    src/hw/utils/singleton.h \
    src/hw/utils/hframe.h \
    src/hw/utils/hgl.h \

SOURCES += \
    src/hw/base/hversion.c \
    src/hw/base/hbase.c \
    src/hw/base/herr.c \
    src/hw/base/htime.c \
    src/hw/base/hlog.c \
    src/hw/base/hstring.cpp \
    src/hw/utils/hframe.cpp \

# qt
INCLUDEPATH += src/qt
HEADERS += \
    src/qt/qtheaders.h \
    src/qt/qtfunctions.h \
    src/qt/qtrcloader.h \
    src/qt/hglwidget.h \

SOURCES += \
    src/qt/qtrcloader.cpp \
    src/qt/hglwidget.cpp \

# ui
INCLUDEPATH += src/ui
HEADERS +=  \
    src/ui/centralwidget.h \
    src/ui/hmedialist.h \
    src/ui/htable.h \
    src/ui/hmultiview.h \
    src/ui/hvideotitlebar.h \
    src/ui/hvideotoolbar.h \
    src/ui/hvideownd.h \
    src/ui/mainwindow.h \
    src/ui/qtstyles.h   \
    src/ui/hvideowidget.h \
    src/ui/hopenmediadlg.h \

SOURCES +=  \
    src/ui/centralwidget.cpp \
    src/ui/hmedialist.cpp \
    src/ui/htable.cpp \
    src/ui/hmultiview.cpp \
    src/ui/hvideotitlebar.cpp \
    src/ui/hvideotoolbar.cpp \
    src/ui/hvideownd.cpp \
    src/ui/mainwindow.cpp \
    src/ui/hvideowidget.cpp \
    src/ui/hopenmediadlg.cpp \

# GL
DEFINES += GLEW_STATIC
INCLUDEPATH += src/GL
HEADERS += src/GL/glew.h
SOURCES += src/GL/glew.c

# video
INCLUDEPATH += src/video
HEADERS +=  \
    src/video/hmedia.h \
    src/video/hvideoplayer.h \
    src/video/hvideoplayerfactory.h \
    src/video/opencv_util.h \
    src/video/hvideocapture.h \
    src/video/hffplayer.h \

SOURCES += \
    src/video/hvideocapture.cpp \
    src/video/hffplayer.cpp \

# win32
INCLUDEPATH += src/win32
HEADERS += src/win32/hdevice.h
SOURCES += src/win32/hdevice.cpp

# global
HEADERS += \
    src/appdef.h

SOURCES += \
    src/main.cpp

win32 {
    DEFINES += WIN32_LEAN_AND_MEAN
    INCLUDEPATH += 3rd/include

    ## opencv
    LIBS += -lopencv_core341        \
            -lopencv_highgui341     \
            -lopencv_imgcodecs341   \
            -lopencv_imgproc341     \
            -lopencv_videoio341     \

    ## FFmpeg
    LIBS += -lavformat      \
            -lavdevice      \
            -lavcodec       \
            -lswresample    \
            -lswscale       \
            -lavutil        \

    ## Windows API
    LIBS += -lkernel32    \
            -luser32      \
            -lgdi32       \
            \
            -lopengl32    \
            -lglu32       \
            \
            -lole32       \
            -loleaut32    \
            -lstrmiids    \
            \
            -lws2_32      \
            -lsecur32     \

    win32-msvc {
        if (contains(QMAKE_HOST.arch, x86_64)) {
            LIBS += -L3rd/lib/msvc14_x64
            DESTDIR = bin/msvc14_x64
        } else {
            LIBS += -L3rd/lib/msvc14_x86
            DESTDIR = bin/msvc14_x86
        }
    }

    win32-g++ {
        QMAKE_CFLAGS += -std=c99
        QMAKE_CXXFLAGS += -std=c++11
        if (contains(QMAKE_HOST.arch, x86_64)) {
            LIBS += -L3rd/lib/mingw64
            DESTDIR = bin/mingw64
        } else {
            LIBS += -L3rd/lib/mingw32
            DESTDIR = bin/mingw32
        }

        # for ffmpeg staticlib
        LIBS += -liconv \
        -lz     \
        -lbz2   \
        -llzma  \
        -lcrypto \
        -lbcrypt
    }
}

unix {
    ## opencv
    LIBS += -lopencv_highgui    \
            -lopencv_imgproc    \
            -lopencv_core       \

    ## FFmpeg
    LIBS += -lavformat  \
            -lavcodec   \
            -lswscale   \
            -lavutil    \

    ## sys
    LIBS += -lGLU   \
            -lGL    \
            -lpthread   \
            -lm         \

    linux-g++ {
        QMAKE_CFLAGS += -std=c99
        QMAKE_CXXFLAGS += -std=c++11
        LIBS += -L3rd/lib/linux
        DESTDIR = bin/linux
    }
}

message(ARCH=$$QMAKE_HOST.arch)
message(QT_VERSION=$$QT_VERSION)
message(QMAKE=$$QMAKE_QMAKE)
message(QMAKESPEC=$$QMAKESPEC)
message(CC=$$QMAKE_CC)
message(CXX=$$QMAKE_CXX)
message(LINK=$$QMAKE_LINK)
message(CFLAGS=$$QMAKE_CFLAGS)
message(CXXFLAGS=$$QMAKE_CXXFLAGS)

message(PWD=$$PWD)
message(TARGET=$$DESTDIR/$$TARGET)
message(DEFINES=$$DEFINES)
message(CONFIG=$$CONFIG)
message(INCLUDEPATH=$$INCLUDEPATH)
message(LIBS=$$LIBS)
