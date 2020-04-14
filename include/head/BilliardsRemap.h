#ifndef __BILLIARDS_REMAP_H__
#define __BILLIARDS_REMAP_H__

#ifdef _MSC_VER
#pragma once
#endif // _MSC_VER

#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

class BilliardsRemap
{
public:
	BilliardsRemap();
	~BilliardsRemap();

	int initParams(const std::string& cameraParamsPath);
	void initUnDistort(std::string strImg);
	void remap(cv::Mat img);

private:
	cv::Mat m_intrinsicMatrix = cv::Mat::eye(3, 3, CV_64F);
	cv::Mat m_distCoeffs = cv::Mat::zeros(5, 1, CV_64F);
	cv::Mat m_map1;
	cv::Mat m_map2;
};

#endif // __BILLIARDS_REMAP_H__