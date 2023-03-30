#pragma once

#include "opencv2/core/utility.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/calib3d.hpp"

using namespace cv;
using namespace std;

class Constants
{
private:
    ~Constants() = delete;

public:
    static constexpr double version = 0.2;
    static constexpr double min_version = 0.1;
    static constexpr float threshold = 0.7f;
    static constexpr size_t buffer_size = 80 * 1024 * 1024;
    static constexpr double base_marker_width_ratio = 0.1;
    static constexpr int base_img_width = 256;
    static constexpr int base_img_height = 256;
#ifdef __ANDROID__
    static constexpr int external_img_type = CV_8UC4;
    static constexpr int external_img_convertCode = cv::COLOR_RGBA2GRAY;
#endif
#ifdef __APPLE__
    static constexpr int external_img_type = CV_8UC3;
    static constexpr int external_img_convertCode = cv::COLOR_RGBA2GRAY;
#endif
};