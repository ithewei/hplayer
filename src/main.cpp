#include "qtheaders.h"

#include "mainwindow.h"
#include "qtstyles.h"
#include "appdef.h"

#define LOG_LEVEL   0

void qLogHandler(QtMsgType type, const QMessageLogContext & ctx, const QString & msg) {
    // QtDebugMsg = 0
    // QtInfoMsg = 4
    if (type < LOG_LEVEL)
        return;

    char szType[16];
    switch (type) {
    case QtDebugMsg:
        strcpy(szType, "DEBUG");
        break;
    case QtInfoMsg:
        strcpy(szType, "INFO ");
        break;
    case QtWarningMsg:
        strcpy(szType, "WARN ");
        break;
    case QtCriticalMsg:
        strcpy(szType, "ERROR");
        break;
    case QtFatalMsg:
        strcpy(szType, "FATAL");
    }

    QString strLog = QString::asprintf("[%s] [%s]: %s [%s:%d-%s]\n",
                                       QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz").toLocal8Bit().data(),
                                       szType,
                                       msg.toLocal8Bit().data(),
                                       ctx.file,ctx.line,ctx.function);


    QString strLogfile = "qt.log";

    static FILE* s_fp = NULL;
    if (!s_fp) {
        s_fp = fopen(strLogfile.toLocal8Bit().data(), "a");
    }

    if (s_fp) {
        fseek(s_fp, 0, SEEK_END);
        if (ftell(s_fp) > (2 << 20)) {
            fclose(s_fp);
            s_fp = fopen(strLogfile.toLocal8Bit().data(), "w");
        }
    }

    if (s_fp) {
        fputs(strLog.toLocal8Bit().data(), s_fp);
    }
}

int main(int argc, char *argv[]) {
    qInstallMessageHandler(qLogHandler);
    hlog_set_file(APP_NAME".log");
    hlog_set_level(LOG_LEVEL_DEBUG);

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
