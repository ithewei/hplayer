#include "qtheaders.h"

#include "mainwindow.h"
#include "qtstyles.h"
#include "appdef.h"
#include "confile.h"

IniParser* g_confile = NULL;

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
    QString strLog = QString::asprintf("[%s][%s] %s\n",
                                       QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz").toLocal8Bit().data(),
                                       szType,
                                       msg.toLocal8Bit().data());
#else
    QString strLog = QString::asprintf("[%s][%s] %s [%s:%d-%s]\n",
                                       QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz").toLocal8Bit().data(),
                                       szType,
                                       msg.toLocal8Bit().data(),
                                       ctx.file,ctx.line,ctx.function);
#endif


    QString strLogfile = "logs/qt.log";

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
    MKDIR("logs");
    qInstallMessageHandler(qLogHandler);

    // load confile
    g_confile = new IniParser;
    char confile[] = "conf/" APP_NAME ".conf";
    if (access(confile, 0) != 0) {
        QFile::copy("conf/" APP_NAME ".conf.default", confile);
    }
    g_confile->LoadFromFile(confile);
    // logfile
    string str = g_confile->GetValue("logfile");
    hlog_set_file(str.empty() ? "logs/" APP_NAME ".log" : str.c_str());
    // loglevel
    const char* szLoglevel = g_confile->GetValue("loglevel").c_str();
    int loglevel = LOG_LEVEL_DEBUG;
    if (stricmp(szLoglevel, "VERBOSE") == 0) {
        loglevel = LOG_LEVEL_VERBOSE;
    } else if (stricmp(szLoglevel, "DEBUG") == 0) {
        loglevel = LOG_LEVEL_DEBUG;
    } else if (stricmp(szLoglevel, "INFO") == 0) {
        loglevel = LOG_LEVEL_INFO;
    } else if (stricmp(szLoglevel, "WARN") == 0) {
        loglevel = LOG_LEVEL_WARN;
    } else if (stricmp(szLoglevel, "ERROR") == 0) {
        loglevel = LOG_LEVEL_ERROR;
    } else if (stricmp(szLoglevel, "FATAL") == 0) {
        loglevel = LOG_LEVEL_FATAL;
    } else if (stricmp(szLoglevel, "SILENT") == 0) {
        loglevel = LOG_LEVEL_SILENT;
    } else {
        loglevel = LOG_LEVEL_INFO;
    }
    hlog_set_level(loglevel);
    // log_filesize
    str = g_confile->GetValue("log_filesize");
    if (!str.empty()) {
        int num = atoi(str.c_str());
        if (num > 0) {
            // 16 16M 16MB
            const char* p = str.c_str() + str.size() - 1;
            char unit;
            if (*p >= '0' && *p <= '9') unit = 'M';
            else if (*p == 'B')         unit = *(p-1);
            else                        unit = *p;
            unsigned long long filesize = num;
            switch (unit) {
            case 'K': filesize <<= 10; break;
            case 'M': filesize <<= 20; break;
            case 'G': filesize <<= 30; break;
            default:  filesize <<= 20; break;
            }
            hlog_set_max_filesize(filesize);
        }
    }
    // log_remain_days
    str = g_confile->GetValue("log_remain_days");
    if (!str.empty()) {
        hlog_set_remain_days(atoi(str.c_str()));
    }
    // log_fsync
    str = g_confile->GetValue("log_fsync");
    if (!str.empty()) {
        logger_enable_fsync(hlog, getboolean(str.c_str()));
    }
    // first log here
    hlogi("%s version: %s", argv[0], hv_compile_version());
    hlog_fsync();

    qInfo("-------------------app start----------------------------------");
    QApplication app(argc, argv);
    app.setApplicationName(APP_NAME);

    str = g_confile->GetValue("skin", "ui");
    loadSkin(str.empty() ? DEFAULT_SKIN : str.c_str());

    str = g_confile->GetValue("palette", "ui");
    setPalette(str.empty() ? DEFAULT_PALETTE_COLOR : strtoul(str.c_str(), NULL, 16));

    str = g_confile->GetValue("language", "ui");
    loadLang(str.empty() ? DEFAULT_LANGUAGE : str.c_str());

    setFont(g_confile->Get<int>("fontsize", "ui", DEFAULT_FONT_SIZE));

    // NOTE: replace with image.qrc
    // rcloader->loadIcon();

    MainWindow::instance();
    g_mainwnd->window_state = (MainWindow::window_state_e)(g_confile->Get<int>("main_window_state", "ui"));
    switch (g_mainwnd->window_state) {
    case MainWindow::FULLSCREEN:
        g_mainwnd->showFullScreen();
        break;
    case MainWindow::MAXIMIZED:
        g_mainwnd->showMaximized();
        break;
    case MainWindow::MINIMIZED:
        g_mainwnd->showMinimized();
        break;
    default:
        str = g_confile->GetValue("main_window_rect", "ui");
        if (!str.empty()) {
            int x,y,w,h;
            x = y = w = h = 0;
            sscanf(str.c_str(), "rect(%d,%d,%d,%d)", &x, &y, &w, &h);
            if (w && h) {
                g_mainwnd->setGeometry(x, y, w, h);
            }
        }
        g_mainwnd->show();
        break;
    }
    if (g_confile->Get<bool>("mv_fullscreen", "ui")) {
        g_mainwnd->mv_fullscreen();
    }

    int exitcode = app.exec();

    g_confile->Set<int>("main_window_state", (int)g_mainwnd->window_state, "ui");
    str = asprintf("rect(%d,%d,%d,%d)",
                    g_mainwnd->x(),
                    g_mainwnd->y(),
                    g_mainwnd->width(),
                    g_mainwnd->height());
    g_confile->SetValue("main_window_rect", str, "ui");

    MainWindow::exitInstance();
    qInfo("-------------------app end----------------------------------");

    g_confile->Save();
    SAFE_DELETE(g_confile);

    return exitcode;
}
