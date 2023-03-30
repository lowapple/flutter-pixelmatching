#include "image_converter.hpp"

cv::Mat ImageConverter::Convert(cv::Mat img)
{
    cv::Mat gray;

    cv::cvtColor(img, gray, Constants::external_img_convertCode);

    if (img.cols == Constants::base_img_width && img.rows == Constants::base_img_height)
    {
        // 0 -> 1�� ����
        for (int row = 0; row < gray.rows; row++)
        {
            for (int column = 0; column < gray.cols; column++)
            {
                if (gray.at<uint8_t>(row, column) == 0)
                {
                    gray.at<uchar>(row, column) = 1;
                }
            }
        }

        return gray;
    }

    cv::Mat result;

    if ((double)img.rows / img.cols == (double)Constants::base_img_height / Constants::base_img_width)
    {
        cv::resize(gray, result, {Constants::base_img_width, Constants::base_img_height});
    }
    else
    {
        int crop_start_row = img.rows > img.cols ? (img.rows - img.cols) / 2 : 0;

        int crop_start_column = crop_start_row == 0 ? (img.cols - img.rows) / 2 : 0;

        int crop_end_row = crop_start_row == 0 ? img.rows : crop_start_row + img.cols;

        int crop_end_column = crop_start_column == 0 ? img.cols : crop_start_column + img.rows;

        cv::resize(gray(cv::Range(crop_start_row, crop_end_row), cv::Range(crop_start_column, crop_end_column)), result, {Constants::base_img_width, Constants::base_img_height});
    }

    for (int row = 0; row < result.rows; row++)
    {
        for (int column = 0; column < result.cols; column++)
        {
            if (result.at<uint8_t>(row, column) == 0)
            {
                result.at<uint8_t>(row, column) = 1;
            }
        }
    }

    return result;
}

// Matrixȭ + Color space �� ũ�� ����
cv::Mat ImageConverter::Convert(char *ptr, int img_width, int img_height)
{
    return Convert({img_height, img_width, Constants::external_img_type, ptr});
}
