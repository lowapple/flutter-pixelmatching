cmake_minimum_required(VERSION 3.4.1)

include_directories(../../../include)

add_library(
        lib_opencv
        SHARED
        IMPORTED
)

set_target_properties(
        lib_opencv
        PROPERTIES
        IMPORTED_LOCATION
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../android/src/main/opencv/${ANDROID_ABI}/libopencv_java4.so
)


add_library(
        flutter_pixelmatching
        SHARED
        DebugLogger.cpp
        ImageConverter.cpp
        ImageProcessor.cpp
        ImageCompare.cpp
        PixelMatching.cpp
)
set(BUILD_WRITERS ON)

if (ANDROID)
    find_library(log-lib log)
    target_link_libraries(
            flutter_pixelmatching
            lib_opencv
            ${log-lib}
    )
else ()
    target_link_libraries(
            flutter_pixelmatching
            lib_opencv
    )
endif ()