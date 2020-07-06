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
    const char* c_str = str.c_str();
    aspect_ratio.type = ASPECT_FULL;
    if (str.empty() || strcmp(c_str, "100%") == 0) {
        aspect_ratio.type = ASPECT_FULL;
    }
    else if (stricmp(c_str, "w:h") == 0) {
        aspect_ratio.type = ASPECT_ORIGINAL_RATIO;
    }
    else if (stricmp(c_str, "wxh") == 0 || stricmp(c_str, "w*h") == 0) {
        aspect_ratio.type = ASPECT_ORIGINAL_SIZE;
    }
    else if (strchr(c_str, '%')) {
        int percent = 0;
        sscanf(c_str, "%d%%", &percent);
        if (percent) {
            aspect_ratio.type = ASPECT_PERCENT;
            aspect_ratio.w = percent;
            aspect_ratio.h = percent;
        }
    }
    else if (strchr(c_str, ':')) {
        int w = 0;
        int h = 0;
        sscanf(c_str, "%d:%d", &w, &h);
        if (w && h) {
            aspect_ratio.type = ASPECT_CUSTOM_RATIO;
            aspect_ratio.w = w;
            aspect_ratio.h = h;
        }
    }
    else if (strchr(c_str, 'x')) {
        int w = 0;
        int h = 0;
        sscanf(c_str, "%dx%d", &w, &h);
        if (w && h) {
            aspect_ratio.type = ASPECT_CUSTOM_SIZE;
            aspect_ratio.w = w;
            aspect_ratio.h = h;
        }
    }
    else if (strchr(c_str, 'X')) {
        int w = 0;
        int h = 0;
        sscanf(c_str, "%dX%d", &w, &h);
        if (w && h) {
            aspect_ratio.type = ASPECT_CUSTOM_SIZE;
            aspect_ratio.w = w;
            aspect_ratio.h = h;
        }
    }
    else if (strchr(c_str, '*')) {
        int w = 0;
        int h = 0;
        sscanf(c_str, "%d*%d", &w, &h);
        if (w && h) {
            aspect_ratio.type = ASPECT_CUSTOM_SIZE;
            aspect_ratio.w = w;
            aspect_ratio.h = h;
        }
    }
    else {
        aspect_ratio.type = ASPECT_FULL;
    }
    hlogd("aspect_ratio type=%d w=%d h=%d", aspect_ratio.type, aspect_ratio.w, aspect_ratio.h);
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
    setAspectRatio(aspect_ratio);
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
        int64_t cur_time = gettimeofday_ms();
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
    setAspectRatio(aspect_ratio);
    if (pImpl_player->duration > 0) {
        int duration_sec = pImpl_player->duration / 1000;
        char szTime[16];
        duration_fmt(duration_sec, szTime);
        toolbar->lblDuration->setText(szTime);
        toolbar->sldProgress->setRange(0, duration_sec);
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

void HVideoWidget::setAspectRatio(aspect_ratio_t ar) {
    aspect_ratio = ar;
    int border = 1;
    int scr_w = width() - border * 2;
    int scr_h = height() - border * 2;
    if (scr_w <= 0 || scr_h <= 0) return;
    int pic_w = 0;
    int pic_h = 0;
    if (pImpl_player && status != STOP) {
        pic_w = pImpl_player->width;
        pic_h = pImpl_player->height;
    }
    if (pic_w == 0) pic_w = scr_w;
    if (pic_h == 0) pic_h = scr_h;
    // calc videownd rect
    int dst_w, dst_h;
    switch (ar.type) {
    case ASPECT_FULL:
        dst_w = scr_w;
        dst_h = scr_h;
        break;
    case ASPECT_PERCENT:
        dst_w = pic_w * ar.w / 100;
        dst_h = pic_h * ar.h / 100;
        break;
    case ASPECT_ORIGINAL_SIZE:
        dst_w = pic_w;
        dst_h = pic_h;
        break;
    case ASPECT_CUSTOM_SIZE:
        dst_w = ar.w;
        dst_h = ar.h;
        break;
    case ASPECT_ORIGINAL_RATIO:
    case ASPECT_CUSTOM_RATIO:
    {
        double scr_ratio = (double)scr_w / (double)scr_h;
        double dst_ratio = 1.0;
        if (ar.type == ASPECT_CUSTOM_RATIO) {
            dst_ratio = (double)ar.w / (double)ar.h;
        }
        else {
            dst_ratio = (double)pic_w / (double)pic_h;
        }
        if (dst_ratio > scr_ratio) {
            dst_w = scr_w;
            dst_h = scr_w / dst_ratio;
        }
        else {
            dst_h = scr_h;
            dst_w = scr_h * dst_ratio;
        }
    }
        break;
    }
    dst_w = MIN(dst_w, scr_w);
    dst_h = MIN(dst_h, scr_h);
    // align 4
    dst_w = dst_w >> 2 << 2;
    dst_h = dst_h >> 2 << 2;

    int x = border + (scr_w - dst_w) / 2;
    int y = border + (scr_h - dst_h) / 2;
    videownd->setGeometry(QRect(x, y, dst_w, dst_h));
}
