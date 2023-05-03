#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @return initialize result
*/
bool initialize();

/**
 * @return state code
*/
int getStateCode();

/**
 * @return set marker result
*/
bool setMarker(const char *imageType, unsigned char *image, int width, int height, int rotation);


/**
 * @return set query result
*/
bool setQuery(const char *imageType, unsigned char *image, int width, int height, int rotation);

/**
 * match marker and query
 * @return match confidence rate
*/
double getQueryConfidenceRate();

/**
 * @param size output image size
 * @return output image bytes(encoded by jpeg)
*/
unsigned char *getMarkerQueryDifferenceImage(int *size);

void dispose();

#ifdef __cplusplus
}
#endif
