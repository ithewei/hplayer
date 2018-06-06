#ifndef OPENCV_UTIL_H
#define OPENCV_UTIL_H

#include "opencv2/opencv.hpp"
#include "hframe.h"
#include "hgl.h"

inline void mat2frame(cv::Mat& mat, HFrame& frame){
    frame.buf.base  = mat.data;
    frame.buf.len   = mat.cols * mat.rows * mat.channels();
    frame.w = mat.cols;
    frame.h = mat.rows;
    frame.bpp = mat.channels()*8;
    switch(mat.type()){
    default:
        frame.type = GL_BGR;
    }
}

#endif // OPENCV_UTIL_H
