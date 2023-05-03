#pragma once

#include "opencv2/core/utility.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/calib3d.hpp"

using namespace cv;

class Constants {
public:
    static constexpr int knn = 2;
    static constexpr float threshold = 0.7f;
    static constexpr int sourceImageW = 256;
    static constexpr int sourceImageH = 256;
#ifdef __ANDROID__
    static constexpr int sourceImageType = CV_8UC4;
    static constexpr int sourceImageCode = cv::COLOR_RGBA2GRAY;
#endif
#ifdef __APPLE__
    static constexpr int sourceImageType = CV_8UC4;
    static constexpr int sourceImageCode = cv::COLOR_BGRA2GRAY;
#endif
};

enum StateCode : int {
    NotInitialized = -1,
    NoMarker = 0,
    NoQuery = 1,
    Processing = 2,
};