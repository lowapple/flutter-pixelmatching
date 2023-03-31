#pragma once
#include <stdbool.h>
#ifdef __cplusplus
extern "C"
{
#endif
    void init();
    const char *version();
    void grayscale(unsigned char *buf, int w, int h, bool isYuv, unsigned char *out);
    unsigned int *yuv2rgb(unsigned char *p1, unsigned char *p2, unsigned char *p3, int pr, int pp, int w, int h);
#ifdef __cplusplus
}
#endif
