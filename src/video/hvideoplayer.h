#ifndef HVIDEOPLAYER_H
#define HVIDEOPLAYER_H

#include "hmedia.h"
#include "hframe.h"

#define DEFAULT_FPS         25
#define DEFAULT_FRAME_CACHE 5

class HVideoPlayer
{
public:
    HVideoPlayer(){
        set_fps(DEFAULT_FPS);
        set_frame_cache(DEFAULT_FRAME_CACHE);
    }
    virtual ~HVideoPlayer() {}

    virtual int start() = 0;
    virtual int stop() = 0;
    virtual int pause() = 0;
    virtual int resume() = 0;

    void set_media(HMedia& media){
        this->media = media;
    }

    void set_fps(int fps){
        this->fps = fps;
    }

    FrameStats get_frame_stats(){
        return frame_buf.frame_stats;
    }

    FrameInfo get_frame_info(){
        return frame_buf.frame_info;
    }

    void set_frame_cache(int cache){
        frame_buf.setCache(cache);
    }

    int push_frame(HFrame* pFrame){
        return frame_buf.push(pFrame);
    }

    int pop_frame(HFrame* pFrame){
        return frame_buf.pop(pFrame);
    }

protected:
    int         fps;
    HMedia      media;
    HFrameBuf   frame_buf;
};

#endif // HVIDEOPLAYER_H
