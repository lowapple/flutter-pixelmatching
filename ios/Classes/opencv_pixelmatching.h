#pragma once
#include <stdbool.h>
#ifdef __cplusplus
extern "C"
{
#endif
    void init();
    const char *version();
    void grayscale(unsigned char *buf, int w, int h, bool isYuv, unsigned char *out);
#ifdef __cplusplus
}
#endif
