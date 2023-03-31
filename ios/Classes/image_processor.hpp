
/*
    ImageProcessor.hpp

    Image 처리 모듈을 의미하는 class lpin::ImageProcessor에 대한 정의가 적혀 있는 헤더 파일입니다.

    비템플릿 멤버 함수들에 대한 정의는 ImageProcessor.cpp에 있습니다.
*/

#pragma once
#define USE_HISTOGRAM_EQUALIZER

#include "debug_logger.hpp"
#include "common.hpp"
#include "image_cache.hpp"
#include <queue>

class ImageProcessor
{
private:
    struct Comparator_Base
    {
        virtual int Process(Mat img, bool isTargetImage) = 0;
        virtual Mat ExportImg(int code) = 0;
        virtual double GetConfidenceRate() = 0;
    };

public:
    template <class Detector_t, DescriptorMatcher::MatcherType matcherType>
    struct Comparator_BuiltInDetector : public Comparator_Base
    {
        Ptr<Detector_t> detector;
        Ptr<DescriptorMatcher> matcher;

        vector<KeyPoint> keypoints_target, keypoints_toQuery;
        Mat descriptors_target, descriptors_toQuery;

        Mat img_target, img_toQuery, img_toQuery_aligned, img_target_masked;

        Mat mask_target, mask_toQuery;

        int rows_marker = 0, cols_marker = 0;

        Mat translate_matrix;

#ifdef USE_HISTOGRAM_EQUALIZER
        Ptr<CLAHE> clahe;
        Mat img_target_equalized, img_toQuery_equalized;
#endif

        Comparator_BuiltInDetector() : detector(Detector_t::create()),
                                       matcher(DescriptorMatcher::create(matcherType)),
#ifdef USE_HISTOGRAM_EQUALIZER
                                       clahe(createCLAHE())
#endif
        {
            translate_matrix = Mat::zeros(3, 3, CV_64F);
            translate_matrix.at<double>(0, 0) = 1;
            translate_matrix.at<double>(1, 1) = 1;
            translate_matrix.at<double>(2, 2) = 1;
        }

        int Process(Mat img, bool isTargetImage) override
        {
            if (isTargetImage)
            {
                // Detect keypoints and compute descriptors for the target image
                detector->detectAndCompute(img, noArray(), keypoints_target, descriptors_target);

                // Add the target image's descriptors to the matcher
                matcher->add(descriptors_target);

                // Store the target image
                img_target = img;

                return 1;
            }
            else
            {
                // Detect keypoints and compute descriptors for the target image.
                detector->detectAndCompute(img_target, noArray(), keypoints_target, descriptors_target);

                // Detect keypoints and compute descriptors for the query image.
                detector->detectAndCompute(img_query, noArray(), keypoints_query, descriptors_query);

                // Add the descriptors of the query image to the matcher.
                matcher->add(descriptors_query);

                // find the 2 closest matches for each keypoint
                vector<vector<DMatch>> knn_matches;
                matcher->knnMatch(descriptors_target, descriptors_query, knn_matches, 2);

                vector<DMatch> chosen_matches;
                // filter matches using the Lowe's ratio test
                for (auto &match : knn_matches)
                    if (match[0].distance < Constants::threshold * match[1].distance)
                        chosen_matches.push_back(match[0]);

                if (chosen_matches.size() <= 4)
                    return 0;

                vector<Point2f> mapped_points_query;
                vector<Point2f> mapped_points_toTarget;
                for (auto &match : chosen_matches)
                {
                    mapped_points_query.push_back(keypoints_toQuery[match.queryIdx].pt);
                    mapped_points_toTarget.push_back(keypoints_target[match.trainIdx].pt);
                }

                Mat H = findHomography(mapped_points_toTarget, mapped_points_query, RANSAC, 1.0, noArray());

                if (H.data == 0)
                    return 0;

                Mat M = H.inv();
                M /= M.at<double>(2, 2);

                warpPerspective(img, img_toQuery_aligned, M, img.size());

                mask_toQuery = Mat::ones(img.size(), CV_8U);

                img_target_masked = img_target.clone();

                for (int row = 0; row < img_toQuery_aligned.rows; row++)
                {
                    for (int column = 0; column < img_toQuery_aligned.cols; column++)
                    {
                        if (img_toQuery_aligned.at<uint8_t>(row, column) == 0)
                        {
                            img_target_masked.at<Vec3b>(row, column) = 0;

                            mask_toQuery.at<uint8_t>(row, column) = 0;
                        }
                    }
                }

                return 1;
            }
        }

        Mat ExportImg(int code) override
        {
            switch (code)
            {
            case 1:
                return img_target;
            case 2:
                return img_toQuery;
            case 3:
                return img_target_masked;
            case 4:
                return img_toQuery_aligned;
            default:
                return {};
            }
        }

        double GetConfidenceRate() override
        {
            if (img_toQuery_aligned.empty())
            {
                return -1.0;
            }

            Mat result;

#ifdef USE_HISTOGRAM_EQUALIZER
            clahe->apply(img_target, img_target_equalized);
            clahe->apply(img_toQuery_aligned, img_toQuery_equalized);

            matchTemplate(img_target_equalized, img_toQuery_equalized, result, TemplateMatchModes::TM_CCORR_NORMED, mask_toQuery);
#else
            matchTemplate(img_target, img_toQuery_aligned, result, TemplateMatchModes::TM_CCORR_NORMED, mask_toQuery);
#endif
            return result.at<float>(0, 0);
        }
    };

    StateCode stateCode = ReadyToRetriveTargetData;
    double confidence_rate = -1.0;

public:
    vector<Comparator_Base *> comparators;

private:
    char *const buffer;

    ImageProcessor(const ImageProcessor &) = delete;
    ImageProcessor(ImageProcessor &&) = delete;
    ImageProcessor &operator=(const ImageProcessor &) = delete;
    ImageProcessor &operator=(ImageProcessor &&) = delete;

public:
    ImageCache cache;

    ImageProcessor();
    ~ImageProcessor();

    int GetStateCode();
    double Initialize(double wrapper_version);
    char *GetPtrOfBuffer();
    int SetMarker(Mat img_marker, bool needConvert);
    int SetMarker(char *ptr_marker, int img_width, int img_height);
    int SetMarker(int img_width, int img_height);
    int SetMarkerData();
    int Start_RetriveTargetData();
    int End_RetriveTargetData();
    int ImportTargetData();
    int ExportTargetData();
    int Start_ProcessQuery();
    int End_ProcessQuery();
    double GetConfidenceRate();
    int ExportMostAccurateQueryImage();
    int Process(Mat img, bool needConvert);
    int Process(char *ptr_img, int width, int height);
    int Process(int width, int height);
};