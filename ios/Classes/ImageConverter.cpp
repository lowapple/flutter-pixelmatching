#include "ImageConverter.hpp"

namespace imageconverter
{
	cv::Mat process(cv::Mat img)
	{
		cv::Mat gray;
		cv::cvtColor(img, gray, Constants::sourceImageCode);

		if (img.cols == Constants::sourceImageW && img.rows == Constants::sourceImageH)
		{
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

			return gray;
		}

		cv::Mat result;

		if ((double)img.rows / img.cols == (double)Constants::sourceImageH / Constants::sourceImageW)
		{
			cv::resize(gray, result, {Constants::sourceImageW, Constants::sourceImageH});
		}
		else
		{
			int cropSR = img.rows > img.cols ? (img.rows - img.cols) / 2 : 0;
			int cropSC = cropSR == 0 ? (img.cols - img.rows) / 2 : 0;
			int cropER = cropSR == 0 ? img.rows : cropSR + img.cols;
			int cropEC = cropSC == 0 ? img.cols : cropSC + img.rows;
			cv::resize(gray(cv::Range(cropSR, cropER), cv::Range(cropSC, cropEC)), result, {Constants::sourceImageW, Constants::sourceImageH});
		}

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

		return result;
	}
}