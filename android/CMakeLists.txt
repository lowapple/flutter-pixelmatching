cmake_minimum_required(VERSION 3.4.1)

include_directories(../include ../ios/Classes)
add_library(lib_opencv SHARED IMPORTED)
set_target_properties(lib_opencv PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/src/main/cmakeLibs/${ANDROID_ABI}/libopencv_java4.so)

find_library(log-lib log)

add_library(
    opencv_pixelmatching 
    
    SHARED 
    
    ../ios/Classes/debug_logger.cpp
    ../ios/Classes/common.cpp
    ../ios/Classes/opencv_pixelmatching.cpp
    )
target_link_libraries(opencv_pixelmatching lib_opencv ${log-lib})