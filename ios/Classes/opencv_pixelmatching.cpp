#include "opencv_pixelmatching.h"

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
#ifdef __cplusplus
}
#endif
