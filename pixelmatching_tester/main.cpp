
#include <iostream>
#include "Constants.h"
#include "ImageProcessor.h"

using namespace cv;
using namespace std;

int main() {
    ImageProcessor imageProcessor;
    imageProcessor.initialize();
    cout << "State Code: " << imageProcessor.getStateCode() << endl;
    // Load Files
    string aPath = "../c/IMG_7169.jpeg";
    Mat aImg = imread(aPath, IMREAD_COLOR);
    // 감마 조정 하기
    double alpha = 2.91; /*< Simple contrast control */
    int beta = 70;       /*< Simple brightness control */
//    aImg.convertTo(aImg, -1, alpha, beta);

    if (aImg.empty()) {
        cout << "Could not read the image: " << aPath << endl;
        return 1;
    }
    string bPath = "../c/IMG_7175.jpeg";
    Mat bImg = imread(bPath, IMREAD_COLOR);
    if (bImg.empty()) {
        cout << "Could not read the image: " << bPath << endl;
        return 1;
    }
    // Set Marker
    imageProcessor.setMarker(aImg);
    cout << "State Code: " << imageProcessor.getStateCode() << endl;
    // Set Query
    imageProcessor.setQuery(bImg);
    cout << "State Code: " << imageProcessor.getStateCode() << endl;
    // Print Confidence
    cout << "Confidence: " << imageProcessor.getConfidenceRate() << endl;

    Mat mkr = imageProcessor.getImageMarker();
    Mat qry = imageProcessor.getImageQuery();
    cv::Mat combined;
    cv::hconcat(mkr, qry, combined);
    cv::Mat diff;
    cv::absdiff(mkr, qry, diff);
    cv::threshold(diff, diff, 50, 255, cv::THRESH_BINARY);
    cv::hconcat(combined, diff, combined);
    cv::imshow("Main", combined);
    waitKey(0);
    return 0;
}
