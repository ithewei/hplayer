#ifndef H_VIDEO_WND_FACTORY_H
#define H_VIDEO_WND_FACTORY_H

#include "HVideoWnd.h"
#include "GLWnd.h"
#include "SDL2Wnd.h"

enum renderer_type_e {
    RENDERER_TYPE_OPENGL,
    RENDERER_TYPE_SDL2,
    // Windows only
    // RENDERER_TYPE_D3D,
    // RENDERER_TYPE_DDRAW,
    // RENDERER_TYPE_GDI,
};
#define DEFAULT_RENDERER_TYPE RENDERER_TYPE_OPENGL

class HVideoWndFactory {
public:
    static HVideoWnd* create(renderer_type_e type = DEFAULT_RENDERER_TYPE, QWidget* parent = NULL) {
        switch(type) {
        case RENDERER_TYPE_OPENGL:
            return new GLWnd(parent);
        case RENDERER_TYPE_SDL2:
            return new SDL2Wnd(parent);
        default:
            return NULL;
        }
    }
};

#endif // H_VIDEO_WND_FACTORY_H
