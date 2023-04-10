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
    StateCode stateCode = StateCode::NotInitialized;
public:
    ImageProcessor();

    void initialize();

    void setStateCode(StateCode stateCode);

    [[nodiscard]] StateCode getStateCode() const;

    bool setMarker(Mat marker);

    bool setQuery(Mat query);

    double confidence();

    Mat getImageMarker();

    Mat getImageQuery();
};
