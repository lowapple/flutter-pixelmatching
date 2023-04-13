#include "ImageProcessor.h"
#include "ImageConverter.h"

ImageProcessor::ImageProcessor() : stateCode(NotInitialized) {

    compare.setMatchers(cv::DescriptorMatcher::create(DescriptorMatcher::MatcherType::FLANNBASED));
#ifdef __ANDROID__
    compare.setDetector(cv::SIFT::create());
#elif __APPLE__
    compare.setDetector(cv::KAZE::create());
#endif
}

void ImageProcessor::setStateCode(StateCode code) {
    this->stateCode = code;
}

void ImageProcessor::initialize() {
    if (getStateCode() == NotInitialized) {
        setStateCode(NoMarker);
    }
}

bool ImageProcessor::setMarker(Mat marker) {
    mutex.lock();
    StateCode oldState = getStateCode();
    if (oldState != StateCode::NoMarker) {
        return false;
    }
    setStateCode(StateCode::Processing);
    Mat converted = image_converter::process(marker);
    marker.release();
    if (compare.setMarker(converted)) {
        setStateCode(StateCode::NoQuery);
        mutex.unlock();
        return true;
    }
    setStateCode(oldState);
    mutex.unlock();
    return false;
}

bool ImageProcessor::setQuery(Mat query) {
    mutex.lock();
    StateCode oldState = getStateCode();
    if (oldState != StateCode::NoQuery) {
        return false;
    }
    setStateCode(StateCode::Processing);
    Mat converted = image_converter::process(query);
    query.release();
    if (compare.setQuery(converted)) {
        setStateCode(StateCode::NoQuery);
        mutex.unlock();
        return true;
    }
    setStateCode(oldState);
    mutex.unlock();
    return false;
}

double ImageProcessor::getConfidenceRate() {
    return compare.getConfidenceRate();
}

Mat ImageProcessor::getImageMarker() {
    mutex.lock();
    Mat marker = compare.getImageMarker().clone();
    mutex.unlock();
    return marker;
}

Mat ImageProcessor::getImageQuery() {
    mutex.lock();
    Mat query = compare.getImageQuery().clone();
    mutex.unlock();
    return query;
}

StateCode ImageProcessor::getStateCode() {
    return stateCode;
}
