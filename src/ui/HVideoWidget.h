#ifndef H_VIDEO_WIDGET_H
#define H_VIDEO_WIDGET_H

#include "qtheaders.h"
#include "avdef.h"

#include "HVideoWndFactory.h"
#include "HVideoTitlebar.h"
#include "HVideoToolbar.h"
#include "HVideoPlayerFactory.h"

class HVideoWidget : public QFrame
{
    Q_OBJECT
public:
    enum Status {
        STOP,
        PAUSE,
        PLAY,
    };

    explicit HVideoWidget(QWidget *parent = nullptr);
    ~HVideoWidget();

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

    void setAspectRatio(aspect_ratio_t aspect_ratio);

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
    int     playerid;
    int     status;
    QString title;
    int     fps;
    aspect_ratio_t  aspect_ratio;
    renderer_type_e renderer_type;

    HVideoWnd       *videownd;
    HVideoTitlebar  *titlebar;
    HVideoToolbar   *toolbar;
    QPushButton     *btnMedia;
private:
    QPoint          ptMousePress;
    QTimer*         timer;

    HMedia          media;
    HVideoPlayer*   pImpl_player;
    // for retry when SIGNAL_END_OF_FILE
    int             retry_interval;
    int             retry_maxcnt;
    int64_t         last_retry_time;
    int             retry_cnt;
};

#endif // H_VIDEO_WIDGET_H
