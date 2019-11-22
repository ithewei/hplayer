#ifndef H_VIDEO_CAPTURE_H
#define H_VIDEO_CAPTURE_H

#include "HVideoPlayer.h"
#include "hthread.h"
#include "opencv2/opencv.hpp"

class HVideoCapture : public HVideoPlayer, public HThread
{
public:
    HVideoCapture() : HVideoPlayer(), HThread() {}

    virtual int start();
    virtual int stop();
    virtual int pause() {return HThread::pause();}
    virtual int resume() {return HThread::resume();}

    virtual void doTask();

protected:
    cv::VideoCapture vc;
};

#endif // HVIDEOCAPTURE_H
