#include <iostream>
#include "opencv_pixelmatching.h"
#include "common.hpp"
#include "debug_logger.hpp"

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
  // init
  FUNCTION_ATTRIBUTE
  void init()
  {
#ifdef __ANDROID__
    logger_info("pixelmatching %s", "init os android");
#elif __APPLE__
  logger_info("pixelmatching %s", "init os ios");
#endif
  }

  // 버전 정보 반환
  FUNCTION_ATTRIBUTE
  const char *version()
  {
    return CV_VERSION;
  }

  // 흑백 이미지 반환
  FUNCTION_ATTRIBUTE
  void grayscale(uchar *buf, int w, int h, bool isYuv, uchar *out)
  {
    logger_info("grayscale %d %d %d", w, h, isYuv);
    Mat myyuv(h + h / 2, w, CV_8UC1, buf);
    Mat myrgb(h, w, CV_8UC3);
    cvtColor(myyuv, myrgb, COLOR_YUV2BGR_NV21);
    // cvtColor(myrgb, myrgb, COLOR_RGB2GRAY);

    vector<uchar> encoded;
    imencode(".png", myrgb, encoded);

    memcpy(out, encoded.data(), encoded.size());
  }
#ifdef __cplusplus
}
#endif
