#ifndef HVIDEOWIDGET_H
#define HVIDEOWIDGET_H

#include "qtheaders.h"
#include "hvideownd.h"
#include "hvideotitlebar.h"
#include "hvideotoolbar.h"
#include "hvideoplayer.h"

class  HVideoWidget : public QFrame
{
    Q_OBJECT
public:
    enum Status{
        STOP,
        PAUSE,
        PLAY,
    };

    explicit HVideoWidget(QWidget *parent = nullptr);
    ~HVideoWidget();

    void setVideoArea() {videoWnd->setGeometry(rect() - QMargins(1,1,1,1));}
    QImage snap() {return videoWnd->grabFramebuffer();}

signals:

public slots:
    void onBtnMedia();

    void open(HMedia& media);
    void close();

    void start();
    void pause();
    void stop();

    void onTimerUpdate();

protected:
    void initUI();
    void initConnect();
    void updateUI();

    void resizeEvent(QResizeEvent *e);
    void enterEvent(QEvent *e);
    void leaveEvent(QEvent *e);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
    virtual void mouseMoveEvent(QMouseEvent *e);

public:
    int playerid;
    int status;
    QString title;
private:
    QPoint ptMousePress;

    HVideoWnd *videoWnd;
    HVideoTitlebar *titlebar;
    HVideoToolbar *toolbar;
    QPushButton *btnMedia;

    QTimer *timer;

    HMedia        media;
    HVideoPlayer* pImpl_player;
};

#endif // HVIDEOWIDGET_H
