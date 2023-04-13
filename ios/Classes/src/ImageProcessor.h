#pragma once

#include "Constants.h"
#include "ImageCompare.h"
#include "DebugLogger.h"
#include <queue>

class ImageProcessor {
private:
    ImageCompare compare;
private:
    Mutex mutex;
private:
    StateCode stateCode;
public:
    ImageProcessor();

    void initialize();

    void setStateCode(StateCode code);

    StateCode getStateCode();

    bool setMarker(Mat marker);

    bool setQuery(Mat query);

    double getConfidenceRate();

    Mat getImageMarker();

    Mat getImageQuery();
};
