#pragma once

#include <stdbool.h>
#ifdef __cplusplus
extern "C"
{
#endif
    // 초기화 함수
    bool initialize();

    // 현재 상태값 반환 함수
    int getStatusCode();

    // 대상 이미지 등록 함수
    bool setTargetImage(unsigned char *image, int width, int height, int rotation);

    // 비교 이미지 등록 함수
    bool setQueryImage(unsigned char *image, int width, int height, int rotation);

    // 이미지 비교 함수
    double getQueryConfidenceRate();

    // 메모리 해제 함수
    void dispose();
#ifdef __cplusplus
}
#endif
