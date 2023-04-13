#include "ImageConverter.h"

namespace image_converter {
    cv::Mat process(cv::Mat img) {
        Mat gray;
        if (img.channels() == 3) {
            cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
        } else if (img.channels() == 4) {
            cv::cvtColor(img, gray, cv::COLOR_BGRA2GRAY);
        } else {
            gray = img;
        }

        float imgRatio = (float) gray.cols / (float) gray.rows;
        int cropW = gray.cols;
        int cropH = (int) round((float) gray.rows * imgRatio);
        int cropX = (int) round((float) (gray.cols - cropW) / 2.0f);
        int cropY = (int) round((float) (gray.rows - cropH) / 2.0f);
        cv::Rect roi(cropX, cropY, cropW, cropH);
        gray = gray(roi);
        if (gray.cols != Constants::sourceImageW || gray.rows != Constants::sourceImageH) {
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