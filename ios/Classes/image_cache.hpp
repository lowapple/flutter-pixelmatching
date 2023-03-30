#pragma once

#include "common.hpp"
#include <map>

class ImageCache
{
private:
    ImageCache(const ImageCache &) = delete;
    ImageCache(ImageCache &&) = delete;
    ImageCache &operator=(const ImageCache &) = delete;
    ImageCache &operator=(ImageCache &&) = delete;
    int state;

public:
    ImageCache(char *buffer);
    ~ImageCache() = default;

    char *const buffer;

    Mat img_marker;

    map<int, Mat> imgs_target;
    int count_imgs_target;

    map<int, Mat> imgs_query;
    int count_imgs_query;

    int GetState();

    void Clear();
};