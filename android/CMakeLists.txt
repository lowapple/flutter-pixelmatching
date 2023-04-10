cmake_minimum_required(VERSION 3.4.1)
# add opencv library to project
include_directories(../include ../pixelmatching)
# add library
add_library(
    lib_opencv 
    SHARED 
    IMPORTED
    )
set_target_properties(
    lib_opencv 
    PROPERTIES 
    IMPORTED_LOCATION 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main/opencv/${ANDROID_ABI}/libopencv_java4.so
)

find_library(log-lib log)

add_library(
    opencv_pixelmatching 
    SHARED 
    ../pixelmatching/DebugLogger.cpp
    ../pixelmatching/ImageConverter.cpp
    ../pixelmatching/ImageProcessor.cpp
    ../pixelmatching/ImageCompare.cpp
    ../pixelmatching/PixelMatching.cpp
    )
target_link_libraries(
    opencv_pixelmatching 
    lib_opencv 
    ${log-lib}
)