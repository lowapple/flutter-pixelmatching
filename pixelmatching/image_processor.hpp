#pragma once

#include "common.h"
#include "debug_logger.hpp"
#include <queue>

class ImageProcessor
{
private:
    struct Comparator_Base
    {
        virtual ~Comparator_Base() = default;
        virtual int Process(Mat img, bool isTargetImage) = 0;
        virtual Mat ExportImg(int code) = 0;
        virtual double getQueryConfidenceRate() = 0;
    };

public:
    template <class DetectorType, DescriptorMatcher::MatcherType matcherType>
    struct ComparatorDetector : public Comparator_Base
    {
        Ptr<DetectorType> detector;     // 키포인트 검출기 (OpenCV의 다양한 검출기 중 하나)
        Ptr<DescriptorMatcher> matcher; // 디스크립터 매처 (OpenCV의 다양한 매칭 알고리즘 중 하나)

        std::vector<KeyPoint> keypointsTarget, keypointsQuery; // 검색 대상(Target) 이미지와 입력(Input) 이미지에서 검출된 키포인트
        Mat descriptorsTarget, descriptorsQuery;               // 검색 대상(Target) 이미지와 입력(Input) 이미지에서 추출된 디스크립터

        Mat imageQuery, imageQueryAligned, imageTarget, imageTargetMasked; // 입력(Input) 이미지, 입력(Input) 이미지를 검색 대상(Target) 이미지와 맞춘 이미지, 검색 대상(Target) 이미지, 검색 대상(Target) 이미지를 마스크로 처리한 이미지
        Mat maskTarget, maskQuery;                                         // 검색 대상(Target) 이미지와 입력(Input) 이미지의 마스크

        int rows_marker = 0, cols_marker = 0; // 마커의 행 개수, 열 개수
        Mat translateMatrix;                  // 이미지 이동을 위한 변환 행렬

        Ptr<CLAHE> clahe;                              // CLAHE(Contrast Limited Adaptive Histogram Equalization) 객체
        Mat imageTargetEqualized, imageQueryEqualized; // 검색 대상(Target) 이미지에 CLAHE를 적용한 이미지, 입력(Input) 이미지를 검색 대상(Target) 이미지와 맞춘 이미지에 CLAHE를 적용한 이미지

        ~ComparatorDetector()
        {
            matcher->clear();
            matcher = nullptr;
            detector->clear();
            detector = nullptr;
        }

        ComparatorDetector() : detector(DetectorType::create()),
                               matcher(DescriptorMatcher::create(matcherType)),
                               clahe(createCLAHE())
        {
            translateMatrix = Mat::zeros(3, 3, CV_64F);
            translateMatrix.at<double>(0, 0) = 1;
            translateMatrix.at<double>(1, 1) = 1;
            translateMatrix.at<double>(2, 2) = 1;
        }

        int Process(Mat img, bool isTargetImage) override
        {
            if (isTargetImage)
            {
                imageTarget.release();
                imageTarget = img;
            }
            else
            {
                imageQuery.release();
                imageQuery = img;
            }

            logger_info("Process()");
            if (isTargetImage)
            {
                logger_info("Process() - isTargetImage");
                detector->detectAndCompute(img, noArray(), keypointsTarget, descriptorsTarget);
                matcher->add(descriptorsTarget);
                // imageTarget = img;
                return 1;
            }
            else
            {
                logger_info("Process() - isQueryImage");
                // 이미지에서 특징점과 디스크립터 추출
                detector->detectAndCompute(img, noArray(), keypointsQuery, descriptorsQuery);
                // 디스크립터를 매처에 추가
                if (!matcher->empty())
                {
                    matcher->clear();
                    matcher->add(descriptorsTarget);
                }
                matcher->add(descriptorsQuery);

                // 최근접 이웃 매칭
                std::vector<std::vector<DMatch>> matches;
                try
                {
                    logger_info("Process() - knnMatch");
                    matcher->knnMatch(descriptorsTarget, descriptorsQuery, matches, 2);
                }
                catch (const cv::Exception &e)
                {
                    logger_error("Process() - knnMatch - cv::Exception: %s", e.what());
                    return 0;
                }
                catch (const std::exception &e)
                {
                    logger_error("Process() - knnMatch - std::exception: %s", e.what());
                    return 0;
                }

                // 거리 임계값을 기준으로 최근접 이웃 중 좋은 매칭점만 선택
                std::vector<DMatch> chosenMatches;
                chosenMatches.reserve(matches.size());
                for (auto &match : matches)
                {
                    if (match[0].distance < Constants::threshold * match[1].distance)
                    {
                        chosenMatches.emplace_back(match[0].queryIdx, match[0].trainIdx, match[0].distance);
                    }
                }

                // 선택된 매칭점이 너무 적으면 실패
                if (chosenMatches.size() <= 4)
                {
                    matches.clear();
                    chosenMatches.clear();
                    return 0;
                }

                // 매칭점 좌표 추출
                // 대응되는 좌표를 추출하여 배열에 저장
                const size_t numMatches = chosenMatches.size();
                cv::Point2f *tarPoints = new cv::Point2f[numMatches];
                cv::Point2f *qryPoints = new cv::Point2f[numMatches];
                for (size_t i = 0; i < numMatches; ++i)
                {
                    const auto &match = chosenMatches[i];
                    tarPoints[i] = keypointsTarget[match.queryIdx].pt;
                    qryPoints[i] = keypointsQuery[match.trainIdx].pt;
                }

                // 배열을 std::vector에 복사
                std::vector<cv::Point2f> mappedTarget(tarPoints, tarPoints + numMatches);
                std::vector<cv::Point2f> mappedPointsQuery(qryPoints, qryPoints + numMatches);

                // 할당된 메모리 해제
                delete[] tarPoints;
                delete[] qryPoints;

                //                std::vector<Point2f> mappedTarget, mappedPointsQuery;
                //                for (auto &match : chosenMatches)
                //                {
                //                    mappedTarget.push_back(keypointsTarget[match.queryIdx].pt);
                //                    mappedPointsQuery.push_back(keypointsQuery[match.trainIdx].pt);
                //                }

                // 원근 변환 행렬 계산
                Mat H = findHomography(mappedTarget, mappedPointsQuery, RANSAC, 1.0, noArray());

                // 원근 변환 실패시 실패 처리
                if (H.data == 0)
                {
                    matches.clear();
                    chosenMatches.clear();
                    H.release();
                    mappedTarget.clear();
                    mappedPointsQuery.clear();
                    return 0;
                }

                // 원근 변환 행렬 역행렬 계산
                Mat M = H.inv();
                M /= M.at<double>(2, 2);

                // 이미지 원근 변환
                warpPerspective(img, imageQueryAligned, M, img.size());

                // 마스크 생성
                maskQuery = Mat::ones(img.size(), CV_8U);

                // 비교 대상 이미지에 대응되지 않는 부분은 제거하기 위한 마스크 생성
                imageTargetMasked = imageTarget.clone();
                for (int row = 0; row < imageQueryAligned.rows; row++)
                {
                    for (int column = 0; column < imageQueryAligned.cols; column++)
                    {
                        if (imageQueryAligned.at<uint8_t>(row, column) == 0)
                        {
                            imageTargetMasked.at<uint8_t>(row, column) = 0;
                            maskQuery.at<uint8_t>(row, column) = 0;
                        }
                    }
                }

                // ExportImg 사용시 아래 주석 처리
                matches.clear();
                chosenMatches.clear();
                H.release();
                mappedTarget.clear();
                mappedPointsQuery.clear();
                M.release();
                imageTargetMasked.release();
                return 1;
            }
        }

        Mat ExportImg(int code) override
        {
            switch (code)
            {
            case 1:
                return imageTarget;
            case 2:
                return imageQuery;
            case 3:
                return imageTargetMasked;
            case 4:
                return imageQueryAligned;
            default:
                return {};
            }
        }

        double getQueryConfidenceRate() override
        {
            if (imageQueryAligned.empty())
            {
                return -1.0;
            }

            Mat result;

            clahe->apply(imageTarget, imageTargetEqualized);
            clahe->apply(imageQueryAligned, imageQueryEqualized);

            matchTemplate(imageTargetEqualized, imageQueryEqualized, result, TemplateMatchModes::TM_CCORR_NORMED, maskQuery);
            return result.at<float>(0, 0);
        }
    };

    StateCode stateCode = StateCode::NotInitialized;
    double confidence_rate = -1.0;

public:
    std::vector<Comparator_Base *> comparators;

private:
    ImageProcessor(const ImageProcessor &) = delete;
    ImageProcessor(ImageProcessor &&) = delete;
    ImageProcessor &operator=(const ImageProcessor &) = delete;
    ImageProcessor &operator=(ImageProcessor &&) = delete;

public:
    ImageProcessor();
    ~ImageProcessor();
    // 초기화
    void initialize();
    // 상태 코드 반환
    int getStateCode();
    // 타겟 등록
    bool setSourceImage(Mat img);
    // 쿼리 이미지 등록
    bool setQueryImage(Mat img);
    // 비교 결과
    double getQueryConfidenceRate();
    // 이미지 처리
    int process(Mat img, bool needConvert);
};
