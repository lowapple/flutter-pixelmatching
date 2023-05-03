project(tests)

cmake_minimum_required(VERSION 3.25)

set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE INTERNAL "" FORCE)
set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)

set(dirs_opencv ${OpenCV_INCLUDE_DIRS})
set(dirs_pixelmatching ${CMAKE_CURRENT_SOURCE_DIR}/../src)

include_directories(${dirs_opencv})
include_directories(${dirs_pixelmatching})

add_executable(
        pixelmatching_tester
        ${dirs_pixelmatching}/ImageProcessor.cpp
        ${dirs_pixelmatching}/ImageConverter.cpp
        ${dirs_pixelmatching}/ImageCompare.cpp
        ${dirs_pixelmatching}/DebugLogger.cpp
        ${dirs_pixelmatching}/PixelMatching.cpp
        main.cpp
)
target_link_libraries(pixelmatching_tester ${OpenCV_LIBS})
