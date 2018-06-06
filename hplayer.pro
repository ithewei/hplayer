#-------------------------------------------------
#
# Project created by QtCreator 2018-05-01T12:34:56
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app

debug{
TARGET          = hplayerd
MOC_DIR         = debug/tmp/moc
RCC_DIR         = debug/tmp/rcc
UI_DIR          = debug/tmp/ui
OBJECTS_DIR     = debug/tmp/obj
DESTDIR         = debug/bin
}

release{
TARGET          = hplayer
MOC_DIR         = release/tmp/moc
RCC_DIR         = release/tmp/rcc
UI_DIR          = release/tmp/ui
OBJECTS_DIR     = release/tmp/obj
DESTDIR         = release/bin
}

#PRECOMPILED_HEADER  = src/ui/qtheaders.h

DEFINES += QT_DEPRECATED_WARNINGS
RESOURCES += rc/skin.qrc rc/image.qrc
TRANSLATIONS = rc/lang/app_zh_CN.ts rc/lang/app_zh_CN.qm

INCLUDEPATH += 3rd/include src src/hw src/ui src/video src/win32

HEADERS +=  \
    src/ui/centralwidget.h \
    src/ui/hmedialist.h \
    src/ui/hmultiview.h \
    src/ui/hvideotitlebar.h \
    src/ui/hvideotoolbar.h \
    src/ui/hvideownd.h \
    src/ui/mainwindow.h \
    src/ui/qtfunctions.h \
    src/ui/qtheaders.h \
    src/ui/qtrcloader.h \
    src/ui/qtstyles.h   \
    src/ui/hglwidget.h \
    src/ui/hvideowidget.h \
    src/ui/hopenmediadlg.h

HEADERS +=  \
    src/video/hvideoplayer.h \
    src/video/hvideocapture.h \
    src/video/hvideoplayerfactory.h \
    src/video/hmedia.h \
    src/video/opencv_util.h

HEADERS += \
    src/appdef.h \
    src/hw/hbuf.h \
    src/hw/hcc.h \
    src/hw/hcos.h \
    src/hw/hdef.h \
    src/hw/herr.h \
    src/hw/hlayout.h \
    src/hw/hlog.h \
    src/hw/hobj.h \
    src/hw/hpch.h \
    src/hw/htask.h \
    src/hw/htaskmanager.h \
    src/hw/hthread.h \
    src/hw/hutil.h \
    src/hw/hvar.h \
    src/hw/hversion.h \
    src/hw/singleton.h \
    src/hw/hgui.h \
    src/hw/hframe.h \
    src/hw/hgl.h

SOURCES +=  \
    src/ui/centralwidget.cpp \
    src/ui/hmedialist.cpp \
    src/ui/hmultiview.cpp \
    src/ui/hvideotitlebar.cpp \
    src/ui/hvideotoolbar.cpp \
    src/ui/hvideownd.cpp \
    src/ui/main.cpp \
    src/ui/mainwindow.cpp \
    src/ui/qtrcloader.cpp \
    src/ui/hglwidget.cpp \
    src/ui/hvideowidget.cpp \
    src/ui/hopenmediadlg.cpp

SOURCES +=  src/video/hvideocapture.cpp

SOURCES += \
    src/hw/herr.cpp \
    src/hw/hlayout.cpp \
    src/hw/hlog.cpp \
    src/hw/htask.cpp \
    src/hw/htaskmanager.cpp \
    src/hw/hutil.cpp \
    src/jsoncpp/json_reader.cpp \
    src/jsoncpp/json_value.cpp \
    src/jsoncpp/json_writer.cpp

HEADERS +=  src/win32/hdevice.h
SOURCES +=  src/win32/hdevice.cpp

win32 {
    DEFINES += WIN32_LEAN_AND_MEAN GLEW_STATIC CURL_STATICLIB
    INCLUDEPATH += 3rd/include/freetype2/

    LIBS += kernel32.lib    \
            user32.lib      \
            gdi32.lib       \
            \
            opengl32.lib    \
            glu32.lib       \
            \
            ws2_32.lib      \
            wldap32.lib     \
            Advapi32.lib    \
            Crypt32.lib     \
            \
            ole32.lib       \
            oleaut32.lib    \
            strmiids.lib

    LIBS += -L$$PWD/3rd/lib/x86 \
            glew32s.lib         \
            libcurl_a.lib

    debug{
    LIBS += opencv_core341d.lib  \
            opencv_highgui341d.lib   \
            opencv_imgcodecs341d.lib \
            opencv_imgproc341d.lib   \
            opencv_videoio341d.lib
    }

    release{
    LIBS += opencv_core341.lib  \
            opencv_highgui341.lib   \
            opencv_imgcodecs341.lib \
            opencv_imgproc341.lib   \
            opencv_videoio341.lib
    }
}

unix{
}
