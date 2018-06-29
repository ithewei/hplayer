#include "hvideowidget.h"
#include "hvideoplayerfactory.h"
#include "qtstyles.h"

HVideoWidget::HVideoWidget(QWidget *parent) : QFrame(parent)
{
    playerid = 0;
    status = STOP;
    pImpl_player = NULL;

    initUI();
    initConnect();
}

HVideoWidget::~HVideoWidget(){
    close();
}

void HVideoWidget::initUI(){
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

void HVideoWidget::initConnect(){
    connect( btnMedia, SIGNAL(clicked(bool)), this, SLOT(onBtnMedia()) );
    connect( titlebar->btnClose, SIGNAL(clicked(bool)), this, SLOT(close()) );

    connect( toolbar, SIGNAL(sigStart()), this, SLOT(start()) );
    connect( toolbar, SIGNAL(sigPause()), this, SLOT(pause()) );
    connect( toolbar, SIGNAL(sigStop()), this, SLOT(stop()) );

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimerUpdate()));
}

void HVideoWidget::updateUI(){
    titlebar->labTitle->setText(QString::asprintf("%02d ", playerid) + title);

    toolbar->btnStart->setVisible(status != PLAY);
    toolbar->btnPause->setVisible(status == PLAY);

    btnMedia->setVisible(status == STOP);
}

void HVideoWidget::resizeEvent(QResizeEvent *e){
    setVideoArea();
}

void HVideoWidget::enterEvent(QEvent *e){
    updateUI();

    titlebar->show();
    toolbar->show();
}

void HVideoWidget::leaveEvent(QEvent *e){
    titlebar->hide();
    toolbar->hide();
}

void HVideoWidget::mousePressEvent(QMouseEvent *e){
    ptMousePress = e->pos();
#if WITH_MV_STYLE
    e->ignore();
#endif
}

void HVideoWidget::mouseReleaseEvent(QMouseEvent *e){
#if WITH_MV_STYLE
    e->ignore();
#endif
}

void HVideoWidget::mouseMoveEvent(QMouseEvent *e){
    if (e->buttons() == Qt::LeftButton){

    }
#if WITH_MV_STYLE
    e->ignore();
#endif
}

void HVideoWidget::open(HMedia& media){
    this->media = media;
    start();
}

void HVideoWidget::close(){
    stop();
    this->media.type = MEDIA_TYPE_NONE;
    title = "";
    updateUI();
}

void HVideoWidget::start(){
    if (media.type == MEDIA_TYPE_NONE){
        QMessageBox::information(this, tr("Info"), tr("Please first set media source, then start."));
        goto end;
    }

    if (!pImpl_player){
        pImpl_player = HVideoPlayerFactory::create(media.type);
        pImpl_player->set_media(media);
        //pImpl_player->set_frame_cache(5);
        if (pImpl_player->start() != 0){
            QMessageBox::critical(this, tr("ERROR"), tr("Could not open media: \n")
                                  + media.src.c_str() + QString::asprintf("[%d]", media.index));
            SAFE_DELETE(pImpl_player);
            goto end;
        }
        title = media.src.c_str();
    }else{
        pImpl_player->resume();
    }

    timer->start(1000/DEFAULT_FPS);
    status = PLAY;

end:
    updateUI();
}

void HVideoWidget::stop(){
    timer->stop();

    if (pImpl_player){
        pImpl_player->stop();
        SAFE_DELETE(pImpl_player);
    }

    videoWnd->last_frame.buf.cleanup();
    videoWnd->update();
    status = STOP;

    updateUI();
}

void HVideoWidget::pause(){
    if (pImpl_player){
        pImpl_player->pause();
    }
    timer->stop();
    status = PAUSE;

    updateUI();
}

void HVideoWidget::onTimerUpdate(){
    if (pImpl_player){
        if (pImpl_player->pop_frame(&videoWnd->last_frame) == 0){
            videoWnd->update();
        }
    }
}

#include "hopenmediadlg.h"
void HVideoWidget::onBtnMedia(){
    HOpenMediaDlg dlg(this);
    if (dlg.exec() == QDialog::Accepted){
        open(dlg.media);
    }
}
