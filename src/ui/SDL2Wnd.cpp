#include "SDL2Wnd.h"
#include "avdef.h"

std::atomic_flag SDL2Wnd::s_sdl_init = ATOMIC_FLAG_INIT;

static SDL_PixelFormatEnum SDL_pix_fmt(int type) {
    switch(type) {
    case PIX_FMT_IYUV: return SDL_PIXELFORMAT_IYUV;
    case PIX_FMT_YV12: return SDL_PIXELFORMAT_YV12;
    case PIX_FMT_NV12: return SDL_PIXELFORMAT_NV12;
    case PIX_FMT_NV21: return SDL_PIXELFORMAT_NV21;
    case PIX_FMT_YUY2: return SDL_PIXELFORMAT_YUY2;
    case PIX_FMT_YVYU: return SDL_PIXELFORMAT_YVYU;
    case PIX_FMT_UYVY: return SDL_PIXELFORMAT_UYVY;
    case PIX_FMT_RGB:  return SDL_PIXELFORMAT_RGB24;
    case PIX_FMT_BGR:  return SDL_PIXELFORMAT_BGR24;
    case PIX_FMT_RGBA: return SDL_PIXELFORMAT_RGBA8888;
    case PIX_FMT_BGRA: return SDL_PIXELFORMAT_RGBA8888;
    case PIX_FMT_ARGB: return SDL_PIXELFORMAT_ARGB8888;
    case PIX_FMT_ABGR: return SDL_PIXELFORMAT_ABGR8888;
    }
    return SDL_PIXELFORMAT_UNKNOWN;
}

SDL2Wnd::SDL2Wnd(QWidget* parent) : HVideoWnd(parent), QWidget(parent)
{
    if (!s_sdl_init.test_and_set()) {
        SDL_Init(SDL_INIT_VIDEO);
    }

    setAttribute(Qt::WA_NativeWindow);
    sdlWindow = SDL_CreateWindowFrom((void*)winId());
    sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!sdlRenderer) {
        hlogw("Failed to initialize a hardware accelerated renderer: %s", SDL_GetError());
        sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, 0);
    }
    if (sdlRenderer) {
        memset(&renderer_info, 0, sizeof(SDL_RendererInfo));
        if (SDL_GetRendererInfo(sdlRenderer, &renderer_info) == 0) {
            hlogd("Initialized %s renderer.", renderer_info.name);
        }
    }
    if (!sdlWindow || !sdlRenderer || !renderer_info.num_texture_formats) {
        hloge("Failed to create window or renderer: %s", SDL_GetError());
        exit(-1);
    }
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    sdlTexture = NULL;
    tex_w = tex_h = 0;
}

SDL2Wnd::~SDL2Wnd() {
    if (sdlTexture) {
        SDL_DestroyTexture(sdlTexture);
        sdlTexture = NULL;
    }
    if (sdlRenderer) {
        SDL_DestroyRenderer(sdlRenderer);
        sdlRenderer = NULL;
    }
    if (sdlWindow) {
        SDL_DestroyWindow(sdlWindow);
        sdlWindow = NULL;
    }
}

void SDL2Wnd::update() {
    // QWidget::update();
    paintEvent(NULL);
}

void SDL2Wnd::paintEvent(QPaintEvent* e) {
    calcFPS();
    SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 255);
    SDL_RenderClear(sdlRenderer);

    if (last_frame.isNull()) {
        // NO VIDEO
    }
    else {
        if (sdlTexture == NULL ||
            tex_w != last_frame.w ||
            tex_h != last_frame.h ||
            tex_pix_fmt != last_frame.type) {
            if (sdlTexture) {
                SDL_DestroyTexture(sdlTexture);
                sdlTexture = NULL;
            }
            sdl_pix_fmt = SDL_pix_fmt(last_frame.type);
            sdlTexture = SDL_CreateTexture(sdlRenderer, sdl_pix_fmt, SDL_TEXTUREACCESS_STREAMING, last_frame.w, last_frame.h);
            if (sdlTexture == NULL) {
                hloge("SDL_CreateTexture");
                return;
            }
            void* pixels;
            int pitch;
            if (SDL_LockTexture(sdlTexture, NULL, &pixels, &pitch) < 0) return;
            memset(pixels, 0, pitch * last_frame.h);
            SDL_UnlockTexture(sdlTexture);

            tex_w = last_frame.w;
            tex_h = last_frame.h;
            tex_pix_fmt = last_frame.type;
            tex_bpp = pix_fmt_bpp(last_frame.type);
            tex_pitch = pitch;
            hlogi("SDL_Texture w=%d h=%d bpp=%d pitch=%d pix_fmt=%d",
                tex_w, tex_h, tex_bpp, tex_pitch, tex_pix_fmt);
        }
#if 0
        void* pixels;
        int pitch;
        if (SDL_LockTexture(sdlTexture, NULL, &pixels, &pitch) < 0) return;
        memcpy(pixels, last_frame.buf.base, last_frame.buf.len);
        SDL_UnlockTexture(sdlTexture);
#else
        SDL_UpdateTexture(sdlTexture, NULL, last_frame.buf.base, tex_pitch);
        // SDL_UpdateYUVTexture(sdlTexture, NULL, data[0], linesize[0], data[1], linesize[1], data[2], linesize[2]);
#endif
        SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, NULL);
    }

    SDL_RenderPresent(sdlRenderer);
}

void SDL2Wnd::resizeEvent(QResizeEvent* e) {

}
