#ifndef FFMPEG_UTIL_H
#define FFMPEG_UTIL_H

#include "hw/hframe.h"
#include "hw/hgl.h"

extern "C" {
#include "libavutil/avutil.h"
#include "libavutil/pixdesc.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavdevice/avdevice.h"
#include "libavfilter/avfilter.h"
}

#endif // FFMPEG_UTIL_H