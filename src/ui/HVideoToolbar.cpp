#include "HVideoToolbar.h"
#include "qtstyles.h"

HVideoToolbar::HVideoToolbar(QWidget *parent) : QFrame(parent)
{
    initUI();
    initConnect();
}

void HVideoToolbar::initUI() {
    setFixedHeight(VIDEO_TOOLBAR_HEIGHT);

    QSize sz(VIDEO_TOOLBAR_ICON_WIDTH, VIDEO_TOOLBAR_ICON_HEIGHT);
    btnStart = genPushButton(QPixmap(":/image/start.png"), tr("start"));
    btnPause = genPushButton(QPixmap(":/image/pause.png"), tr("pause"));
#if !WITH_MV_STYLE
    btnStart->setShortcut(Qt::Key_Space);
    btnPause->setShortcut(Qt::Key_Space);
#endif

    btnPrev  = genPushButton(QPixmap(":/image/prev.png"), tr("prev"));
    btnStop  = genPushButton(QPixmap(":/image/stop.png"), tr("stop"));
    btnStop->setAutoDefault(true);
    btnNext  = genPushButton(QPixmap(":/image/next.png"), tr("next"));

    sldProgress = new QSlider;
    sldProgress->setOrientation(Qt::Horizontal);
    lblDuration = new QLabel("00:00:00");

    QHBoxLayout *hbox = genHBoxLayout();
    hbox->setSpacing(5);
    hbox->addWidget(btnStart, 0, Qt::AlignLeft);
    hbox->addWidget(btnPause, 0, Qt::AlignLeft);
    btnPause->hide();

    hbox->addSpacing(5);
    hbox->addWidget(btnPrev, 0, Qt::AlignLeft);
    btnPrev->hide();
    hbox->addWidget(btnStop, 0, Qt::AlignLeft);
    btnStop->hide();
    hbox->addWidget(btnNext, 0, Qt::AlignLeft);
    btnNext->hide();

    hbox->addSpacing(5);
    hbox->addWidget(sldProgress);
    sldProgress->hide();
    hbox->addWidget(lblDuration);
    lblDuration->hide();

    setLayout(hbox);
}

void HVideoToolbar::initConnect() {
    connectButtons(btnStart, btnPause);

    connect(btnStart, SIGNAL(clicked(bool)), this, SIGNAL(sigStart()));
    connect(btnPause, SIGNAL(clicked(bool)), this, SIGNAL(sigPause()));
    connect(btnStop, SIGNAL(clicked(bool)), this, SIGNAL(sigStop()));

    connect(btnStop, SIGNAL(clicked(bool)), btnStart, SLOT(show()));
    connect(btnStop, SIGNAL(clicked(bool)), btnPause, SLOT(hide()));
}
