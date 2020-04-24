#include "GLWnd.h"

GLWnd::GLWnd(QWidget* parent) : HVideoWnd(parent), HGLWidget(parent)
{

}

void GLWnd::drawFPS() {
    char szFPS[16];
    sprintf(szFPS, "FPS:%d", fps);

    QPoint pt(width()-100,40);
    drawText(pt, szFPS, 16, Qt::blue);
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
        if (draw_fps) {
            drawFPS();
        }
    }
}
