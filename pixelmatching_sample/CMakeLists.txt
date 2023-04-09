cmake_minimum_required(VERSION 3.25)
set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE INTERNAL "" FORCE)
project(pixelmatching)
set(CMAKE_CXX_STANDARD 17)
find_package(OpenCV REQUIRED)
include_directories(../pixelmatching)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(pixelmatching
        ../pixelmatching/DebugLogger.cpp
        ../pixelmatching/ImageConverter.cpp
        ../pixelmatching/ImageProcessor.cpp
        ../pixelmatching/PixelMatching.cpp
        ../pixelmatching/ImageCompare.cpp
        PixelMatchingMain.cpp
        )
target_link_libraries(pixelmatching ${OpenCV_LIBS})