#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "YuvToRGB.h"
#include "opencv2/core/utility.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/calib3d.hpp"

#if defined(__GNUC__)
#define FUNCTION_ATTRIBUTE __attribute__((visibility("default"))) __attribute__((used))
#elif defined(_MSC_VER)
#define FUNCTION_ATTRIBUTE __declspec(dllexport)
#endif
#ifdef __cplusplus
extern "C"
{
#endif

int clamp(int lower, int higher, int val) {
    if (val < lower)
        return 0;
    else if (val > higher)
        return 255;
    else
        return val;
}

FUNCTION_ATTRIBUTE
uint8_t *YuvToRGB(uint8_t *plane0,  // Y
                  uint8_t *plane1,  // Cb
                  uint8_t *plane2,  // Cr
                  int bytesPerRow,  // getRowStride
                  int bytesPerPixel,// getPixelStride
                  int width,        // getWidth
                  int height        // getHeight
) {
    int y, uv, index;
    int yp, up, vp;
    int r, g, b;
    int rt, gt, bt;

    uint8_t *image = (uint8_t *) malloc(width * height * 3);

    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            y = row * width + col;
            uv = bytesPerPixel * ((int) floor(col / 2)) +
                 bytesPerRow * ((int) floor(row / 2));
            index = y * 3;
            yp = plane0[y];
            up = plane1[uv];
            vp = plane2[uv];
            rt = round(yp + vp * 1436 / 1024 - 179);
            gt = round(yp - up * 46549 / 131072 + 44 - vp * 93604 / 131072 + 91);
            bt = round(yp + up * 1814 / 1024 - 227);
            r = clamp(0, 255, rt);
            g = clamp(0, 255, gt);
            b = clamp(0, 255, bt);
            image[index] = r;
            image[index + 1] = g;
            image[index + 2] = b;
        }
    }
    return image;
}

#ifdef __cplusplus
}
#endif
