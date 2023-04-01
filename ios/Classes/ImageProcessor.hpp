/*
	ImageProcessor.hpp

	Image 처리 모듈을 의미하는 class pixelmatching::ImageProcessor에 대한 정의가 적혀 있는 헤더 파일입니다.

	비템플릿 멤버 함수들에 대한 정의는 ImageProcessor.cpp에 있습니다.
*/

#pragma once

#include "common.hpp"

#include <queue>

using namespace cv;

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
	struct Comparator_BuiltInDetector : public Comparator_Base
	{
		Ptr<DetectorType> detector;
		Ptr<DescriptorMatcher> matcher;

		std::vector<KeyPoint> keypoints_target, keypoints_toQuery;
		Mat descriptors_target, descriptors_toQuery;

		Mat img_target, img_toQuery, img_toQuery_aligned, img_target_masked;

		Mat mask_target, mask_toQuery;

		int rows_marker = 0, cols_marker = 0;

		Mat translate_matrix;

		Ptr<CLAHE> clahe;
		Mat img_target_equalized, img_toQuery_equalized;

		~Comparator_BuiltInDetector()
		{
			matcher->clear();
			matcher = nullptr;
			detector->clear();
			detector = nullptr;
		}

		Comparator_BuiltInDetector() : detector(DetectorType::create()),
									   matcher(DescriptorMatcher::create(matcherType)),
									   clahe(createCLAHE())
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
				detector->detectAndCompute(img, noArray(), keypoints_target, descriptors_target);
				matcher->add(descriptors_target);

				img_target = img;

				return 1;
			}
			else
			{
				// 이미지에서 특징점과 디스크립터 추출
				detector->detectAndCompute(img, noArray(), keypoints_toQuery, descriptors_toQuery);
				// 디스크립터를 매처에 추가
				matcher->add(descriptors_toQuery);

				// 비교 대상 이미지 설정
				img_toQuery = img;

				// 최근접 이웃 매칭
				std::vector<std::vector<DMatch>> knn_matches;
				matcher->knnMatch(descriptors_target, descriptors_toQuery, knn_matches, 2);

				// 거리 임계값을 기준으로 최근접 이웃 중 좋은 매칭점만 선택
				std::vector<DMatch> chosen_matches;
				for (auto &match : knn_matches)
					if (match[0].distance < Constants::threshold * match[1].distance)
						chosen_matches.push_back(match[0]);

				// 선택된 매칭점이 너무 적으면 실패
				if (chosen_matches.size() <= 4)
					return 0;

				// 매칭점 좌표 추출
				std::vector<Point2f> mappedpoints_target;
				std::vector<Point2f> mappedpoints_toQuery;
				for (auto &match : chosen_matches)
				{
					mappedpoints_target.push_back(keypoints_target[match.queryIdx].pt);
					mappedpoints_toQuery.push_back(keypoints_toQuery[match.trainIdx].pt);
				}

				// 원근 변환 행렬 계산
				Mat H = findHomography(mappedpoints_target, mappedpoints_toQuery, RANSAC, 1.0, noArray());

				// 원근 변환 실패시 실패 처리
				if (H.data == 0)
					return 0;

				// 원근 변환 행렬 역행렬 계산
				Mat M = H.inv();
				M /= M.at<double>(2, 2);

				// 이미지 원근 변환
				warpPerspective(img, img_toQuery_aligned, M, img.size());

				// 마스크 생성
				mask_toQuery = Mat::ones(img.size(), CV_8U);

				// 비교 대상 이미지에 대응되지 않는 부분은 제거하기 위한 마스크 생성
				img_target_masked = img_target.clone();
				for (int row = 0; row < img_toQuery_aligned.rows; row++)
				{
					for (int column = 0; column < img_toQuery_aligned.cols; column++)
					{
						if (img_toQuery_aligned.at<uint8_t>(row, column) == 0)
						{
							img_target_masked.at<uint8_t>(row, column) = 0;
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

		double getQueryConfidenceRate() override
		{
			if (img_toQuery_aligned.empty())
			{
				return -1.0;
			}

			Mat result;

			clahe->apply(img_target, img_target_equalized);
			clahe->apply(img_toQuery_aligned, img_toQuery_equalized);

			matchTemplate(img_target_equalized, img_toQuery_equalized, result, TemplateMatchModes::TM_CCORR_NORMED, mask_toQuery);
			return result.at<float>(0, 0);
		}
	};

	StateCode stateCode = StateCode::NotInitialize;
	double confidence_rate = -1.0;

public:
	std::vector<Comparator_Base *> comparators;

private:
	char *const buffer;

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