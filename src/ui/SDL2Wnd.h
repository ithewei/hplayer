#ifndef SDL2_WND_H
#define SDL2_WND_H

#include "HVideoWnd.h"
#include "sdl_util.h"

class SDL2Wnd : public HVideoWnd, QWidget {
public:
    SDL2Wnd(QWidget* parent = nullptr);
    ~SDL2Wnd();

    virtual void setGeometry(const QRect& rc) {
        QWidget::setGeometry(rc);
    }

    virtual void update();

protected:
    virtual void paintEvent(QPaintEvent* e);
    virtual void resizeEvent(QResizeEvent* e);

    static std::atomic_flag s_sdl_init;
    SDL_Window*             sdlWindow;
    SDL_Renderer*           sdlRenderer;
    SDL_Texture*            sdlTexture;
    SDL_RendererInfo        renderer_info;
    int     tex_w;
    int     tex_h;
    int     tex_pix_fmt;
    int     tex_bpp;
    int     tex_pitch;
    SDL_PixelFormatEnum     sdl_pix_fmt;
};

#endif // SDL2_WND_H
