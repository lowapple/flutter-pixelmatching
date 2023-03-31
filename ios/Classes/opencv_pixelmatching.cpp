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
    Mat myyuv(h + h / 2, w, CV_8UC1, buf);
    Mat myrgb;
    cvtColor(myyuv, myrgb, COLOR_YUV2RGB_NV21, 3);
    // cvtColor(myrgb, myrgb, COLOR_RGB2GRAY);

    vector<uchar> encoded;
    imencode(".jpg", myrgb, encoded);
    memcpy(out, encoded.data(), encoded.size());
  }

  // code from
  // https://github.com/Hugand/camera_tutorial/blob/master/ios/Classes/converter.c
  FUNCTION_ATTRIBUTE
  int clamp(int lower, int higher, int val)
  {
    if (val < lower)
      return 0;
    else if (val > higher)
      return 255;
    else
      return val;
  }

  FUNCTION_ATTRIBUTE
  int getRotatedImageByteIndex(int x, int y, int rw)
  {
    return rw * (y + 1) - (x + 1);
  }

  // YUV 이미지를 RGB 이미지로 변환
  FUNCTION_ATTRIBUTE
  uint32_t *yuv2rgb(uchar *p1, uchar *p2, uchar *p3, int pr, int pp, int w, int h)
  {
    int hexFF = 255;
    int x, y, uvIndex, index;
    int yp, up, vp;
    int r, g, b;
    int rt, gt, bt;

    uint32_t *image = (uint32_t *)malloc(sizeof(uint32_t) * (w * h));

    for (x = 0; x < w; x++)
    {
      for (y = 0; y < h; y++)
      {

        uvIndex = pp * ((int)floor(x / 2)) + pr * ((int)floor(y / 2));
        index = y * w + x;

        yp = p1[index];
        up = p2[uvIndex];
        vp = p3[uvIndex];
        rt = round(yp + vp * 1436 / 1024 - 179);
        gt = round(yp - up * 46549 / 131072 + 44 - vp * 93604 / 131072 + 91);
        bt = round(yp + up * 1814 / 1024 - 227);
        r = clamp(0, 255, rt);
        g = clamp(0, 255, gt);
        b = clamp(0, 255, bt);
        image[getRotatedImageByteIndex(y, x, h)] = (hexFF << 24) | (b << 16) | (g << 8) | r;
      }
    }
    return image;
  }
#ifdef __cplusplus
}
#endif
