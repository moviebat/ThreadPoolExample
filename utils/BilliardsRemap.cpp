#include "BilliardsRemap.h"



BilliardsRemap::BilliardsRemap()
{
}


BilliardsRemap::~BilliardsRemap()
{
}

int BilliardsRemap::initParams(const std::string& cameraParamsPath)
{
	cv::FileStorage fsread(cameraParamsPath, cv::FileStorage::READ);
	fsread["IntrinsicMatrix"] >> m_intrinsicMatrix;

	cv::Mat radialDistortion;
	fsread["RadialDistortion"] >> radialDistortion;

	m_distCoeffs.at<double>(0, 0) = radialDistortion.at<double>(0, 0);
	m_distCoeffs.at<double>(1, 0) = radialDistortion.at<double>(1, 0);

	fsread.release();

	return 1;
}

void BilliardsRemap::initUnDistort(std::string strImg)
{
	cv::Mat img = cv::imread(strImg);
	cv::Size imgSize = img.size();
	m_map1 = cv::Mat(imgSize, CV_32FC1);
	m_map2 = cv::Mat(imgSize, CV_32FC1);
	cv::initUndistortRectifyMap(m_intrinsicMatrix, m_distCoeffs, cv::Mat(),
		cv::getOptimalNewCameraMatrix(m_intrinsicMatrix, m_distCoeffs, imgSize, 1, imgSize, 0),
		imgSize, CV_16SC2, m_map1, m_map2);
}


void BilliardsRemap::remap(cv::Mat img)
{
	try
	{
		cv::remap(img, img, m_map1, m_map2, cv::INTER_LINEAR);
	}
	catch (const cv::Exception& e)
	{
		std::cout << "cv::remap exception, error code: " << e.code << ", error: " << e.err << std::endl;
	}
}
