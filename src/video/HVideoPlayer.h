#ifndef H_VIDEO_PLAYER_H
#define H_VIDEO_PLAYER_H

#include "hmedia.h"
#include "hframe.h"
#include "hstring.h"
#include "confile.h"

#define DEFAULT_FPS         25
#define DEFAULT_FRAME_CACHE 5

enum {
    SOFTWARE_DECODE         = 1,
    HARDWARE_DECODE_QSV     = 2,
    HARDWARE_DECODE_CUVID   = 3,
};
#define DEFAULT_DECODE_MODE HARDWARE_DECODE_CUVID

enum hplayer_event_e {
    HPLAYER_OPEN_FAILED,
    HPLAYER_OPENED,
    HPLAYER_EOF,
    HPLAYER_CLOSED,
    HPLAYER_ERROR,
};
typedef int (*hplayer_event_cb)(hplayer_event_e e, void* userdata);

class HVideoPlayer
{
public:
    HVideoPlayer() {
        set_frame_cache(g_confile->Get<int>("frame_cache", "video", DEFAULT_FRAME_CACHE));
        fps = g_confile->Get<int>("fps", "video", DEFAULT_FPS);
        decode_mode = g_confile->Get<int>("decode_mode", "video", DEFAULT_DECODE_MODE);

        width = 0;
        height = 0;
        duration = 0;
        start_time = 0;
        eof = 0;
        error = 0;
        event_cb = NULL;
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

    void clear_frame_cache() {
        frame_buf.clear();
    }

    int push_frame(HFrame* pFrame) {
        return frame_buf.push(pFrame);
    }

    int pop_frame(HFrame* pFrame) {
        return frame_buf.pop(pFrame);
    }

    void set_event_callback(hplayer_event_cb cb, void* userdata) {
        event_cb = cb;
        event_cb_userdata = userdata;
    }

    void event_callback(hplayer_event_e e) {
        if (event_cb) {
            event_cb(e, event_cb_userdata);
        }
    }

public:
    HMedia      media;
    int         fps;
    int         decode_mode;
    int         real_decode_mode;

    int32_t     width;
    int32_t     height;
    int64_t     duration;   // ms
    int64_t     start_time; // ms
    int         eof;
    int         error;
protected:
    HFrameBuf   frame_buf;
    hplayer_event_cb    event_cb;
    void*               event_cb_userdata;
};

#endif // H_VIDEO_PLAYER_H
