#include "PixelMatching.h"
#include "DebugLogger.h"
#include "ImageProcessor.h"

#if defined(__GNUC__)
// Attributes to prevent 'unused' function from being removed and to make it visible
#define FUNCTION_ATTRIBUTE __attribute__((visibility("default"))) __attribute__((used))
#elif defined(_MSC_VER)
// Marking a function for export
#define FUNCTION_ATTRIBUTE __declspec(dllexport)
#endif

#ifdef __cplusplus
extern "C"
{
#endif

ImageProcessor *processor = nullptr;

FUNCTION_ATTRIBUTE
bool initialize() {
    if (processor == nullptr) {
        processor = new ImageProcessor();
        processor->initialize();
        return true;
    }
    return false;
}

FUNCTION_ATTRIBUTE
int getStateCode() {
    if (processor == nullptr) {
        return -1;
    }
    return processor->getStateCode();
}

FUNCTION_ATTRIBUTE
bool setMarker(unsigned char *bytes, int width, int height, int rotation) {
    if (processor == nullptr) {
        return false;
    }
    Mat image = imdecode(Mat(1, width * height * 3, CV_8UC1, bytes), IMREAD_COLOR);
    if (rotation == 90) {
        transpose(image, image);
        flip(image, image, 1);
    } else if (rotation == 180) {
        transpose(image, image);
        flip(image, image, -1);
    } else if (rotation == 270) {
        transpose(image, image);
        flip(image, image, 0);
    }
    bool res = processor->setMarker(image);
    image.release();

    return res;
}

FUNCTION_ATTRIBUTE
bool setQuery(unsigned char *bytes, int width, int height, int rotation) {
    if (processor == nullptr) {
        return false;
    }
#ifdef __ANDROID__
    Mat image = imdecode(Mat(1, width * height * 3, CV_8UC1, bytes), IMREAD_COLOR);
#elif __APPLE__
    Mat image(height, width, CV_8UC4, bytes);
#endif
    if (rotation == 90) {
        transpose(image, image);
        flip(image, image, 1);
    } else if (rotation == 180) {
        transpose(image, image);
        flip(image, image, -1);
    } else if (rotation == 270) {
        transpose(image, image);
        flip(image, image, 0);
    }

    bool res = processor->setQuery(image);
    image.release();

    return res;
}

FUNCTION_ATTRIBUTE
double getQueryConfidenceRate() {
    if (processor == nullptr) {
        return -1;
    }
    return processor->confidence();
}

FUNCTION_ATTRIBUTE
void dispose() {
    if (processor != nullptr) {
        delete processor;
        processor = nullptr;
    }
}

FUNCTION_ATTRIBUTE
unsigned char *getMarkerQueryDifferenceImage() {
    if (processor == nullptr) {
        return nullptr;
    }
    Mat mkr = processor->getImageMarker();
    Mat qry = processor->getImageQuery();
    if (mkr.empty() || qry.empty()) {
        return nullptr;
    }
    cv::Mat combined;
    cv::hconcat(mkr, qry, combined);
    cv::Mat diff;
    cv::absdiff(mkr, qry, diff);
    cv::threshold(diff, diff, 50, 255, cv::THRESH_BINARY);
    cv::hconcat(combined, diff, combined);

    std::vector<unsigned char> encoded_data;
    std::vector<int> compression_params;
    compression_params.push_back(cv::IMWRITE_JPEG_QUALITY);
    compression_params.push_back(90);

    cv::imencode(".jpg", combined, encoded_data, compression_params);

    auto *data = new unsigned char[encoded_data.size()];
    std::memcpy(data, encoded_data.data(), encoded_data.size());
    return data;
}

#ifdef __cplusplus
}
#endif
