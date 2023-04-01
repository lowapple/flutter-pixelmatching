#pragma once
#include <stdbool.h>
#ifdef __cplusplus
extern "C"
{
#endif
    void init();
    const char *version();
    unsigned char *extractFeaturesAndEncodeToJpeg(unsigned char *, int, int);
#ifdef __cplusplus
}
#endif
