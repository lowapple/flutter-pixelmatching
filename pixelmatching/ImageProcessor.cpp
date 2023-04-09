#include "ImageProcessor.h"
#include "ImageConverter.h"
#include <thread>

ImageProcessor::ImageProcessor() {
    stateCode = StateCode::NotInitialized;

    compare.setMatchers(cv::DescriptorMatcher::create(DescriptorMatcher::MatcherType::FLANNBASED));
#ifdef __ANDROID__
    this->compare->setDetector(cv::SIFT::create());
#elif __APPLE__
    compare.setDetector(cv::KAZE::create());
#endif
}

StateCode ImageProcessor::getStateCode() const {
    return stateCode;
}

void ImageProcessor::setStateCode(StateCode stateCode) {
    this->stateCode = stateCode;
}

void ImageProcessor::initialize() {
    if (getStateCode() == StateCode::NotInitialized) {
        setStateCode(StateCode::NoMarker);
    }
}

bool ImageProcessor::setMarker(Mat marker) {
    StateCode oldState = getStateCode();
    if ((oldState & StateCode::NoMarker) != StateCode::NoMarker) {
        return false;
    }
    setStateCode(StateCode::Processing);
    Mat converted = image_converter::process(marker);
    marker.release();
    if (compare.setMarker(converted)) {
        setStateCode(StateCode::NoQuery);
        return true;
    }
    setStateCode(oldState);
    return false;
}

bool ImageProcessor::setQuery(Mat query) {
    StateCode oldState = getStateCode();
    if ((oldState & StateCode::NoQuery) != StateCode::NoQuery) {
        return false;
    }
    setStateCode(StateCode::Processing);
    Mat converted = image_converter::process(query);
    query.release();
    if (compare.setQuery(converted)) {
        setStateCode(StateCode::NoQuery);
        return true;
    }
    setStateCode(oldState);
    return false;
}

double ImageProcessor::confidence() {
    return compare.getConfidenceRate();
}

Mat ImageProcessor::getImageMarker() {
    return compare.getImageMarker();
}

Mat ImageProcessor::getImageQuery() {
    return compare.getImageQuery();
}