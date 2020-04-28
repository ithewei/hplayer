#ifndef AV_DEF_H
#define AV_DEF_H

typedef enum {
    PIX_FMT_NONE = 0,

    PIX_FMT_GRAY,       // YYYYYYYY

    PIX_FMT_YUV_FIRST = 100,
    PIX_FMT_YUV_PLANAR_FIRST = 200,
    PIX_FMT_IYUV,       // YYYYYYYYUUVV
    PIX_FMT_YV12,       // YYYYVVYYVVUU
    PIX_FMT_NV12,       // YYUVYYYYUVUV
    PIX_FMT_NV21,       // YYVUYYYYVUVU
    PIX_FMT_YUV_PLANAR_LAST,
    PIX_FMT_YUV_PACKED_FIRST = 300,
    PIX_FMT_YUY2,       // YUYVYUYV
    PIX_FMT_YVYU,       // YVYUYVYU
    PIX_FMT_UYVY,       // UYVYUYVY
    PIX_FMT_YUV_PACKED_LAST,
    PIX_FMT_YUV_LAST,

    PIX_FMT_RGB_FIRST = 400,
    PIX_FMT_RGB,        // RGBRGB
    PIX_FMT_BGR,        // BGRBGR
    PIX_FMT_RGBA,       // RGBARGBA
    PIX_FMT_BGRA,       // BGRABGRA
    PIX_FMT_ARGB,       // ARGBARGB
    PIX_FMT_ABGR,       // ABGRABGR
    PIX_FMT_RGB_LAST,
} pix_fmt_e;

static inline bool pix_fmt_is_yuv(int type) {
    return type > PIX_FMT_YUV_FIRST && type < PIX_FMT_YUV_LAST;
}

static inline bool pix_fmt_is_planar_yuv(int type) {
    return type > PIX_FMT_YUV_PLANAR_FIRST && type < PIX_FMT_YUV_PLANAR_LAST;
}

static inline bool pix_fmt_is_packet_yuv(int type) {
    return type > PIX_FMT_YUV_PACKED_FIRST && type < PIX_FMT_YUV_PACKED_LAST;
}

static inline bool pix_fmt_is_rgb(int type) {
    return type > PIX_FMT_RGB_FIRST && type < PIX_FMT_RGB_LAST;
}

static inline int pix_fmt_bpp(int type) {
    if (pix_fmt_is_yuv(type)) {
        return 12;
    }
    switch(type) {
    case PIX_FMT_RGB:
    case PIX_FMT_BGR:
        return 24;
    case PIX_FMT_RGBA:
    case PIX_FMT_BGRA:
    case PIX_FMT_ARGB:
    case PIX_FMT_ABGR:
        return 32;
    case PIX_FMT_GRAY:
        return 8;
    }
    return 0;
}

typedef enum {
    MEDIA_TYPE_FILE = 0,
    MEDIA_TYPE_NETWORK,
    MEDIA_TYPE_CAPTURE,
    MEDIA_TYPE_NB
} media_type_e;
#define DEFAULT_MEDIA_TYPE  MEDIA_TYPE_CAPTURE
#define MEDIA_TYPE_NONE     MEDIA_TYPE_NB

typedef enum {
    AVSTREAM_TYPE_VIDEO,
    AVSTREAM_TYPE_AUDIO,
    AVSTREAM_TYPE_SUBTITLE,
    AVSTREAM_TYPE_NB
} avstream_type_e;

typedef enum {
    ASPECT_FULL,            // 100%
    ASPECT_PERCENT,         // 50%
    ASPECT_ORIGINAL_RATIO,  // w:h
    ASPECT_ORIGINAL_SIZE,   // wxh
    ASPECT_CUSTOM_RATIO,    // 4:3 16:9 ...
    ASPECT_CUSTOM_SIZE,     // 1280x720 640*480 ...
} aspect_ratio_e;
#define DEFAULT_ASPECT_RATIO    ASPECT_FULL

typedef struct aspect_ratio_s {
    aspect_ratio_e type;
    int w, h;
} aspect_ratio_t;

#endif // AV_DEF_H
