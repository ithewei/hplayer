#ifndef H_VIDEO_PLAYER_H
#define H_VIDEO_PLAYER_H

#include "hmedia.h"
#include "hframe.h"
#include "hstring.h"
#include "hlog.h"

#define DEFAULT_FPS         25
#define DEFAULT_FRAME_CACHE 5

#define SIGNAL_END_OF_FILE  0x01

#define HARDWARE_DECODE     1
#define SOFTWARE_DECODE     2

#define DEFAULT_DECODE_MODE HARDWARE_DECODE

inline string strtime(int64_t ms) {
    int sec = ms / 1000;

    int min = sec / 60;
    sec = sec % 60;

    int hour = min / 60;
    min = min % 60;

    return asprintf("%02d:%02d:%02d", hour, min, sec);
}

class HVideoPlayer
{
public:
    HVideoPlayer() {
        set_frame_cache(DEFAULT_FRAME_CACHE);
        fps = DEFAULT_FPS;
        decode_mode = DEFAULT_DECODE_MODE;
        duration = 0;
        start_time = 0;
        signal = 0;
    }

    virtual ~HVideoPlayer() {}

    virtual int start() = 0;
    virtual int stop() = 0;
    virtual int pause() = 0;
    virtual int resume() = 0;

    virtual int seek(int64_t ms) {
        return 0;
    }

    void set_media(HMedia& media) {
        this->media = media;
    }

    void set_decode_mode(int mode) {
        decode_mode = mode;
    }

    FrameStats get_frame_stats() {
        return frame_buf.frame_stats;
    }

    FrameInfo get_frame_info() {
        return frame_buf.frame_info;
    }

    void set_frame_cache(int cache) {
        frame_buf.setCache(cache);
    }

    int push_frame(HFrame* pFrame) {
        return frame_buf.push(pFrame);
    }

    int pop_frame(HFrame* pFrame) {
        return frame_buf.pop(pFrame);
    }

public:
    HMedia      media;
    int         fps;
    int         decode_mode;
    int64_t     duration;   // ms
    int64_t     start_time; // ms
    int         signal;
protected:
    HFrameBuf   frame_buf;
};

#endif // H_VIDEO_PLAYER_H
