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

  FUNCTION_ATTRIBUTE
  uchar *extractFeaturesAndEncodeToJpeg(uchar *data, int width, int height)
  {
    // Convert uchar* data to cv::Mat
    cv::Mat image = cv::imdecode(cv::Mat(1, width * height, CV_8UC1, data), cv::IMREAD_COLOR);

    // Convert image to grayscale
    cv::Mat grayImage;
    cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);

    // Extract keypoints and descriptors using ORB feature detector
    cv::Ptr<cv::ORB> orbDetector = cv::ORB::create();
    std::vector<cv::KeyPoint> keypoints;
    cv::Mat descriptors;
    orbDetector->detectAndCompute(grayImage, cv::Mat(), keypoints, descriptors);

    // Draw keypoints on the image (for visualization purposes only)
    cv::Mat imageWithKeypoints;
    cv::drawKeypoints(image, keypoints, imageWithKeypoints);

    // Rotate image by 90 degrees
    cv::Mat rotatedImage;
    cv::transpose(imageWithKeypoints, rotatedImage);
    cv::flip(rotatedImage, rotatedImage, 1);

    // Encode imageWithKeypoints to JPEG format
    std::vector<uchar> encodedData;
    std::vector<int> compressionParams = {cv::IMWRITE_JPEG_QUALITY, 90};
    cv::imencode(".jpg", rotatedImage, encodedData, compressionParams);

    // Convert encodedData to uchar*
    uchar *resultData = new uchar[encodedData.size()];
    std::copy(encodedData.begin(), encodedData.end(), resultData);

    return resultData;
  }

  // // 흑백 이미지 반환
  // /**
  //  * buf: jpeg encoded image
  //  * w: width
  //  * h: height
  //  * out: grayscale image
  //  */
  // FUNCTION_ATTRIBUTE
  // void grayscale(uchar *buf, int w, int h, uchar *out)
  // {
  //   logger_info("grayscale buf size %d", sizeof(buf));
  //   logger_info("grayscale w %d", w);
  //   logger_info("grayscale h %d", h);
  //   logger_info("grayscale out size %d", sizeof(out));

  //   cv::Mat image = cv::imdecode(cv::Mat(1, w * h, CV_8UC1, buf), cv::IMREAD_COLOR);
  //   cv::Mat gray;
  //   cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);

  //   std::vector<uchar> res;
  //   cv::imencode(".jpg", gray, res);
  //   memcpy(out, res.data(), res.size());
  // }

  // // code from
  // // https://github.com/Hugand/camera_tutorial/blob/master/ios/Classes/converter.c
  // int clamp(int lower, int higher, int val)
  // {
  //   if (val < lower)
  //     return 0;
  //   else if (val > higher)
  //     return 255;
  //   else
  //     return val;
  // }

  // int getRotatedImageByteIndex(int x, int y, int rw)
  // {
  //   return rw * (y + 1) - (x + 1);
  // }

  // // YUV 이미지를 RGB 이미지로 변환
  // /*
  //  * p1: Y
  //  * p2: U
  //  * p3: V
  //  * pr: U의 row stride
  //  * pp: U의 pixel stride
  //  * w: width
  //  * h: height
  //  *
  //  * return: RGB 이미지
  //  */
  // FUNCTION_ATTRIBUTE
  // uchar *yuv2rgb(uchar *p1, uchar *p2, uchar *p3, int pr, int pp, int w, int h)
  // {
  //   // Convert YUV to BGR
  //   Mat yuv(h + h / 2, w, CV_8UC1);
  //   uchar *imageData = yuv.data;
  //   memcpy(imageData, p1, w * h);
  //   memcpy(imageData + w * h, p2, w * h / 4);
  //   memcpy(imageData + w * h * 5 / 4, p3, w * h / 4);

  //   Mat bgr;
  //   cvtColor(yuv, bgr, COLOR_YUV2BGR_NV21);

  //   // Encode BGR image to JPEG
  //   std::vector<uchar> buffer;
  //   imencode(".jpg", bgr, buffer, {IMWRITE_JPEG_QUALITY, 80});

  //   // Copy JPEG data to a new array
  //   uchar *image = new uchar[buffer.size()];
  //   memcpy(image, buffer.data(), buffer.size());

  //   return image;

  //   // Mat yuv(h + h / 2, w, CV_8UC1);
  //   // uchar *imageData = yuv.data;
  //   // memcpy(imageData, p1, w * h);
  //   // memcpy(imageData + w * h, p2, w * h / 4);
  //   // memcpy(imageData + w * h * 5 / 4, p3, w * h / 4);

  //   // // Convert YUV to BGR
  //   // cv::Mat rgb;
  //   // cv::cvtColor(yuv, rgb, cv::COLOR_YUV2BGR_NV21);

  //   // // Convert BGR to uint32_t format
  //   // uchar *image = (uchar *)malloc(sizeof(uchar) * w * h * 3);
  //   // uchar *rgbData = rgb.data;
  //   // for (int i = 0; i < w * h; i++)
  //   // {
  //   //   image[i * 3 + 0] = rgbData[i * 3 + 0];
  //   //   image[i * 3 + 1] = rgbData[i * 3 + 1];
  //   //   image[i * 3 + 2] = rgbData[i * 3 + 2];
  //   // }

  //   // return image;
  // }
#ifdef __cplusplus
}
#endif
