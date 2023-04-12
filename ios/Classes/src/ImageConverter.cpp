#include "ImageConverter.h"

namespace image_converter {
    // TODO 타겟 이미지와 비율을 맞춰야할 것 같다.
    cv::Mat process(cv::Mat img) {
        cv::Mat gray;
        if (img.channels() == 3) {
            cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
        } else if (img.channels() == 4) {
            cv::cvtColor(img, gray, cv::COLOR_BGRA2GRAY);
        } else {
            gray = img;
        }
        if (img.cols != Constants::sourceImageW || img.rows != Constants::sourceImageH) {
            cv::resize(gray, gray, {Constants::sourceImageW, Constants::sourceImageH});
        }
        for (int r = 0; r < gray.rows; r++) {
            for (int c = 0; c < gray.cols; c++) {
                if (gray.at<uchar>(r, c) == 0) {
                    gray.at<uchar>(r, c) = 1;
                }
            }
        }
        img.release();
        return gray;
    }
}