#ifndef OPENCV_UTIL_H
#define OPENCV_UTIL_H

#include "opencv2/opencv.hpp"
#include "hframe.h"
#include "hgl.h"

inline void Mat2HFrame(cv::Mat& mat, HFrame& frame) {
    frame.buf.base  = (char*)mat.data;
    frame.buf.len   = mat.cols * mat.rows * mat.channels();
    frame.w = mat.cols;
    frame.h = mat.rows;
    frame.bpp = mat.channels()*8;
    switch(mat.type()) {
    default:
        frame.type = GL_BGR;
    }
}

#include <QImage>
inline QImage Mat2QImage(cv::Mat& matBGR) {
    cv::Mat matRGB;
    cv::cvtColor(matBGR, matRGB,CV_BGR2RGB); // cvtColor Makes a copt, that what i need
    QImage dst((const uchar *) matRGB.data, matRGB.cols, matRGB.rows, matRGB.step, QImage::Format_RGB888);
    dst.bits(); // enforce deep copy, see documentation
    return dst;
}

inline cv::Mat QImage2Mat(QImage const& img) {
    cv::Mat matBGR;
    if (img.format() == QImage::Format_RGB32) {
        cv::Mat matRGBA(img.height(),img.width(),CV_8UC4,(uchar*)img.bits(),img.bytesPerLine());
        cv::cvtColor(matRGBA, matBGR,CV_BGRA2BGR);
    }
    else if (img.format() == QImage::Format_RGB888) {
        cv::Mat matRGB(img.height(),img.width(),CV_8UC3,(uchar*)img.bits(),img.bytesPerLine());
        cv::cvtColor(matRGB, matBGR,CV_RGB2BGR);
    }
    return matBGR;
}

#endif // OPENCV_UTIL_H
