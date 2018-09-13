#-------------------------------------------------
#
# Project created by QtCreator 2018-05-01T12:34:56
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app

TARGET          = hplayer
MOC_DIR         = tmp/moc
RCC_DIR         = tmp/rcc
UI_DIR          = tmp/ui
OBJECTS_DIR     = tmp/obj
DESTDIR         = bin

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
    src/ui/hopenmediadlg.h \
    src/hw/base64.h \
    src/hw/h.h \
    src/hw/hbuf.h \
    src/hw/hbytearray.h \
    src/hw/hdef.h \
    src/hw/herr.h \
    src/hw/hframe.h \
    src/hw/hgl.h \
    src/hw/hgui.h \
    src/hw/hlog.h \
    src/hw/hmutex.h \
    src/hw/hobj.h \
    src/hw/hplatform.h \
    src/hw/hscope.h \
    src/hw/hstring.h \
    src/hw/htable.h \
    src/hw/hthread.h \
    src/hw/htime.h \
    src/hw/hvar.h \
    src/hw/hversion.h \
    src/hw/singleton.h

HEADERS +=  \
    src/video/hvideoplayer.h \
    src/video/hvideocapture.h \
    src/video/hvideoplayerfactory.h \
    src/video/hmedia.h \
    src/video/opencv_util.h

HEADERS += \
    src/appdef.h

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
    src/ui/hopenmediadlg.cpp \
    src/hw/base64.cpp \
    src/hw/herr.cpp \
    src/hw/hframe.cpp \
    src/hw/hlog.cpp \
    src/hw/hstring.cpp \
    src/hw/htable.cpp \
    src/hw/htime.cpp

SOURCES +=  src/video/hvideocapture.cpp

SOURCES += \
    src/jsoncpp/json_reader.cpp \
    src/jsoncpp/json_value.cpp \
    src/jsoncpp/json_writer.cpp

HEADERS +=  src/win32/hdevice.h
SOURCES +=  src/win32/hdevice.cpp

win32 {
    DEFINES += WIN32_LEAN_AND_MEAN GLEW_STATIC
#    DEFINES += CURL_STATICLIB
#    INCLUDEPATH += 3rd/include/freetype2/

    LIBS += kernel32.lib    \
            user32.lib      \
            gdi32.lib       \
            \
            opengl32.lib    \
            glu32.lib       \
#            \
#            ws2_32.lib      \
#            wldap32.lib     \
#            Advapi32.lib    \
#            Crypt32.lib     \
            \
            ole32.lib       \
            oleaut32.lib    \
            strmiids.lib

    LIBS += -L$$PWD/3rd/lib/x86
    LIBS += glew32s.lib
#    LIBS += libcurl_a.lib

    LIBS += opencv_core341.lib  \
            opencv_highgui341.lib   \
            opencv_imgcodecs341.lib \
            opencv_imgproc341.lib   \
            opencv_videoio341.lib
}

unix{
}
