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
  bool setTargetImage(unsigned char *bytes, int width, int height, int rotation)
  {
    if (processor == nullptr)
    {
      return false;
    }
    // Decode images from JPEG-encoded targets
    cv::Mat image = cv::imdecode(cv::Mat(1, width * height * 3, CV_8UC1, bytes), cv::IMREAD_COLOR);
    if (rotation == 90)
      cv::rotate(image, image, cv::ROTATE_90_CLOCKWISE);
    else if (rotation == 180)
      cv::rotate(image, image, cv::ROTATE_180);
    else if (rotation == 270)
      cv::rotate(image, image, cv::ROTATE_90_COUNTERCLOCKWISE);
    return processor->setSourceImage(image);
  }

  FUNCTION_ATTRIBUTE
  bool setQueryImage(unsigned char *bytes, int width, int height, int rotation)
  {
    if (processor == nullptr)
    {
      return false;
    }
    // Decode images from JPEG-encoded queries
    cv::Mat image = cv::imdecode(cv::Mat(1, width * height * 3, CV_8UC1, bytes), cv::IMREAD_COLOR);
    if (rotation == 90)
      cv::rotate(image, image, cv::ROTATE_90_CLOCKWISE);
    else if (rotation == 180)
      cv::rotate(image, image, cv::ROTATE_180);
    else if (rotation == 270)
      cv::rotate(image, image, cv::ROTATE_90_COUNTERCLOCKWISE);
    return processor->setQueryImage(image);
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
