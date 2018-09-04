#ifndef HVIDEOCAPTURE_H
#define HVIDEOCAPTURE_H

#include "hvideoplayer.h"
#include "hthread.h"
#include "opencv2/opencv.hpp"

#include <chrono>
using namespace std::chrono;

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

    bool first_flag;
    system_clock::time_point base_tp;
};

#endif // HVIDEOCAPTURE_H
