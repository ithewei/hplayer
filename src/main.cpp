#include "qtheaders.h"

#include "mainwindow.h"
#include "qtstyles.h"
#include "appdef.h"

#define LOG_LEVEL   0

void qLogHandler(QtMsgType type, const QMessageLogContext & ctx, const QString & msg) {
    if (type < LOG_LEVEL)
        return;

    //enum QtMsgType { QtDebugMsg, QtWarningMsg, QtCriticalMsg, QtFatalMsg, QtInfoMsg, QtSystemMsg = QtCriticalMsg };
    static char s_types[5][6] = {"DEBUG", "WARN ", "ERROR", "FATAL", "INFO "};
    const char* szType = "DEBUG";
    if (type < 5) {
        szType = s_types[(int)type];
    }


#ifdef QT_NO_DEBUG
    QString strLog = QString::asprintf("[%s][%s]: %s\n",
                                       QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz").toLocal8Bit().data(),
                                       szType,
                                       msg.toLocal8Bit().data());
#else
    QString strLog = QString::asprintf("[%s][%s]: %s [%s:%d-%s]\n",
                                       QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz").toLocal8Bit().data(),
                                       szType,
                                       msg.toLocal8Bit().data(),
                                       ctx.file,ctx.line,ctx.function);
#endif


    QString strLogfile = "qt.log";

    static FILE* s_fp = NULL;
    if (s_fp) {
        fseek(s_fp, 0, SEEK_END);
        if (ftell(s_fp) > (2 << 20)) {
            fclose(s_fp);
            s_fp = NULL;
        }
    }

    if (!s_fp) {
        s_fp = fopen(strLogfile.toLocal8Bit().data(), "w");
    }

    if (s_fp) {
        fputs(strLog.toLocal8Bit().data(), s_fp);
    }
}

int main(int argc, char *argv[]) {
    qInstallMessageHandler(qLogHandler);
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

    // note: replace with image.qrc
    // rcloader->loadIcon();

    g_mainwnd->showMaximized();

    int retcode = a.exec();

    MainWindow::exitInstance();
    qInfo("-------------------app end----------------------------------");
    return retcode;
}
