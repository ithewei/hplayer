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

    vc.set(cv::CAP_PROP_FPS, 25);

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
}
