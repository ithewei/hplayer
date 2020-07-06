#ifndef H_VIDEO_WND_H
#define H_VIDEO_WND_H

#include <qtheaders.h>

class HVideoWnd {
public:
    HVideoWnd(QWidget* parent = nullptr);
    virtual ~HVideoWnd() {}

    virtual void setGeometry(const QRect& rc) = 0;
    virtual void update() = 0;

protected:
    void calcFPS();

public:
    HFrame      last_frame;
    int         fps;
    bool        draw_time;
    bool        draw_fps;
    bool        draw_resolution;
protected:
    // for calFPS
    uint64_t    tick;
    int         framecnt;
};

#endif // H_VIDEO_WND_H
