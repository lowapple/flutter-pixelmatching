#include "image_cache.hpp"

ImageCache::ImageCache(char *buffer) : buffer(buffer), state(0), count_imgs_target(0), count_imgs_query(0)
{
}

int ImageCache::GetState()
{
    return state;
}

void ImageCache::Clear()
{
    state = 0;
    count_imgs_target = 0;
    count_imgs_query = 0;
}