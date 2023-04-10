#include "ImageProcessor.h"
#include "ImageConverter.h"

ImageProcessor::ImageProcessor() {
    stateCode = StateCode::NotInitialized;

    compare.setMatchers(cv::DescriptorMatcher::create(DescriptorMatcher::MatcherType::FLANNBASED));
#ifdef __ANDROID__
    compare.setDetector(cv::SIFT::create());
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
    mutex.lock();
    StateCode oldState = getStateCode();
    logger_i("[ImageProcessor] setMarker: %d", getStateCode());
    if (oldState != StateCode::NoMarker) {
        return false;
    }
    setStateCode(StateCode::Processing);
    logger_i("[ImageProcessor] setMarker: %d", getStateCode());
    Mat converted = image_converter::process(marker);
    logger_i("[ImageProcessor] setMarker process image");
    marker.release();
    logger_i("[ImageProcessor] setMarker marker image release");
    if (compare.setMarker(converted)) {
        logger_i("[ImageProcessor] setMarker set marker");
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
    logger_i("[ImageProcessor] setQuery: %d", getStateCode());
    if (oldState != StateCode::NoQuery) {
        return false;
    }
    setStateCode(StateCode::Processing);
    logger_i("[ImageProcessor] setQuery: %d", getStateCode());
    Mat converted = image_converter::process(query);
    logger_i("[ImageProcessor] setQuery process image");
    query.release();
    logger_i("[ImageProcessor] setQuery query image release");
    if (compare.setQuery(converted)) {
        logger_i("[ImageProcessor] setQuery set marker");
        setStateCode(StateCode::NoQuery);
        mutex.unlock();
        return true;
    }
    setStateCode(oldState);
    mutex.unlock();
    return false;
}

double ImageProcessor::confidence() {
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