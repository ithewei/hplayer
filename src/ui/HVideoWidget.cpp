#include "HVideoWidget.h"

#include "qtstyles.h"

#include "HOpenMediaDlg.h"
#include "HVideoPlayerFactory.h"

#define DEFAULT_RETRY_INTERVAL  3000  // ms
#define DEFAULT_RETRY_MAXCNT    100

HVideoWidget::HVideoWidget(QWidget *parent) : QFrame(parent)
{
    playerid = 0;
    status = STOP;
    pImpl_player = NULL;
    // retry
    retry_interval = DEFAULT_RETRY_INTERVAL;
    max_retry_cnt = DEFAULT_RETRY_MAXCNT;
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
        goto end;
    }

    if (!pImpl_player) {
        pImpl_player = HVideoPlayerFactory::create(media.type);
        pImpl_player->set_media(media);
        if (pImpl_player->start() != 0) {
            QMessageBox::critical(this, tr("ERROR"), tr("Could not open media: \n")
                                  + media.src.c_str() + QString::asprintf("[%d]", media.index));
            SAFE_DELETE(pImpl_player);
            goto end;
        }
        title = media.src.c_str();
        if (pImpl_player->duration > 0) {
            toolbar->lblDuration->setText(strtime(pImpl_player->duration).c_str());
            toolbar->sldProgress->setRange(0, pImpl_player->duration/1000);
            toolbar->lblDuration->show();
            toolbar->sldProgress->show();
        }
        else {
            toolbar->lblDuration->hide();
            toolbar->sldProgress->hide();
        }
    }
    else {
        pImpl_player->resume();
    }

    timer->start(1000 / pImpl_player->fps);
    status = PLAY;

end:
    updateUI();
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

void HVideoWidget::onTimerUpdate() {
    if (pImpl_player) {
        if (pImpl_player->pop_frame(&videoWnd->last_frame) == 0) {
            int progress = (videoWnd->last_frame.ts - pImpl_player->start_time) / 1000;
            if (toolbar->sldProgress->value() != progress &&
                !toolbar->sldProgress->isSliderDown()) {
                toolbar->sldProgress->setValue(progress);
            }
            videoWnd->update();
        }
        else {
            if (pImpl_player->flags == EOF) {
                stop();
                return;
            }
            if (pImpl_player->error != 0) {
                switch (media.type) {
                case MEDIA_TYPE_NETWORK:
                    // retry
                    if (retry_cnt < max_retry_cnt) {
                        uint64_t cur_time = timestamp_ms();
                        if (cur_time - last_retry_time > retry_interval) {
                            ++retry_cnt;
                            last_retry_time = cur_time;
                            pImpl_player->stop();
                            int ret = pImpl_player->start();
                            if (ret != 0) {
                                hlogw("retry cnt=%d media.src=%s failed!", retry_cnt, media.src.c_str());
                            }
                            else {
                                hlogi("retry cnt=%d media.src=%s succeed!", retry_cnt, media.src.c_str());
                                retry_cnt = 0;
                            }
                        }
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
        }
    }
}
