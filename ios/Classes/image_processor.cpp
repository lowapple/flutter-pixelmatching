#include "image_processor.hpp"
#include "image_converter.hpp"

ImageProcessor::ImageProcessor() : buffer(new char[Constants::buffer_size]),
                                   cache(buffer)
{
}

ImageProcessor::~ImageProcessor()
{
    delete[] buffer;
}

int ImageProcessor::GetStateCode()
{
    return stateCode;
}

double ImageProcessor::Initialize(double wrapper_version)
{
    if (stateCode != ReadyToRetriveTargetData)
    {
        return 0;
    }

    if (wrapper_version < Constants::min_version)
    {
        return -1.0;
    }

    stateCode = Busy;

    comparators.push_back(
        new Comparator_BuiltInDetector<cv::SIFT, cv::DescriptorMatcher::MatcherType::FLANNBASED>());
    ;
    stateCode = ReadyToRetriveTargetData;

    return Constants::version;
}

char *ImageProcessor::GetPtrOfBuffer()
{
    return buffer;
}

int ImageProcessor::SetMarker(cv::Mat img_marker, bool needConvert)
{
    stateCode = (StateCode)(stateCode | ReadyToRetriveTargetData);

    return 1;
}

int ImageProcessor::SetMarker(char *ptr_marker, int img_width, int img_height)
{
    return SetMarker(ImageConverter::Convert(ptr_marker, img_width, img_height), false);
}

int ImageProcessor::SetMarker(int img_width, int img_height)
{
    return SetMarker(buffer, img_width, img_height);
}

int ImageProcessor::SetMarkerData()
{
    return 0;
}

int ImageProcessor::Start_RetriveTargetData()
{
    StateCode old_stateCode = stateCode;

    stateCode = Busy;

    if ((old_stateCode & ReadyToRetriveTargetData) != ReadyToRetriveTargetData)
    {
        stateCode = old_stateCode;
        return 0;
    }

    stateCode = RetrivingTargetData;
    return 1;
}

int ImageProcessor::End_RetriveTargetData()
{
    StateCode old_stateCode = stateCode;

    stateCode = Busy;

    if (old_stateCode != RetrivingTargetData)
    {
        stateCode = old_stateCode;
        return 0;
    }

    stateCode = ReadyToProcessQueries;
    return 1;
}

int ImageProcessor::ImportTargetData()
{
    // �̱���
    return 0;
}

int ImageProcessor::ExportTargetData()
{
    // �̱���
    return 0;
}

int ImageProcessor::Start_ProcessQuery()
{
    StateCode old_stateCode = stateCode;

    stateCode = Busy;

    if ((old_stateCode & ReadyToProcessQueries) != ReadyToProcessQueries)
    {
        stateCode = old_stateCode;
        return 0;
    }

    confidence_rate = -1.0;

    stateCode = ProcessingQueries;
    return 1;
}

int ImageProcessor::End_ProcessQuery()
{
    StateCode old_stateCode = stateCode;

    stateCode = Busy;

    if (old_stateCode != ProcessingQueries)
    {
        stateCode = old_stateCode;
        return 0;
    }

    stateCode = ReadyToProcessQueries;
    return 1;
}

double ImageProcessor::GetConfidenceRate()
{
    return comparators[0]->GetConfidenceRate();
}

int ImageProcessor::ExportMostAccurateQueryImage()
{
    // �̱���
    return 0;
}

int ImageProcessor::Process(cv::Mat img, bool needConvert)
{
    if (stateCode != RetrivingTargetData && stateCode != ProcessingQueries)
    {
        return 0;
    }

    cv::Mat img_converted;

    if (needConvert)
    {
        img_converted = ImageConverter::Convert(img);
    }
    else
    {
        img_converted = img.clone();
    }

    if (stateCode == RetrivingTargetData)
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

int ImageProcessor::Process(char *ptr_img, int width, int height)
{
    // return Process(ImageConverter::Convert(ptr_img, width, height), false);
    try
    {
        return Process(ImageConverter::Convert(ptr_img, width, height), false);
    }
    catch (...)
    {
        return 0;
    }
}

int ImageProcessor::Process(int width, int height)
{
    return Process(buffer, width, height);
}