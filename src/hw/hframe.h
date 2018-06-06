#ifndef HFRAME_H
#define HFRAME_H

#include "hbuf.h"
#include <deque>

typedef struct hframe_s{
    hbuf_t buf;
    int w;
    int h;
    int type;
    int bpp;
    uint64 ts;
    void* userdata;
    hframe_s(){
        w = h = type = bpp = ts = 0;
        userdata = NULL;
    }

    bool isNull(){
        return w == 0 || h == 0 || buf.isNull();
    }

    // deep copy
    void load(const hframe_s& frame){
        this->w = frame.w;
        this->h = frame.h;
        this->type = frame.type;
        this->bpp  = frame.bpp;
        this->ts   = frame.ts;
        this->userdata = frame.userdata;
        if (this->buf.isNull() || this->buf.len != frame.buf.len){
            this->buf.init(frame.buf.len);
        }
        memcpy(this->buf.base, frame.buf.base, frame.buf.len);
    }
}HFrame;

typedef struct frame_info_s{
    int w;
    int h;
    int type;
    int bpp;
}FrameInfo;

typedef struct frame_stats_s{
    int push_cnt;
    int pop_cnt;

    int push_ok_cnt;
    int pop_ok_cnt;

    frame_stats_s(){
        push_cnt = pop_cnt = push_ok_cnt = pop_ok_cnt = 0;
    }
}FrameStats;

#define DEFAULT_FRAME_CACHENUM  10

class HFrameBuf : public HRingBuf{
public:
    HFrameBuf() : HRingBuf() {cache_num = DEFAULT_FRAME_CACHENUM;}

    void setCache(int num) {cache_num = num;}

    int push(HFrame* pFrame){
        if (pFrame->isNull())
            return -10;

        frame_stats.push_cnt++;

        std::lock_guard<std::mutex> locker(mutex);

        if (frames.size() >= cache_num){
            //return -20; // note: cache full, discard frame

            // note: cache full, remove front, push newer frame
            HFrame& frame = frames.front();
            frames.pop_front();
            free(frame.buf.len);
        }

        int ret = 0;
        if (isNull()){
            init(pFrame->buf.len * cache_num);
            ret = 1; // note: first push

            frame_info.w = pFrame->w;
            frame_info.h = pFrame->h;
            frame_info.type = pFrame->type;
            frame_info.bpp  = pFrame->bpp;
        }

        HFrame frame;
        frame.buf.base = alloc(pFrame->buf.len);
        frame.buf.len  = pFrame->buf.len;
        frame.load(*pFrame);
        frames.push_back(frame);
        frame_stats.push_ok_cnt++;

        return ret;
    }

    int pop(HFrame* pFrame){
        frame_stats.pop_cnt++;

        std::lock_guard<std::mutex> locker(mutex);

        if (isNull())
            return -10;

        if (frames.size() == 0)
            return -20;

        HFrame& frame = frames.front();
        frames.pop_front();
        free(frame.buf.len);

        if (frame.isNull())
            return -30;

        pFrame->load(frame);
        frame_stats.pop_ok_cnt++;

        return 0;
    }

    int         cache_num;
    FrameStats  frame_stats;
    FrameInfo   frame_info;
    std::deque<HFrame> frames;
};

#endif // HFRAME_H
