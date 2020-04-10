#ifndef H_VIDEO_WIDGET_H
#define H_VIDEO_WIDGET_H

#include "qtheaders.h"

#include "HVideoWnd.h"
#include "HVideoTitlebar.h"
#include "HVideoToolbar.h"
#include "HVideoPlayer.h"

class HVideoWidget : public QFrame
{
    Q_OBJECT
public:
    enum Status {
        STOP,
        PAUSE,
        PLAY,
    };

    enum AspectRatio {
        SPREAD,
        ORIGINAL_RATIO,
        CUSTOM_RATIO
    };

    explicit HVideoWidget(QWidget *parent = nullptr);
    ~HVideoWidget();

    void setVideoArea() {videoWnd->setGeometry(rect() - QMargins(1,1,1,1));}
    QImage snap() {return videoWnd->grabFramebuffer();}

signals:

public slots:
    void open(HMedia& media);
    void close();

    void start();
    void stop();
    void pause();
    void resume();
    void restart();
    void retry();

    void onTimerUpdate();
    void onOpenSucceed();
    void onOpenFailed();
    void onPlayerEOF();
    void onPlayerError();

    void setAspectRatio(AspectRatio e, double ratio = 0.0);

protected:
    void initUI();
    void initConnect();
    void updateUI();

    virtual void resizeEvent(QResizeEvent *e);
    virtual void enterEvent(QEvent *e);
    virtual void leaveEvent(QEvent *e);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
    virtual void mouseMoveEvent(QMouseEvent *e);
    virtual void customEvent(QEvent* e);

public:
    int playerid;
    int status;
    QString title;
    int fps;
    AspectRatio eAspectRatio;
    double  aspect_ratio;
private:
    QPoint ptMousePress;

    HVideoWnd *videoWnd;
    HVideoTitlebar *titlebar;
    HVideoToolbar *toolbar;
    QPushButton *btnMedia;

    QTimer *timer;

    HMedia        media;
    HVideoPlayer* pImpl_player;
    // for retry when SIGNAL_END_OF_FILE
    int           retry_interval;
    int           retry_maxcnt;
    int64_t       last_retry_time;
    int           retry_cnt;
};

#endif // H_VIDEO_WIDGET_H
