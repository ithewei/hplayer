#include "HVideoWidget.h"

#include "confile.h"
#include "qtstyles.h"

#include "HOpenMediaDlg.h"
#include "HVideoPlayerFactory.h"

#define DEFAULT_RETRY_INTERVAL  3000  // ms
#define DEFAULT_RETRY_MAXCNT    100

#include "CustomEventType.h"
static int hplayer_event_callback(hplayer_event_e e, void* userdata) {
    HVideoWidget* wdg = (HVideoWidget*)userdata;
    int custom_event_type = QCustomEvent::User;
    switch (e) {
    case HPLAYER_OPEN_FAILED:
        custom_event_type = QCustomEvent::OpenMediaFailed;
        break;
    case HPLAYER_OPENED:
        custom_event_type = QCustomEvent::OpenMediaSucceed;
        break;
    case HPLAYER_EOF:
        custom_event_type = QCustomEvent::PlayerEOF;
        break;
    case HPLAYER_ERROR:
        custom_event_type = QCustomEvent::PlayerError;
        break;
    default:
        return 0;
    }
    hlogi("postEvent %d", custom_event_type);
    QApplication::postEvent(wdg, new QEvent((QEvent::Type)custom_event_type));
    return 0;
}

HVideoWidget::HVideoWidget(QWidget *parent) : QFrame(parent)
{
    playerid = 0;
    status = STOP;
    pImpl_player = NULL;
    // retry
    retry_interval = g_confile->Get<int>("retry_interval", "video", DEFAULT_RETRY_INTERVAL);
    retry_maxcnt = g_confile->Get<int>("retry_maxcnt", "video", DEFAULT_RETRY_MAXCNT);
    last_retry_time = 0;
    retry_cnt = 0;

    initUI();
    initConnect();
}

HVideoWidget::~HVideoWidget() {
    hlogd("~HVideoWidget");
    close();
}

void HVideoWidget::initUI() {
    setFocusPolicy(Qt::ClickFocus);

    videoWnd = new HVideoWnd(this);
    titlebar = new HVideoTitlebar(this);
    toolbar  = new HVideoToolbar(this);
    btnMedia = genPushButton(QPixmap(":/image/media_bk.png"), tr("Open media"));

    QVBoxLayout *vbox = genVBoxLayout();

    vbox->addWidget(titlebar, 0, Qt::AlignTop);
    vbox->addWidget(btnMedia, 0, Qt::AlignCenter);
    vbox->addWidget(toolbar, 0, Qt::AlignBottom);

    setLayout(vbox);

    titlebar->hide();
    toolbar->hide();
}

void HVideoWidget::initConnect() {
    connect( btnMedia, &QPushButton::clicked, [this] {
        HOpenMediaDlg dlg(this);
        if (dlg.exec() == QDialog::Accepted) {
            open(dlg.media);
        }
    });
    connect( titlebar->btnClose, SIGNAL(clicked(bool)), this, SLOT(close()) );

    connect( toolbar, SIGNAL(sigStart()), this, SLOT(start()) );
    connect( toolbar, SIGNAL(sigPause()), this, SLOT(pause()) );
    connect( toolbar, SIGNAL(sigStop()),  this, SLOT(stop())  );
    connect( toolbar->sldProgress, &QSlider::sliderReleased, [this]() {
        if (pImpl_player) {
            pImpl_player->seek(toolbar->sldProgress->value()*1000);
        }
    });

    timer = new QTimer(this);
    timer->setTimerType(Qt::PreciseTimer);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimerUpdate()));
}

void HVideoWidget::updateUI() {
    titlebar->labTitle->setText(QString::asprintf("%02d ", playerid) + title);

    toolbar->btnStart->setVisible(status != PLAY);
    toolbar->btnPause->setVisible(status == PLAY);

    btnMedia->setVisible(status == STOP);

    if (status == STOP) {
        toolbar->sldProgress->hide();
        toolbar->lblDuration->hide();
    }
}

void HVideoWidget::resizeEvent(QResizeEvent *e) {
    setVideoArea();
}

void HVideoWidget::enterEvent(QEvent *e) {
    updateUI();

    titlebar->show();
    toolbar->show();
}

void HVideoWidget::leaveEvent(QEvent *e) {
    titlebar->hide();
    toolbar->hide();
}

void HVideoWidget::mousePressEvent(QMouseEvent *e) {
    ptMousePress = e->pos();
#if WITH_MV_STYLE
    e->ignore();
#endif
}

void HVideoWidget::mouseReleaseEvent(QMouseEvent *e) {
#if WITH_MV_STYLE
    e->ignore();
#endif
}

void HVideoWidget::mouseMoveEvent(QMouseEvent *e) {
#if WITH_MV_STYLE
    e->ignore();
#endif
}

void HVideoWidget::customEvent(QEvent* e) {
    switch(e->type()) {
    case QCustomEvent::OpenMediaSucceed:
        onOpenSucceed();
        break;
    case QCustomEvent::OpenMediaFailed:
        onOpenFailed();
        break;
    case QCustomEvent::PlayerEOF:
        onPlayerEOF();
        break;
    case QCustomEvent::PlayerError:
        onPlayerError();
        break;
    default:
        break;
    }
}

void HVideoWidget::open(HMedia& media) {
    this->media = media;
    start();
}

void HVideoWidget::close() {
    stop();
    this->media.type = MEDIA_TYPE_NONE;
    title = "";
    updateUI();
}

void HVideoWidget::start() {
    if (media.type == MEDIA_TYPE_NONE) {
        QMessageBox::information(this, tr("Info"), tr("Please first set media source, then start."));
        updateUI();
        return;
    }

    if (!pImpl_player) {
        pImpl_player = HVideoPlayerFactory::create(media.type);
        pImpl_player->set_media(media);
        pImpl_player->set_event_callback(hplayer_event_callback, this);
        title = media.src.c_str();
        int ret = pImpl_player->start();
        if (ret != 0) {
            onOpenFailed();
        }
        else {
            onOpenSucceed();
        }
    }
    else {
        if (status == PAUSE) {
            resume();
        }
    }
}

void HVideoWidget::stop() {
    timer->stop();

    if (pImpl_player) {
        pImpl_player->stop();
        SAFE_DELETE(pImpl_player);
    }

    videoWnd->last_frame.buf.cleanup();
    videoWnd->update();
    status = STOP;

    last_retry_time = 0;
    retry_cnt = 0;

    updateUI();
}

void HVideoWidget::pause() {
    if (pImpl_player) {
        pImpl_player->pause();
    }
    timer->stop();
    status = PAUSE;

    updateUI();
}

void HVideoWidget::resume() {
    if (status == PAUSE && pImpl_player) {
        pImpl_player->resume();
        timer->start(1000 / pImpl_player->fps);
        status = PLAY;

        updateUI();
    }
}

void HVideoWidget::restart() {
    if (retry_cnt < retry_maxcnt) {
        uint64_t cur_time = timestamp_ms();
        if (cur_time - last_retry_time > retry_interval) {
            ++retry_cnt;
            last_retry_time = cur_time;
            if (pImpl_player) {
                pImpl_player->stop();
                pImpl_player->start();
            }
            else {
                start();
            }
        }
    }
    else {
        stop();
    }
}

void HVideoWidget::onOpenSucceed() {
    timer->start(1000 / pImpl_player->fps);
    status = PLAY;
    if (pImpl_player->duration > 0) {
        toolbar->lblDuration->setText(strtime(pImpl_player->duration).c_str());
        toolbar->sldProgress->setRange(0, pImpl_player->duration/1000);
        toolbar->lblDuration->show();
        toolbar->sldProgress->show();
    }

    if (retry_cnt != 0) {
        hlogi("retry succeed: cnt=%d media.src=%s", retry_cnt, media.src.c_str());
    }
}

void HVideoWidget::onOpenFailed() {
    if (retry_cnt == 0) {
        QMessageBox::critical(this, tr("ERROR"), tr("Could not open media: \n") +
                              media.src.c_str() +
                              QString::asprintf("\nerrcode=%d", pImpl_player->error));
        stop();
    }
    else {
        hlogw("retry failed: cnt=%d media.src=%s", retry_cnt, media.src.c_str());
        restart();
    }
}

void HVideoWidget::onPlayerEOF() {
    stop();
}

void HVideoWidget::onPlayerError() {
    switch (media.type) {
    case MEDIA_TYPE_NETWORK:
        restart();
        break;
    default:
        stop();
        break;
    }
}

void HVideoWidget::onTimerUpdate() {
    if (pImpl_player == NULL)   return;

    if (pImpl_player->pop_frame(&videoWnd->last_frame) == 0) {
        // update progress bar
        if (toolbar->sldProgress->isVisible()) {
            int progress = (videoWnd->last_frame.ts - pImpl_player->start_time) / 1000;
            if (toolbar->sldProgress->value() != progress &&
                !toolbar->sldProgress->isSliderDown()) {
                toolbar->sldProgress->setValue(progress);
            }
        }
        // update video frame
        videoWnd->update();
    }
}
