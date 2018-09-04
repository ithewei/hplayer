#include "hvideocapture.h"
#include "opencv_util.h"

#include "qtheaders.h"
int HVideoCapture::start(){
    switch (media.type) {
    case MEDIA_TYPE_CAPTURE:
        vc.open(media.index, cv::CAP_DSHOW);
        break;
    case MEDIA_TYPE_FILE:
    case MEDIA_TYPE_NETWORK:
        vc.open(media.src.c_str(), cv::CAP_FFMPEG);
        break;
    default:
        return -10;
    }

    if (!vc.isOpened())
        return -20;

    vc.set(cv::CAP_PROP_FPS, fps);
    first_flag = true;

    return HThread::start();
}

int HVideoCapture::stop(){
    HThread::stop();
    vc.release();
    return 0;
}

void HVideoCapture::doTask(){
    cv::Mat mat;
    if (!vc.read(mat))
        return;

    //...ALG

    HFrame frame;
    Mat2HFrame(mat, frame);
    push_frame(&frame);

    if (first_flag){
        first_flag = false;
        base_tp = std::chrono::system_clock::now();
    }

    base_tp += std::chrono::milliseconds(1000/fps); // 40ms
    std::this_thread::sleep_until(base_tp);
}
