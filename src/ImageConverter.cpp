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

        int tarWidth = Constants::sourceImageW;
        int tarHeight = Constants::sourceImageH;

        // Calculate the aspect ratio of the original image
        float imgRatio = (float)gray.cols / (float)gray.rows;

        // Calculate the new dimensions while maintaining the aspect ratio
        int newWidth, newHeight;
        if (imgRatio > 1.0) {
            newWidth = tarWidth;
            newHeight = (int)round(tarWidth / imgRatio);
        } else {
            newHeight = tarHeight;
            newWidth = (int)round(tarHeight * imgRatio);
        }

        // Create a new image with the target size and fill it with black
        cv::Mat resizedImage(tarHeight, tarWidth, gray.type(), cv::Scalar(0));

        // Calculate the ROI to paste the resized image
        int x = (tarWidth - newWidth) / 2;
        int y = (tarHeight - newHeight) / 2;
        cv::Rect roi(x, y, newWidth, newHeight);

        // Resize the original image to fit the new dimensions
        cv::Mat resizedGray;
        cv::resize(gray, resizedGray, cv::Size(newWidth, newHeight));

        // Copy the resized image to the ROI
        resizedGray.copyTo(resizedImage(roi));

        // Ensure that any 0 values in the image are set to 1
        for (int r = 0; r < resizedImage.rows; r++) {
            for (int c = 0; c < resizedImage.cols; c++) {
                if (resizedImage.at<uchar>(r, c) == 0) {
                    resizedImage.at<uchar>(r, c) = 1;
                }
            }
        }

        img.release();
        return resizedImage;
    }
}