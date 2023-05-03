
#include <iostream>
#include "Constants.h"
#include "ImageProcessor.h"

using namespace cv;
using namespace std;

int main() {
    ImageProcessor imageProcessor;
    imageProcessor.initialize();
    cout << "State Code: " << imageProcessor.getStateCode() << endl;
    string aPath = "../a/a.jpg";
    Mat aImg = imread(aPath, IMREAD_COLOR);
    if (aImg.empty()) {
        cout << "Could not read the image: " << aPath << endl;
        return 1;
    }
    string bPath = "../b/1.jpeg";
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
