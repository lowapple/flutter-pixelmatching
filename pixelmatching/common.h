#pragma once

#include "opencv2/core/utility.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/calib3d.hpp"

#include "state_code.hpp"

using namespace cv;

class Constants
{
private:
	~Constants() = delete;

public:
	// Feature point matching에서 threshold로 사용되는 값
	// 높은 값을 사용하면 정확도가 높아지지만 잘못된 결과를 반환할 가능성이 있음
	static constexpr float threshold = 0.7f;

	// 라이브러리에서 사용하는 buffer의 크기
	static constexpr size_t sourceBufferSize = 80 * 1024 * 1024;
	static constexpr double sourceRatio = 0.1;
	static constexpr int sourceImageW = 256;
	static constexpr int sourceImageH = 256;
#ifdef __ANDROID__
	static constexpr int sourceImageType = CV_8UC4;
	static constexpr int sourceImageCode = cv::COLOR_RGBA2GRAY;
#endif
#ifdef __APPLE__
	static constexpr int sourceImageType = CV_8UC4;
	static constexpr int sourceImageCode = cv::COLOR_BGRA2GRAY;
#endif
};
