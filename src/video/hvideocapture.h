#ifndef HVIDEOCAPTURE_H
#define HVIDEOCAPTURE_H

#include "hvideoplayer.h"
#include "hthread.h"
#include "opencv2/opencv.hpp"

void mat2frame(cv::Mat& mat, HFrame& frame);

class HVideoCapture : public HVideoPlayer, public HThread
{
public:
    HVideoCapture() : HVideoPlayer(), HThread() {}

    virtual int start();

    virtual int stop();

    virtual void pause() {HThread::pause();}
    virtual void resume() {HThread::resume();}

    virtual int doTask();

protected:
    cv::VideoCapture vc;
};

#endif // HVIDEOCAPTURE_H
