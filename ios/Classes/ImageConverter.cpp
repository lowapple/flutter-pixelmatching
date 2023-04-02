#include "ImageConverter.hpp"

namespace imageconverter
{
	cv::Mat process(cv::Mat img)
	{
		cv::Mat gray;
		cv::cvtColor(img, gray, Constants::sourceImageCode); // 입력 이미지를 그레이 스케일로 변환합니다.

		// 입력 이미지의 크기가 Constants::sourceImageW와 Constants::sourceImageH로 지정된 값과 일치하는지 확인합니다.
		if (img.cols == Constants::sourceImageW && img.rows == Constants::sourceImageH)
		{
			// 이미지의 모든 픽셀을 반복하면서 0인 픽셀을 1로 바꿉니다.
			for (int r = 0; r < gray.rows; r++)
			{
				for (int c = 0; c < gray.cols; c++)
				{
					if (gray.at<uchar>(r, c) == 0)
					{
						gray.at<uchar>(r, c) = 1;
					}
				}
			}

			return gray; // 처리된 이미지를 반환합니다.
		}

		cv::Mat result;

		// 입력 이미지의 가로 세로 비율이 Constants::sourceImageW와 Constants::sourceImageH의 가로 세로 비율과 동일한지 확인합니다.
		if ((double)img.rows / img.cols == (double)Constants::sourceImageH / Constants::sourceImageW)
		{
			cv::resize(gray, result, {Constants::sourceImageW, Constants::sourceImageH}); // 이미지 크기를 조정합니다.
		}
		else
		{
			// 입력 이미지를 가운데 크롭(crop)합니다.
			int cropSR = img.rows > img.cols ? (img.rows - img.cols) / 2 : 0;
			int cropSC = cropSR == 0 ? (img.cols - img.rows) / 2 : 0;
			int cropER = cropSR == 0 ? img.rows : cropSR + img.cols;
			int cropEC = cropSC == 0 ? img.cols : cropSC + img.rows;
			cv::resize(gray(cv::Range(cropSR, cropER), cv::Range(cropSC, cropEC)), result, {Constants::sourceImageW, Constants::sourceImageH});
		}

		// 이미지의 모든 픽셀을 반복하면서 0인 픽셀을 1로 바꿉니다.
		for (int r = 0; r < result.rows; r++)
		{
			for (int c = 0; c < result.cols; c++)
			{
				if (result.at<uchar>(r, c) == 0)
				{
					result.at<uchar>(r, c) = 1;
				}
			}
		}

		return result; // 처리된 이미지를 반환합니다.
	}
}