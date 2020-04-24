#include "HVideoWidget.h"

#include "confile.h"
#include "qtstyles.h"

#include "HOpenMediaDlg.h"

#define DEFAULT_RETRY_INTERVAL  10000  // ms
#define DEFAULT_RETRY_MAXCNT    6

#include "CustomEventType.h"
static int hplayer_event_callback(hplayer_event_e e, void* userdata) {
    HVideoWidget* wdg = (HVideoWidget*)userdata;
    int custom_event_type = QCustomEvent::User;
    switch (e) {
    case HPLAYER_OPENED:
        custom_event_type = QCustomEvent::OpenMediaSucceed;
        break;
    case HPLAYER_OPEN_FAILED:
        custom_event_type = QCustomEvent::OpenMediaFailed;
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

static renderer_type_e renderer_type_enum(const char* str) {
    if (stricmp(str, "opengl") == 0) {
        return RENDERER_TYPE_OPENGL;
    }
    else if (stricmp(str, "sdl") == 0 || stricmp(str, "sdl2") == 0) {
        return RENDERER_TYPE_SDL2;
    }
    /*
    else if (stricmp(str, "d3d") == 0) {
        return RENDERER_TYPE_D3D;
    }
    */
    return DEFAULT_RENDERER_TYPE;
}

HVideoWidget::HVideoWidget(QWidget *parent) : QFrame(parent)
{
    playerid = 0;
    status = STOP;
    pImpl_player = NULL;
    fps = g_confile->Get<int>("fps", "video");
    // aspect_ratio
    string str = g_confile->GetValue("aspect_ratio", "video");
    if (str.empty() || strcmp(str.c_str(), "100%") == 0) {
        aspect_type = ASPECT_SPREAD;
    }
    else if (stricmp(str.c_str(), "w:h") == 0) {
        aspect_type = ASPECT_ORIGINAL_RATIO;
    }
    else {
        int x = 0;
        int y = 0;
        sscanf(str.c_str(), "%d:%d", &x, &y);
        if (x && y) {
            aspect_type = ASPECT_CUSTOM_RATIO;
            aspect_ratio = (double)x / (double)y;
        }
        else {
            aspect_type = ASPECT_SPREAD;
        }
    }
    // renderer
    str = g_confile->GetValue("renderer", "video");
    if (str.empty()) {
        renderer_type = RENDERER_TYPE_OPENGL;
    }
    else {
        renderer_type = renderer_type_enum(str.c_str());
    }
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

    videownd = HVideoWndFactory::create(renderer_type, this);
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
    videownd->setGeometry(rect() - QMargins(1,1,1,1));
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
        updateUI();
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

    videownd->last_frame.buf.cleanup();
    videownd->update();
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
        timer->start(1000 / (fps ? fps : pImpl_player->fps));
        status = PLAY;

        updateUI();
    }
}

void HVideoWidget::restart() {
    hlogi("restart...");
    if (pImpl_player) {
        pImpl_player->stop();
        pImpl_player->start();
    }
    else {
        start();
    }
}

void HVideoWidget::retry() {
    if (retry_maxcnt < 0 || retry_cnt < retry_maxcnt) {
        ++retry_cnt;
        int64_t cur_time = timestamp_ms();
        int64_t timespan = cur_time - last_retry_time;
        if (timespan >= retry_interval) {
            last_retry_time = cur_time;
            restart();
        }
        else {
            last_retry_time += retry_interval;
            if (pImpl_player) {
                pImpl_player->stop();
            }
            int retry_after = retry_interval - timespan;
            hlogi("retry after %dms", retry_after);
            QTimer::singleShot(retry_after, this, SLOT(restart()));
        }
    }
    else {
        stop();
    }
}

void HVideoWidget::onOpenSucceed() {
    timer->start(1000 / (fps ? fps : pImpl_player->fps));
    status = PLAY;
    setAspectRatio(aspect_type, aspect_ratio);
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
        retry();
    }
}

void HVideoWidget::onPlayerEOF() {
    switch (media.type) {
    case MEDIA_TYPE_NETWORK:
        retry();
        break;
    case MEDIA_TYPE_FILE:
        if (g_confile->Get<bool>("loop_playback", "video")) {
            restart();
        }
        else {
            stop();
        }
        break;
    default:
        stop();
        break;
    }
}

void HVideoWidget::onPlayerError() {
    switch (media.type) {
    case MEDIA_TYPE_NETWORK:
        retry();
        break;
    default:
        stop();
        break;
    }
}

void HVideoWidget::onTimerUpdate() {
    if (pImpl_player == NULL)   return;

    if (pImpl_player->pop_frame(&videownd->last_frame) == 0) {
        // update progress bar
        if (toolbar->sldProgress->isVisible()) {
            int progress = (videownd->last_frame.ts - pImpl_player->start_time) / 1000;
            if (toolbar->sldProgress->value() != progress &&
                !toolbar->sldProgress->isSliderDown()) {
                toolbar->sldProgress->setValue(progress);
            }
        }
        // update video frame
        videownd->update();
    }
}

void HVideoWidget::setAspectRatio(aspect_ratio_e type, double ratio) {
    aspect_type = type;
    switch(type) {
    case ASPECT_SPREAD:
        aspect_ratio = 0.0;
        break;
    case ASPECT_ORIGINAL_RATIO:
        if (pImpl_player && pImpl_player->height > 0) {
            aspect_ratio = (double)pImpl_player->width / (double)pImpl_player->height;
        }
        break;
    case ASPECT_CUSTOM_RATIO:
        aspect_ratio = ratio;
        break;
    }
    if (videownd) {
        videownd->setAspectRatio(aspect_ratio);
    }
}
