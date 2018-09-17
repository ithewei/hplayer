#include "qtheaders.h"
#include "mainwindow.h"
#include "qtstyles.h"
#include "appdef.h"

FILE* g_loggerfp = NULL;
#define LOG_LEVEL   0

void myLogHandler(QtMsgType type, const QMessageLogContext & ctx, const QString & msg){
    // QtDebugMsg = 0
    // QtInfoMsg = 4
    if (type < LOG_LEVEL)
        return;

    char szType[16];
    switch (type) {
    case QtDebugMsg:
        strcpy(szType, "Debug");
        break;
    case QtInfoMsg:
        strcpy(szType, "Info");
        break;
    case QtWarningMsg:
        strcpy(szType, "Warning");
        break;
    case QtCriticalMsg:
        strcpy(szType, "Critical");
        break;
    case QtFatalMsg:
        strcpy(szType, "Fatal");
    }

    QString strLog = QString::asprintf("[%s] [%s]:%s [%s:%d-%s]\n",
                                       QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz").toLocal8Bit().data(),
                                       szType,
                                       msg.toLocal8Bit().data(),
                                       ctx.file,ctx.line,ctx.function);


    QString strLogFilePath = "qt.log";

    if (!g_loggerfp){
        g_loggerfp = fopen(strLogFilePath.toLocal8Bit().data(), "a");
    }

    if (g_loggerfp){
        fseek(g_loggerfp, 0, SEEK_END);
        if (ftell(g_loggerfp) > (2 << 20)){
            fclose(g_loggerfp);
            g_loggerfp = fopen(strLogFilePath.toLocal8Bit().data(), "w");
        }
    }

    if (g_loggerfp){
        fputs(strLog.toLocal8Bit().data(), g_loggerfp);
    }
}

int main(int argc, char *argv[])
{
    qInstallMessageHandler(myLogHandler);
    hlog_set_file(APP_NAME".log");
    hlog_set_level(LOG_LEVEL_INFO);

    hlogi("%s", get_compile_version());
    qInfo("-------------------app start----------------------------------");
    QApplication a(argc, argv);
    a.setApplicationName(APP_NAME);

    setFont(DEFAULT_FONT_SIZE);
    loadSkin(DEFAULT_SKIN);
    setPalette(DEFAULT_PALETTE_COLOR);
    loadLang(DEFAULT_LANGUAGE);

    // note: we use image.qrc
    // rcloader->loadIcon();

    g_mainwnd->showMaximized();

    int retcode = a.exec();

    MainWindow::exitInstance();
    return retcode;
}
