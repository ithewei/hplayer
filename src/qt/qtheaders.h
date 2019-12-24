#ifndef QT_HEADERS_H
#define QT_HEADERS_H

// using glew.h not gl.h
#include "hgl.h"

#include "hv.h"

#include <QtCore>
#include <QtGui>
#include <QtWidgets>

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#define DATE qPrintable(QDate::currentDate().toString("yyyy-MM-dd"))
#define TIME qPrintable(QTime::currentTime().toString("hh:mm:ss"))
#define DATETIME qPrintable(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))

#define TIMEMS qPrintable(QTime::currentTime().toString("hh:mm:ss zzz"))
#define DATETIMENBSP qPrintable(QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss"))
#define DATETIMEMSNBSP qPrintable(QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss-zzz"))

#define DESKTOP_WIDTH   qApp->desktop()->availableGeometry().width()
#define DESKTOP_HEIGHT  qApp->desktop()->availableGeometry().height()

#include "qtfunctions.h"
#include "qtrcloader.h"
//============================================================================
#endif // QT_HEADERS_H
