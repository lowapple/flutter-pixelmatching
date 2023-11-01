#ifdef __cplusplus
extern "C"
{
#endif

uint8_t *YuvToRGB(uint8_t *plane0,  // Y
                  uint8_t *plane1,  // Cb
                  uint8_t *plane2,  // Cr
                  int bytesPerRow,  // getRowStride
                  int bytesPerPixel,// getPixelStride
                  int width,        // getWidth
                  int height        // getHeight
);

#ifdef __cplusplus
}
#endif