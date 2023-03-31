#pragma once

#include "opencv2/core/utility.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/calib3d.hpp"

using namespace cv;
using namespace std;

// 상태 코드 열거형
enum StateCode : int
{
    Not_Defined = 0x0,
    Not_Ready = 0x1,
    Not_Initialized = 0x11,
    Busy = 0x21,
    RetrivingTargetData = 0x61,
    ProcessingQueries = 0xA1,
    Idle = 0x2,
    ReadyToSetMarker = 0x2,
    ReadyToRetriveTargetData = 0x12,
    ReadyToProcessQueries = 0x32,
};

class Constants
{
private:
    // 생성자를 삭제하여 인스턴스화 방지
    ~Constants() = delete;

public:
    // 상수 정의
    static constexpr double version = 0.2;
    static constexpr double min_version = 0.1;
    static constexpr float threshold = 0.7f;
    static constexpr size_t buffer_size = 80 * 1024 * 1024;
    static constexpr double base_marker_width_ratio = 0.1;
    static constexpr int base_img_width = 256;
    static constexpr int base_img_height = 256;

    // Android 및 iOS에 따라 다른 값으로 정의
#ifdef __ANDROID__
    static constexpr int external_img_type = CV_8UC4;
    static constexpr int external_img_convertCode = cv::COLOR_RGBA2GRAY;
#endif
#ifdef __APPLE__
    static constexpr int external_img_type = CV_8UC3;
    static constexpr int external_img_convertCode = cv::COLOR_RGBA2GRAY;
#endif

    // 함수 선언
    int GetStateCode();
    double Initialize(double wrapper_version);
    char *GetPtrOfBuffer();
    int SetMarker(cv::Mat img_marker);
    int SetMarker(char *ptr_marker, int img_width, int img_height);
    int SetMarker(int img_width, int img_height);
    int SetMarkerData();
    int Start_RetriveTargetData();
    int End_RetriveTargetData();
    int ImportTargetData();
    int ExportTargetData();
    int Start_ProcessQuery();
    int End_ProcessQuery();
    double GetConfidenceRate();
    int ExportMostAccurateQueryImage();
    int Process(cv::Mat img);
    int Process(char *ptr_img, int img_width, int img_height);
    int Process(int img_width, int img_height);
};
