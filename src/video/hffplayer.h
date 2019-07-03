#ifndef H_FFPLAYER_H
#define H_FFPLAYER_H

#include "hvideoplayer.h"
#include "ffmpeg_util.h"

#include "hthread.h"

class HFFPlayer : public HVideoPlayer, public HThread {
public:
    HFFPlayer();
    ~HFFPlayer();

    virtual int start();
    virtual int stop();
    virtual int pause() {return HThread::pause();}
    virtual int resume() {return HThread::resume();}
    virtual int seek(int64_t ms);

private:
    virtual void doTask();

    void cleanup();

private:
    static std::atomic_flag s_init_flag;

    AVFormatContext*    fmt_ctx;
    AVCodecContext*     codec_ctx;

    AVFrame* frame;
    AVPacket* packet;

    int video_stream_index;
    int audio_stream_index;
    int subtitle_stream_index;

    int video_time_base_num;
    int video_time_base_den;

    // for scale
    AVPixelFormat dst_pix_fmt;
    SwsContext* sws_ctx;
    uint8_t* data[4];
    int linesize[4];
    HFrame hframe;
};

#endif // H_FFPLAYER_H
