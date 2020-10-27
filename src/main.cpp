#include "qtheaders.h"

#include "MainWindow.h"
#include "qtstyles.h"
#include "appdef.h"
#include "confile.h"

IniParser* g_confile = NULL;
char g_exec_path[256] = {0};
char g_exec_dir[256] = {0};
char g_run_dir[256] = {0};
char g_conf_file[256] = {0};
char g_log_file[256] = {0};

#define LOG_LEVEL   0

static void qLogHandler(QtMsgType type, const QMessageLogContext & ctx, const QString & msg) {
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

    static FILE* s_fp = NULL;
    if (s_fp) {
        fseek(s_fp, 0, SEEK_END);
        if (ftell(s_fp) > (2 << 20)) {
            fclose(s_fp);
            s_fp = NULL;
        }
    }

    if (!s_fp) {
        char logfile[256];
        snprintf(logfile, sizeof(logfile), "%s/logs/qt.log", g_exec_dir);
        s_fp = fopen(logfile, "w");
    }

    if (s_fp) {
        fputs(strLog.toLocal8Bit().data(), s_fp);
    }
}

static int load_confile() {
    get_executable_path(g_exec_path, sizeof(g_exec_path));
    get_executable_dir(g_exec_dir, sizeof(g_exec_dir));
    get_run_dir(g_run_dir, sizeof(g_run_dir));

    // load confile
    g_confile = new IniParser;
    snprintf(g_conf_file, sizeof(g_conf_file), "%s/conf/%s.conf", g_exec_dir, APP_NAME);
    if (access(g_conf_file, 0) != 0) {
        QFile::copy(QString(g_exec_dir) + "/conf/" APP_NAME ".conf.default", g_conf_file);
    }
    g_confile->LoadFromFile(g_conf_file);
    // logfile
    string str = g_confile->GetValue("logfile");
    if (str.empty()) {
        snprintf(g_log_file, sizeof(g_log_file), "%s/logs", g_exec_dir);
        hv_mkdir_p(g_log_file);
        snprintf(g_log_file, sizeof(g_log_file), "%s/logs/%s.log", g_exec_dir, APP_NAME);
    }
    else {
        strncpy(g_log_file, str.c_str(), sizeof(g_log_file));
    }
    hlog_set_file(g_log_file);
    // loglevel
    str = g_confile->GetValue("loglevel");
    if (!str.empty()) {
        hlog_set_level_by_str(str.c_str());
    }
    // log_filesize
    str = g_confile->GetValue("log_filesize");
    if (!str.empty()) {
        hlog_set_max_filesize_by_str(str.c_str());
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
    hlogi("%s version: %s", g_exec_path, hv_compile_version());
    hlog_fsync();
}

int main(int argc, char *argv[]) {
    load_confile();

    qInstallMessageHandler(qLogHandler);
    qInfo("-------------------app start----------------------------------");
    QApplication app(argc, argv);
    app.setApplicationName(APP_NAME);

    string str = g_confile->GetValue("skin", "ui");
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
