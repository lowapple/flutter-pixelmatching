#include "image_processor.hpp"
#include "image_converter.hpp"
#include <thread>

ImageProcessor::ImageProcessor()
{
}

ImageProcessor::~ImageProcessor()
{
    comparators.clear();
}

int ImageProcessor::getStateCode()
{
    return stateCode;
}

void ImageProcessor::initialize()
{
    if (stateCode == StateCode::NotInitialized)
    {
#ifdef __ANDROID__
        logger_info("ImageProcessor", "initialize", "SIFT");
        comparators.push_back(
            new ComparatorDetector<cv::SIFT, cv::DescriptorMatcher::MatcherType::FLANNBASED>());
#endif
#ifdef __APPLE__
        logger_info("ImageProcessor", "initialize", "KAZE");
        comparators.push_back(
            new ComparatorDetector<cv::KAZE, cv::DescriptorMatcher::MatcherType::FLANNBASED>());
#endif
        stateCode = StateCode::WaitingForTarget;
    }
    return;
}

bool ImageProcessor::setSourceImage(Mat img)
{
    StateCode oldState = stateCode;
    if ((stateCode & StateCode::WaitingForTarget) != StateCode::WaitingForTarget)
    {
        return false;
    }
    int res = process(img, true);
    if (res == 0)
    {
        stateCode = oldState;
        return false;
    }
    stateCode = StateCode::ReadyToProcess;
    return true;
}

bool ImageProcessor::setQueryImage(Mat img)
{
    StateCode oldState = stateCode;
    if ((stateCode & StateCode::ReadyToProcess) != StateCode::ReadyToProcess)
    {
        return false;
    }
    confidence_rate = -1.0;
    int res = process(img, true);
    if (res == 0)
    {
        stateCode = oldState;
        return false;
    }
    stateCode = StateCode::ReadyToProcess;
    return true;
}

double ImageProcessor::getQueryConfidenceRate()
{
    return comparators[0]->getQueryConfidenceRate();
}

int ImageProcessor::process(cv::Mat img, bool needConvert)
{
    cv::Mat img_converted;

    logger_info("ImageProcessor", "process", "Start process image.");

    if (needConvert)
    {
        logger_info("ImageProcessor", "process", "Convert image.");
        img_converted = image_converter::process(img);
    }
    else
    {
        logger_info("ImageProcessor", "process", "No need to convert image.");
        img_converted = img.clone();
    }

    // 타켓 설정
    if (stateCode == StateCode::WaitingForTarget)
    {
        stateCode = StateCode::Processing;
        for (auto comparator : comparators)
        {
            if (comparator->Process(img_converted, true) == 0)
            {
                return 0;
            }
        }
        return 1;
    }
    // 쿼리 설정및 비교
    else
    {
        stateCode = StateCode::Processing;
        for (auto comparator : comparators)
        {
            if (comparator->Process(img_converted, false) == 0)
            {
                return 0;
            }
        }
        return 1;
    }
}
