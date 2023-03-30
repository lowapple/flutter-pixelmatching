#include "opencv_pixelmatching.h"

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <iostream>

using namespace cv;
using namespace std;

// Attributes to prevent 'unused' function from being removed and to make it visible

#ifdef __cplusplus
extern "C" {
#endif
    __attribute__((visibility("default"))) __attribute__((used))
    const char* version() {
        return CV_VERSION;
    }
#ifdef __cplusplus
}
#endif