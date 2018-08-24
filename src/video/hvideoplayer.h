#ifndef HVIDEOPLAYER_H
#define HVIDEOPLAYER_H

#include "hmedia.h"
#include "hframe.h"

class HVideoPlayer
{
public:
    HVideoPlayer(){}
    virtual ~HVideoPlayer() {}

    virtual int start() = 0;
    virtual int stop() = 0;
    virtual int pause() = 0;
    virtual int resume() = 0;

    void set_media(HMedia& media){
        this->media = media;
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
    HMedia      media;
    HFrameBuf   frame_buf;
};

#endif // HVIDEOPLAYER_H
