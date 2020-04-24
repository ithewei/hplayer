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

    virtual void setAspectRatio(double ratio) {
        HGLWidget::setAspectRatio(ratio);
    }

protected:
    virtual void drawFPS();
    virtual void paintGL();
};

#endif // GL_WND_H
