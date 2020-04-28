#include "MainWindow.h"

#include "appdef.h"
#include "confile.h"
#include "qtstyles.h"

SINGLETON_IMPL(MainWindow)

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    initUI();
    initConnect();
}

MainWindow::~MainWindow() {
    hlogd("~MainWindow");
}

void MainWindow::initMenu() {
    // Media
    QMenu *mediaMenu = menuBar()->addMenu(tr("&Media"));
    QToolBar *mediaToolbar = addToolBar(tr("&Media"));
    toolbars.push_back(mediaToolbar);

    QAction* actOpenFile = new QAction(QIcon(":/image/file.png"), tr(" Open File"));
    actOpenFile->setShortcut(QKeySequence("Ctrl+F"));
    connect(actOpenFile, &QAction::triggered, this, [=](){
        OpenMediaDlg(MEDIA_TYPE_FILE);
    });
    mediaMenu->addAction(actOpenFile);
    mediaToolbar->addAction(actOpenFile);

    QAction* actOpenNetwork = new QAction(QIcon(":/image/network.png"), tr(" Open Network"));
    actOpenNetwork->setShortcut(QKeySequence("Ctrl+N"));
    connect(actOpenNetwork, &QAction::triggered, this, [=](){
        OpenMediaDlg(MEDIA_TYPE_NETWORK);
    });
    mediaMenu->addAction(actOpenNetwork);
    mediaToolbar->addAction(actOpenNetwork);

    QAction* actOpenCapture = new QAction(QIcon(":/image/capture.png"), tr(" Open Capture"));
    actOpenCapture->setShortcut(QKeySequence("Ctrl+C"));
    connect(actOpenCapture, &QAction::triggered, this, [=](){
        OpenMediaDlg(MEDIA_TYPE_CAPTURE);
    });
    mediaMenu->addAction(actOpenCapture);
    mediaToolbar->addAction(actOpenCapture);

    // View
    QMenu *viewMenu = menuBar()->addMenu(tr("&View"));

#if WITH_MV_STYLE
    QToolBar *viewToolbar = addToolBar(tr("&View"));
    toolbars.push_back(viewToolbar);

    QAction *actMVS;
    QSignalMapper *smMVS = new QSignalMapper(this);
#define VISUAL_MV_STYLE(id, row, col, label, image) \
    actMVS = new QAction(QIcon(image), tr(label), this);\
    actMVS->setToolTip(tr(label)); \
    smMVS->setMapping(actMVS, id); \
    connect( actMVS, SIGNAL(triggered(bool)), smMVS, SLOT(map()) ); \
    viewMenu->addAction(actMVS); \
    if (row * col <= 16){        \
        viewToolbar->addAction(actMVS); \
    }

    FOREACH_MV_STYLE(VISUAL_MV_STYLE)
#undef VISUAL_MV_STYLE

    connect( smMVS, SIGNAL(mapped(int)), this, SLOT(onMVStyleSelected(int)) );
#endif

    actMvFullscreen = new QAction(tr(" MV Fullscreen F12"));
    actMvFullscreen->setCheckable(true);
    actMvFullscreen->setChecked(false);
    connect( actMvFullscreen, &QAction::triggered, this, &MainWindow::mv_fullscreen );
    viewMenu->addAction(actMvFullscreen);
    viewMenu->addSeparator();

    actFullscreen = new QAction(tr(" Fullscreen F11"));
    actFullscreen->setCheckable(true);
    actFullscreen->setChecked(false);
    connect( actFullscreen, &QAction::triggered, this, &MainWindow::fullscreen );
    viewMenu->addAction(actFullscreen);

    actMenubar = new QAction(tr(" Menubar F10"));
    actMenubar->setCheckable(true);
    bool menubar_visible = g_confile->Get<bool>("menubar_visible", "ui", true);
    actMenubar->setChecked(menubar_visible);
    menuBar()->setVisible(menubar_visible);
    connect(actMenubar, &QAction::triggered, [=](bool check) {
        menuBar()->setVisible(check);
        g_confile->Set<bool>("menubar_visible", check, "ui");
    });
    viewMenu->addAction(actMenubar);

    QAction *actToolbar = new QAction(tr(" Toolbar"));
    actToolbar->setCheckable(true);
    bool toolbar_visible = g_confile->Get<bool>("toolbar_visible", "ui", true);
    actToolbar->setChecked(toolbar_visible);
    foreach(auto toolbar, toolbars) {
        toolbar->setVisible(toolbar_visible);
    }
    connect(actToolbar, &QAction::triggered, [=](bool check) {
        foreach(auto toolbar, toolbars) {
            toolbar->setVisible(check);
        }
        g_confile->Set<bool>("toolbar_visible", check, "ui");
    });
    viewMenu->addAction(actToolbar);

    QAction *actStatusbar = new QAction(tr(" Statusbar"));
    actStatusbar->setCheckable(true);
    bool statusbar_visible = g_confile->Get<bool>("statusbar_visible", "ui", false);
    actStatusbar->setChecked(statusbar_visible);
    statusBar()->setVisible(statusbar_visible);
    connect(actStatusbar, &QAction::triggered, [=](bool check) {
        statusBar()->setVisible(check);
        g_confile->Set<bool>("statusbar_visible", check, "ui");
    });
    viewMenu->addAction(actStatusbar);

    QAction *actLside = new QAction(tr(" Leftside"));
    actLside->setCheckable(true);
    bool lside_visible = g_confile->Get<bool>("lside_visible", "ui", false);
    actLside->setChecked(lside_visible);
    center->lside->setVisible(lside_visible);
    connect(actLside, &QAction::triggered, [=](bool check) {
        center->lside->setVisible(check);
        g_confile->Set<bool>("lside_visible", check, "ui");
    });
    viewMenu->addAction(actLside);

    QAction *actRside = new QAction(tr(" Rightside"));
    actRside->setCheckable(true);
    bool rside_visible = g_confile->Get<bool>("rside_visible", "ui", false);
    actRside->setChecked(rside_visible);
    center->rside->setVisible(rside_visible);
    connect(actRside, &QAction::triggered, [=](bool check) {
        center->rside->setVisible(check);
        g_confile->Set<bool>("rside_visible", check, "ui");
    });
    viewMenu->addAction(actRside);

    // Help
    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(tr(" &About"), this, SLOT(about()));
}

void MainWindow::initUI() {
    setWindowIcon(QIcon(":/image/icon.png"));

    int w = g_confile->Get<int>("main_window_width", "ui", MAIN_WINDOW_WIDTH);
    int h = g_confile->Get<int>("main_window_height", "ui", MAIN_WINDOW_HEIGHT);
    setBaseSize(w, h);

    centerWidget(this);

    center = new CentralWidget;
    setCentralWidget(center);

    initMenu();

    statusBar()->showMessage(tr("No Message!"));
}

void MainWindow::initConnect() {

}

void MainWindow::fullscreen() {
    static QRect rcOld;
    if (isFullScreen()) {
        menuBar()->setVisible(true);
        showNormal();
        setGeometry(rcOld);
    }
    else {
        rcOld = geometry();
        menuBar()->setVisible(false);
        showFullScreen();
    }
    actFullscreen->setChecked(isFullScreen());
    actMenubar->setChecked(menuBar()->isVisible());
}

void MainWindow::mv_fullscreen() {
    HMultiView* mv = center->mv;
    bool is_mv_fullscreen = false;
    if (mv->windowType() & Qt::Window) {
        mv->setWindowFlags(Qt::SubWindow);
        this->show();
    }
    else {
        mv->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
        this->hide();
        mv->showFullScreen();
        mv->raise();
        is_mv_fullscreen = true;
    }
    this->grabKeyboard();
    actMvFullscreen->setChecked(is_mv_fullscreen);
}

void MainWindow::keyPressEvent(QKeyEvent* e) {
    if (center->mv->windowType() & Qt::Window) {
        if (e->key() == Qt::Key_F12 || e->key() == Qt::Key_Escape) {
            mv_fullscreen();
        }
    }
    else {
        switch(e->key()) {
        case Qt::Key_F10:
            toggle(menuBar());
            actMenubar->setChecked(menuBar()->isVisible());
            return;
        case Qt::Key_F11:
        case Qt::Key_Escape:
            fullscreen();
            return;
        case Qt::Key_F12:
            mv_fullscreen();
            return;
        default:
            return QMainWindow::keyPressEvent(e);
        }
    }
}

void MainWindow::changeEvent(QEvent* e) {
    QMainWindow::changeEvent(e);
    if (e->type() == QEvent::ActivationChange) {
        update();
    }
    else if (e->type() == QEvent::WindowStateChange) {
        if (isFullScreen()) {
            window_state = FULLSCREEN;
        }
        else if (isMaximized()) {
            window_state = MAXIMIZED;
        }
        else if (isMinimized()) {
            window_state = MINIMIZED;
        }
        else {
            window_state = NORMAL;
        }
        qInfo("window_state=%d", (int)window_state);
    }
}

#include "ffmpeg_util.h"
void MainWindow::about() {
    QString strAbout = APP_NAME " " APP_VERSION "\n\n";

    strAbout += "Build on ";
    strAbout += QString::asprintf("%s %s\n\n", __DATE__, __TIME__);

    strAbout += "Qt version: ";
    strAbout += qVersion();
    strAbout += "\nFFmpeg version: ";
    strAbout += av_version_info();
    strAbout += "\n\n";

    strAbout += "Copyright 2018-2028 " COMPANY_NAME " Company.\n";
    strAbout += "All rights reserved.\n";

    QMessageBox::information(this, tr("About Application"), strAbout);
}

void MainWindow::onMVStyleSelected(int id) {
    int r,c;
    switch (id) {
#define CASE_MV_STYLE(id, row, col, lable, image) \
    case id: \
        r = row; c = col; \
        break;

    FOREACH_MV_STYLE(CASE_MV_STYLE)
#undef  CASE_MV_STYLE
    default:
        r = 1; c = 1;
        break;
    }
    center->mv->setLayout(r,c);
}

#include "HOpenMediaDlg.h"
void MainWindow::OpenMediaDlg(int index) {
    HOpenMediaDlg dlg(this);
    dlg.tab->setCurrentIndex(index);
    if (dlg.exec() == QDialog::Accepted) {
        center->mv->play(dlg.media);
    }
}
