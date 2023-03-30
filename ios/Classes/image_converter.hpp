#pragma once

#include "common.hpp"

class ImageConverter
{
private:
    ~ImageConverter() = delete;

public:
    static Mat Convert(Mat img);

    static Mat Convert(char *ptr, int img_width, int img_height);
};