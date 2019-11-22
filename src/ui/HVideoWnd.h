#ifndef H_VIDEO_WND_H
#define H_VIDEO_WND_H

#include "qtheaders.h"
#include "hframe.h"

#include "HGLWidget.h"

class HVideoWnd : public HGLWidget
{
    Q_OBJECT
public:
    explicit HVideoWnd(QWidget *parent = nullptr);

signals:

protected:
    void calFps();
    virtual void paintGL();
    virtual void drawFPS();

public slots:

public:
    HFrame last_frame;

    // for calFPS
    uint64_t tick;
    int framecnt;
    int fps;
};

#endif // H_VIDEO_WND_H
