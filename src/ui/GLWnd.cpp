#include "GLWnd.h"

GLWnd::GLWnd(QWidget* parent) : HVideoWnd(parent), HGLWidget(parent)
{

}

void GLWnd::drawTime() {
    char szTime[12];
    duration_fmt(last_frame.ts / 1000, szTime);
    // Left Top
    QPoint pt(10, 40);
    drawText(pt, szTime, 14, Qt::white);
}

void GLWnd::drawFPS() {
    char szFPS[16];
    sprintf(szFPS, "FPS:%d", fps);
    // Right Top
    QPoint pt(width()-100, 40);
    drawText(pt, szFPS, 14, Qt::blue);
}

void GLWnd::drawResolution() {
    char szResolution[16];
    sprintf(szResolution, "%d X %d", last_frame.w, last_frame.h);
    // Left Bottom
    QPoint pt(10, height() - 10);
    drawText(pt, szResolution, 14, Qt::blue);
}

void GLWnd::paintGL() {
    calcFPS();
    HGLWidget::paintGL();

    if (last_frame.isNull()) {
        /*
        QPoint pt = rect().center() - QPoint(80, -10);
        drawText(pt, "NO VIDEO", 16, Qt::white);

        QPainter painter(this);
        QPixmap pixmap(":/image/media_bk.png");
        int w = pixmap.width();
        int h = pixmap.height();
        QRect rc((width()-w)/2, (height()-h)/2, w, h);
        painter.drawPixmap(rc, pixmap);
        */
    }
    else {
        drawFrame(&last_frame);
        if (draw_time) {
            drawTime();
        }
        if (draw_fps) {
            drawFPS();
        }
        if (draw_resolution) {
            drawResolution();
        }
    }
}
