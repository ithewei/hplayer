#include "mainwindow.h"
#include "appdef.h"
#include "qtstyles.h"

SINGLETON_IMPL(MainWindow)

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    status = NORMAL;
    initUI();
    initConnect();
}

MainWindow::~MainWindow() {
    hlogd("~MainWindow");
}

void MainWindow::initMenu(){
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
    actMenubar->setChecked(true);
    connect( actMenubar, &QAction::triggered, [=](bool check){
        menuBar()->setVisible(check);
    });
    viewMenu->addAction(actMenubar);

    QAction *actToolbar = new QAction(tr(" Toolbar"));
    actToolbar->setCheckable(true);
    actToolbar->setChecked(true);
    connect( actToolbar, &QAction::triggered, [=](bool check){
        foreach(auto toolbar, toolbars)
            toolbar->setVisible(check);
    });
    viewMenu->addAction(actToolbar);

    QAction *actStatusbar = new QAction(tr(" Statusbar"));
    actStatusbar->setCheckable(true);
    actStatusbar->setChecked(true);
    connect( actStatusbar, &QAction::triggered, [=](bool check){
        statusBar()->setVisible(check);
    });
    viewMenu->addAction(actStatusbar);

    // Help
    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(tr(" &About"), this, SLOT(about()));
}

void MainWindow::initUI(){
    setWindowIcon(QIcon(":/image/icon.png"));
    setBaseSize(MAIN_WINDOW_WIDTH, MAIN_WINDOW_HEIGHT);
    centerWidget(this);

    initMenu();

    center = new CentralWidget;
    setCentralWidget(center);

    statusBar()->showMessage(tr("No Message!"));
}

void MainWindow::initConnect(){

}

void MainWindow::fullscreen(){
    static QRect rcOld;
    if (isFullScreen()){
        menuBar()->setVisible(true);
        showNormal();
        setGeometry(rcOld);
        status = NORMAL;
    }else{
        rcOld = geometry();
        menuBar()->setVisible(false);
        showFullScreen();
        status = FULLSCREEN;
    }
    actFullscreen->setChecked(isFullScreen());
    actMenubar->setChecked(menuBar()->isVisible());
}

void MainWindow::mv_fullscreen(){
    HMultiView* mv = center->mv;
    if (mv->windowType() & Qt::Window){
        mv->setWindowFlags(Qt::SubWindow);
        this->show();
        status = NORMAL;
    }else{
        mv->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
        this->hide();
        mv->showFullScreen();
        mv->raise();
        status = MV_FULLSCREEN;
    }
    this->grabKeyboard();

    actMvFullscreen->setChecked(status == MV_FULLSCREEN);
}

void MainWindow::keyPressEvent(QKeyEvent* e){
    if (status == MV_FULLSCREEN){
        if (e->key() == Qt::Key_F12 || e->key() == Qt::Key_Escape)
            mv_fullscreen();
    }else{
        switch(e->key()){
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

void MainWindow::about(){
    QString strAbout = APP_NAME " " APP_VERSION "\n\n";

    strAbout += "Build on ";
    strAbout += QString::asprintf("%s %s\n\n", __DATE__, __TIME__);

    strAbout += "Copyright 2018-2028 " COMPANY_NAME " Company.\n";
    strAbout += "All rights reserved.\n";

    QMessageBox::information(this, tr("About Application"), strAbout);
}

void MainWindow::onMVStyleSelected(int id){
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

#include "hopenmediadlg.h"
void MainWindow::OpenMediaDlg(int index){
    HOpenMediaDlg dlg(this);
    dlg.tab->setCurrentIndex(index);
    if (dlg.exec() == QDialog::Accepted){
        center->mv->play(dlg.media);
    }
}
