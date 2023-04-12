
#include <iostream>
#include "Constants.h"
#include "ImageProcessor.h"

using namespace cv;
using namespace std;

int main() {
    ImageProcessor imageProcessor;
    imageProcessor.initialize();
    printf("State Code: %d", imageProcessor.getStateCode());
    // Load Files
    string aPath = "../sample/20230314_154034.jpg";
    Mat aImg = imread(aPath, IMREAD_COLOR);
    if (aImg.empty()) {
        cout << "Could not read the image: " << aPath << endl;
        return 1;
    }
    string bPath = "../sample/20230322_140703.jpg";
    Mat bImg = imread(bPath, IMREAD_COLOR);
    if (bImg.empty()) {
        cout << "Could not read the image: " << bPath << endl;
        return 1;
    }
    // Set Marker
    imageProcessor.setMarker(aImg);
    printf("State Code: %d", imageProcessor.getStateCode());
    // Set Query
    imageProcessor.setQuery(bImg);
    printf("State Code: %d", imageProcessor.getStateCode());
    // Print Confidence
    printf("Confidence: %f", imageProcessor.confidence());

    Mat mkr = imageProcessor.getImageMarker();
    Mat qry = imageProcessor.getImageQuery();
    cv::Mat combined;
    cv::hconcat(mkr, qry, combined);
    cv::Mat diff;
    cv::absdiff(mkr, qry, diff);
    cv::threshold(diff, diff, 50, 255, cv::THRESH_BINARY);
    cv::hconcat(combined, diff, combined);


//    cv::Mat abc;
//    cv::hconcat(ab, diff, abc);
//
//    cv::Mat threshImg;
//    cv::threshold(diff, diff, 50, 255, cv::THRESH_BINARY);
//
//    cv::Mat abcd;
//    cv::hconcat(abc, threshImg, abcd);
//
//    std::vector<std::vector<cv::Point>> contours;
//    std::vector<cv::Vec4i> hierarchy;
//    cv::findContours(threshImg, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
//
//    std::vector<cv::Point> centers;
//    for (const auto & contour : contours) {
//        cv::Moments mu = cv::moments(contour, false);
//        centers.emplace_back(mu.m10 / mu.m00, mu.m01 / mu.m00);
//    }
//
//    cv::Mat resultImg = mkr.clone();
//    for (size_t i = 0; i < centers.size(); i += 2) {
//        cv::line(resultImg, centers[i], centers[i + 1], cv::Scalar(0, 0, 255), 2);
//    }
//
//    cv::Mat abcde;
//    cv::hconcat(abcd, resultImg, abcde);

    cv::imshow("Result", combined);


//    // 마스크 처리
//    Mat mkrMask = imageProcessor.getMaskedMarker();
//    Mat qryMask = imageProcessor.getMaskedQuery();
//    cv::Mat mask;

//    // 합치기
//    cv::Mat combined;
//    cv::vconcat(process, mask, combined);
//    // Show
//    cv::imshow("Combined", threshImg);
    waitKey(0);
    return 0;
}
