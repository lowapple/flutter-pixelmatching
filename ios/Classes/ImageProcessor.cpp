#include "ImageProcessor.hpp"
#include "ImageConverter.hpp"
#include <thread>

ImageProcessor::ImageProcessor() : buffer(new char[Constants::sourceBufferSize])
{
}

ImageProcessor::~ImageProcessor()
{
    comparators.clear();
    delete[] buffer;
}

int ImageProcessor::getStateCode()
{
    return stateCode;
}

void ImageProcessor::initialize()
{
    if (stateCode == StateCode::NotInitialize)
    {
        comparators.push_back(
            new Comparator_BuiltInDetector<cv::SIFT, cv::DescriptorMatcher::MatcherType::FLANNBASED>());
        stateCode = StateCode::NotReadyTarget;
    }
    return;
}

bool ImageProcessor::setSourceImage(Mat img)
{
    if ((stateCode & StateCode::NotReadyTarget) != StateCode::NotReadyTarget)
    {
        return false;
    }
    int res = process(img, true);
    stateCode = StateCode::ReadyToProcess;

    if (res == 0)
    {
        return false;
    }
    return true;
}

bool ImageProcessor::setQueryImage(Mat img)
{
    if ((stateCode & StateCode::ReadyToProcess) != StateCode::ReadyToProcess)
    {
        return false;
    }
    confidence_rate = -1.0;
    stateCode = StateCode::Processing;
    int res = process(img, true);
    if (res == 0)
    {
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

    if (needConvert)
    {
        img_converted = imageconverter::process(img);
    }
    else
    {
        img_converted = img.clone();
    }

    // 타켓 설정
    if (stateCode == StateCode::NotReadyTarget)
    {
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