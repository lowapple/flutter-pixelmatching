#include "pixel_matching.h"
#include "debug_logger.hpp"
#include "image_processor.hpp"

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
  bool initialize()
  {
    if (processor == nullptr)
    {
      processor = new ImageProcessor();
      processor->initialize();
      return true;
    }
    return false;
  }

  FUNCTION_ATTRIBUTE
  int getStatusCode()
  {
    if (processor == nullptr)
    {
      return -1;
    }
    return processor->getStateCode();
  }

  FUNCTION_ATTRIBUTE
  bool setTargetImage(unsigned char *target, int width, int height)
  {
    if (processor == nullptr)
    {
      return false;
    }
    // Decode images from JPEG-encoded targets
    cv::Mat imageTarget = cv::imdecode(cv::Mat(1, width * height * 3, CV_8UC1, target), cv::IMREAD_COLOR);
    return processor->setSourceImage(imageTarget);
  }

  FUNCTION_ATTRIBUTE
  bool setQueryImage(unsigned char *query, int width, int height)
  {
    if (processor == nullptr)
    {
      return false;
    }
    // Decode images from JPEG-encoded queries
    cv::Mat imageQuery = cv::imdecode(cv::Mat(1, width * height * 3, CV_8UC1, query), cv::IMREAD_COLOR);
    return processor->setQueryImage(imageQuery);
  }

  FUNCTION_ATTRIBUTE
  double getQueryConfidenceRate()
  {
    if (processor == nullptr)
    {
      return -1;
    }
    return processor->getQueryConfidenceRate();
  }

  FUNCTION_ATTRIBUTE
  void dispose()
  {
    if (processor != nullptr)
    {
      delete processor;
      processor = nullptr;
    }
  }
#ifdef __cplusplus
}
#endif
