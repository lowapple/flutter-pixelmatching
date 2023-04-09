#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif
bool initialize();

int getStateCode();

bool setMarker(unsigned char *image, int width, int height, int rotation);

bool setQuery(unsigned char *image, int width, int height, int rotation);

double getQueryConfidenceRate();

unsigned char *getMarkerQueryDifferenceImage();

void dispose();
#ifdef __cplusplus
}
#endif
