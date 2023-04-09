//
// Created by Junseo Youn on 2023/04/09.
//

#include "DebugLogger.h"
#include "ImageCompare.h"

ImageCompare::ImageCompare() {
    clahe = createCLAHE();
    matrix = Mat::zeros(3, 3, CV_64F);
    matrix.at<double>(0, 0) = 1;
    matrix.at<double>(1, 1) = 1;
    matrix.at<double>(2, 2) = 1;
}

ImageCompare::~ImageCompare() {
    cvMatchers->clear();
    cvMatchers = nullptr;
    cvDetector->clear();
    cvDetector = nullptr;
    clahe.release();
    clahe = nullptr;
    keypointsMarker.clear();
    keypointsQuery.clear();
    descriptorsMarker.release();
    descriptorsQuery.release();
    imageQuery.release();
    imageQueryAligned.release();
    imageMarker.release();
    imageMarkerMasked.release();
    imageMaskMarker.release();
    imageMaskQuery.release();
    imageEqualizedMarker.release();
    imageEqualizedQuery.release();
}

void ImageCompare::setDetector(const Ptr<Detector> &detector) {
    this->cvDetector = detector;
}

void ImageCompare::setMatchers(const Ptr<Matcher> &matcher) {
    this->cvMatchers = matcher;
}

bool ImageCompare::compare() {
    std::vector<std::vector<DMatch>> matches;
    try {
        cvMatchers->knnMatch(descriptorsMarker, descriptorsQuery, matches, 2);
    }
    catch (const cv::Exception &e) {
        logger_e("compare - knnMatch - cv::Exception: %s", e.what());
        return false;
    }
    catch (const std::exception &e) {
        logger_e("compare - knnMatch - std::exception: %s", e.what());
        return false;
    }

    std::vector<DMatch> selectedMatches;
    selectedMatches.reserve(matches.size());
    for (auto &match: matches) {
        if (match[0].distance < Constants::threshold * match[1].distance) {
            selectedMatches.emplace_back(match[0].queryIdx, match[0].trainIdx, match[0].distance);
        }
    }

    if (selectedMatches.size() <= 4) {
        matches.clear();
        selectedMatches.clear();
        return false;
    }

    const size_t numMatches = selectedMatches.size();
    auto *tarPoints = new cv::Point2f[numMatches];
    auto *qryPoints = new cv::Point2f[numMatches];
    for (size_t i = 0; i < numMatches; ++i) {
        const auto &match = selectedMatches[i];
        tarPoints[i] = keypointsMarker[match.queryIdx].pt;
        qryPoints[i] = keypointsQuery[match.trainIdx].pt;
    }

    std::vector<cv::Point2f> mappedPointsMarker(tarPoints, tarPoints + numMatches);
    std::vector<cv::Point2f> mappedPointsQuery(qryPoints, qryPoints + numMatches);

    delete[] tarPoints;
    delete[] qryPoints;
    // 원근 변환 행렬 계산
    Mat H = findHomography(mappedPointsMarker, mappedPointsQuery, RANSAC, 1.0, noArray());

    // 원근 변환 실패시 실패 처리
    if (H.data == nullptr) {
        matches.clear();
        selectedMatches.clear();
        H.release();
        mappedPointsMarker.clear();
        mappedPointsQuery.clear();
        return false;
    }

    // 원근 변환 행렬 역행렬 계산
    Mat M = H.inv();
    M /= M.at<double>(2, 2);

    // 이미지 원근 변환
    warpPerspective(imageQuery, imageQueryAligned, M, imageQuery.size());

    // 마스크 생성
    imageMaskQuery = Mat::ones(imageQuery.size(), CV_8U);

    // 비교 대상 이미지에 대응되지 않는 부분은 제거하기 위한 마스크 생성
    imageMarkerMasked = imageMarker.clone();
    for (int row = 0; row < imageQueryAligned.rows; row++) {
        for (int column = 0; column < imageQueryAligned.cols; column++) {
            if (imageQueryAligned.at<uint8_t>(row, column) == 0) {
                imageMarkerMasked.at<uint8_t>(row, column) = 0;
                imageMaskQuery.at<uint8_t>(row, column) = 0;
            }
        }
    }

    // ExportImg 사용시 아래 주석 처리
    matches.clear();
    selectedMatches.clear();
    H.release();
    mappedPointsMarker.clear();
    mappedPointsQuery.clear();
    M.release();
    // imageMarkerMasked.release();
    return true;
}

bool ImageCompare::setMarker(cv::Mat marker) {
    logger_i("[pixelmatching] setMarker");
    imageMarker.release();
    imageMarker = marker.clone();
    cvDetector->detectAndCompute(imageMarker, noArray(), keypointsMarker, descriptorsMarker);
    cvMatchers->add(descriptorsMarker);
    marker.release();
    return true;
}

bool ImageCompare::setQuery(cv::Mat query) {
    logger_i("[pixelmatching] setQuery");
    imageQuery.release();
    imageQuery = query.clone();
    cvDetector->detectAndCompute(imageQuery, noArray(), keypointsQuery, descriptorsQuery);
    cvMatchers->add(descriptorsQuery);
    query.release();
    return compare();
}

double ImageCompare::getConfidenceRate() {
    if (imageQueryAligned.empty()) {
        return -1.0;
    }

    Mat res;
    clahe->apply(imageMarker, imageEqualizedMarker);
    clahe->apply(imageQueryAligned, imageEqualizedQuery);
    matchTemplate(imageEqualizedMarker, imageEqualizedQuery, res, TemplateMatchModes::TM_CCORR_NORMED,
                  imageMaskQuery);
    return res.at<float>(0, 0);
}

Mat ImageCompare::getImageMarker() {
    return imageMarker;
}

Mat ImageCompare::getImageQuery() {
    return imageQuery;
}