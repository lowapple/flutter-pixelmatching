
#include <iostream>
#include <opencv2/opencv.hpp>
#include "common.h"

using namespace cv;
using namespace std;

int main() {
    string path = "../assets/20230314_154034.jpg";
    Mat img = imread(path, IMREAD_COLOR);
    if (img.empty()) {
        cout << "Could not read the image: " << path << endl;
        return 1;
    }
    imshow("Image", img);
    waitKey(0);
    return 0;
}