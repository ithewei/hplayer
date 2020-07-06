#ifndef GL_WND_H
#define GL_WND_H

#include "HVideoWnd.h"
#include "HGLWidget.h"

class GLWnd : public HVideoWnd, HGLWidget {
public:
    GLWnd(QWidget* parent = nullptr);

    virtual void setGeometry(const QRect& rc) {
        HGLWidget::setGeometry(rc);
    }

    virtual void update() {
        HGLWidget::update();
    }

protected:
    virtual void paintGL();
    void drawTime();
    void drawFPS();
    void drawResolution();
};

#endif // GL_WND_H
