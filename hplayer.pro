#-------------------------------------------------
#
# Project created by QtCreator 2018-05-01T12:34:56
#
#-------------------------------------------------

# default CONFIG contains debug,release,debug_and_release
#CONFIG -= debug
#CONFIG -= release
#CONFIG -= debug_and_release
DEFINES -= UNICODE _UNICODE

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app
RC_ICONS = favicon.ico
RC_FILE = hplayer.rc

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

# hv
INCLUDEPATH += src/hv src/hv/base src/hv/utils
HEADERS += \
    src/hv/hv.h \
    src/hv/base/hplatform.h \
    src/hv/base/hdef.h \
    src/hv/base/hbase.h \
    src/hv/base/hversion.h \
    src/hv/base/htime.h \
    src/hv/base/herr.h \
    src/hv/base/hbuf.h \
    src/hv/base/hgui.h \
    src/hv/base/hlog.h \
    src/hv/base/hobj.h \
    src/hv/base/hvar.h \
    src/hv/base/hscope.h \
    src/hv/base/hstring.h \
    src/hv/base/hmutex.h \
    src/hv/base/hthread.h \
    src/hv/utils/singleton.h \
    src/hv/utils/iniparser.h \

SOURCES += \
    src/hv/base/hversion.c \
    src/hv/base/hbase.c \
    src/hv/base/herr.c \
    src/hv/base/htime.c \
    src/hv/base/hlog.c \
    src/hv/base/hstring.cpp \
    src/hv/utils/iniparser.cpp \

# qt
INCLUDEPATH += src/qt
HEADERS += \
    src/qt/qtheaders.h \
    src/qt/qtfunctions.h \
    src/qt/qtrcloader.h \
    src/qt/HGLWidget.h \

SOURCES += \
    src/qt/qtrcloader.cpp \
    src/qt/HGLWidget.cpp \

# ui
INCLUDEPATH += src/ui
HEADERS +=  \
    src/ui/qtstyles.h   \
    src/ui/CustomEventType.h \
    src/ui/htable.h \
    src/ui/MainWindow.h \
    src/ui/CentralWidget.h \
    src/ui/HMultiView.h \
    src/ui/HVideoWidget.h \
    src/ui/HVideoTitlebar.h \
    src/ui/HVideoToolbar.h \
    src/ui/HVideoWnd.h \
    src/ui/HOpenMediaDlg.h \
    src/ui/HMediaInfoDlg.h \
    src/ui/HVideoWndFactory.h \
    src/ui/GLWnd.h \

SOURCES +=  \
    src/ui/htable.cpp \
    src/ui/MainWindow.cpp \
    src/ui/CentralWidget.cpp \
    src/ui/HMultiView.cpp \
    src/ui/HVideoWidget.cpp \
    src/ui/HVideoTitlebar.cpp \
    src/ui/HVideoToolbar.cpp \
    src/ui/HVideoWnd.cpp \
    src/ui/HOpenMediaDlg.cpp \
    src/ui/HMediaInfoDlg.cpp \
    src/ui/GLWnd.cpp \

# GL
DEFINES += GLEW_STATIC
INCLUDEPATH += src/GL
HEADERS += src/GL/glew.h
SOURCES += src/GL/glew.c

# video
INCLUDEPATH += src/video
HEADERS +=  \
    src/video/hmedia.h \
    src/video/HVideoPlayer.h \
    src/video/HVideoPlayerFactory.h \
    src/video/hffplayer.h \

SOURCES += \
    src/video/hffplayer.cpp \

# win32
INCLUDEPATH += src/win32
HEADERS += src/win32/hdevice.h
SOURCES += src/win32/hdevice.cpp

# util
INCLUDEPATH += src/util
HEADERS += \
    src/util/ffmpeg_util.h \
    src/util/sdl_util.h \
    src/util/hgl.h \
    src/util/hframe.h \

SOURCES += src/util/hframe.cpp

# SDL
HEADERS += src/ui/SDL2Wnd.h
SOURCES += src/ui/SDL2Wnd.cpp
LIBS += -lSDL2

# global
HEADERS += src/appdef.h src/confile.h src/avdef.h
SOURCES += src/main.cpp

win32 {
    DEFINES += WIN32_LEAN_AND_MEAN
    INCLUDEPATH += 3rd/include

    ## opencv
    ##LIBS += -lopencv_core341        \
    ##        -lopencv_highgui341     \
    ##        -lopencv_imgcodecs341   \
    ##        -lopencv_imgproc341     \
    ##        -lopencv_videoio341     \

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
        if (contains(DEFINES, WIN64)) {
            LIBS += -L$$_PRO_FILE_PWD_/3rd/lib/msvc2015_x64
            DESTDIR = $$_PRO_FILE_PWD_/bin/msvc2015_x64
        } else {
            LIBS += -L$$_PRO_FILE_PWD_/3rd/lib/msvc2015_x86
            DESTDIR = $$_PRO_FILE_PWD_/bin/msvc2015_x86
        }
    }

    win32-g++ {
        QMAKE_CFLAGS += -std=c99
        QMAKE_CXXFLAGS += -std=c++11
        if (contains(DEFINES, WIN64)) {
            LIBS += -L$$_PRO_FILE_PWD_/3rd/lib/mingw64
            DESTDIR = $$_PRO_FILE_PWD_/bin/mingw64
        } else {
            LIBS += -L$$_PRO_FILE_PWD_/3rd/lib/mingw32
            DESTDIR = $$_PRO_FILE_PWD_/bin/mingw32
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
    ##LIBS += -lopencv_highgui    \
    ##        -lopencv_imgproc    \
    ##        -lopencv_core       \

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
        LIBS += -L$$_PRO_FILE_PWD_/3rd/lib/linux
        DESTDIR = $$_PRO_FILE_PWD_/bin/linux
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
