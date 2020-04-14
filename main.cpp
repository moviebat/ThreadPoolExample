#include "threadpool.h"
#include <iostream>
#include <windows.h>
#include <dirent.h>
#include <list>
#include<string>
#include<vector>;
#include <ballDetect.h>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

#ifdef _MSC_VER
#define SYMBOL_SLASH '\\'
#else
#define SYMBOL_SLASH '/'
#endif // _MSC_VER

typedef vector<std::future<void>> FUTUREVECTOR;

unsigned short ThreadCount = 5;   //线程池大小

cv::Mat map1, map2;
cv::Mat m_curImg, desk;
cv::Mat intrinsicMatrix = cv::Mat::eye(3, 3, CV_64F);
cv::Mat distCoeffs = cv::Mat::zeros(5, 1, CV_64F);
cv::Mat radical , tangential;
BilliardsDetector bd;

void RemapImage(vector<string> vectorstring)
{
	printf("  hello, remap1 !  %d,size of array string is %d\n", std::this_thread::get_id(), vectorstring.size());		
	//for (vector<string>::iterator it = vectorstring.begin(); it != vectorstring.end(); it++) {
	//	cout << *it << endl;
	//}

	//for (int j = 0; j < vectorstring.size(); j++)
	//	std::cout << vectorstring[j] << " ";
	//std::cout << std::endl;
	for (int index = 0; index < vectorstring.size(); index++)
	{
		cv::Mat m_curImage = cv::imread(vectorstring[index]);
		//cv::imshow("cam 0", m_curImage);
		try
		{
			//cv::remap(m_curImage, m_curImage, map1, map2, cv::INTER_LINEAR);
			remap(m_curImage, m_curImage, bd.map1, bd.map2, cv::INTER_LINEAR);
		}
		catch (const cv::Exception& e)
		{
			cout << "cv::remap exception, error code: " << e.code << ", error: " << e.err << endl;
		}

		char sTempFileName[200];
		sprintf_s(sTempFileName, "%s", vectorstring[index]);
		//DeleteFile(sTempFileName);
		cv::imwrite(vectorstring[index], m_curImage);
		//std::cout << "remap -- " << m_strCurDataDir + m_vecImageNames[index] << std::endl;
		//if (cv::waitKey(delay) >= 0)
		//	break;
	}
}

///////////////////////////此处保留，供其他方式调用线程池使用
struct remap2 {
	int operator()(int n) {
		printf("%d  hello, remap2 !  %d\n", n, std::this_thread::get_id());
		return 42;
	}
};

class Image {    //函数必须是 static 的才能使用线程池
public:
	static int Afun(int n = 0) {
		std::cout << n << "  hello, Afun !  " << std::this_thread::get_id() << std::endl;
		return n;
	}

	static std::string Bfun(int n, std::string str, char c) {
		std::cout << n << "  hello, Bfun !  " << str.c_str() << "  " << (int)c << "  " << std::this_thread::get_id() << std::endl;
		return str;
	}
};
///////////////////////////此处保留，供其他方式调用线程池使用

int initParams(const string& calibFileName, cv::Mat deskMat) {
	cv::FileStorage fs(calibFileName, cv::FileStorage::READ);
	if (!fs.isOpened())
		return -1;
	fs["IntrinsicMatrix"] >> intrinsicMatrix;
	fs["RadialDistortion"] >> radical;
	fs["TangentialDistortion"] >> tangential;

	distCoeffs.at<double>(0, 0) = radical.at<double>(0, 0);
	distCoeffs.at<double>(1, 0) = radical.at<double>(1, 0);
	distCoeffs.at<double>(2, 0) = tangential.at<double>(0, 0);
	distCoeffs.at<double>(3, 0) = tangential.at<double>(1, 0);

	cv::Size imgSize = deskMat.size();
	cv::Mat map1 = cv::Mat(imgSize, CV_32FC1);
	cv::Mat map2 = cv::Mat(imgSize, CV_32FC1);

	initUndistortRectifyMap(intrinsicMatrix, distCoeffs, cv::Mat(),
		getOptimalNewCameraMatrix(intrinsicMatrix, distCoeffs, imgSize, 1, imgSize, 0),
		imgSize, CV_16SC2, map1, map2);
	
	fs.release();
	return 1;
}

//从给定的目录中遍历所有的BMP图片，返回vector<string>
bool readBmpFiles(string strDataDir, vector<string>&imageVector)
{
	dirent* p_file = NULL;
	const char* input_path = strDataDir.c_str();
	DIR* input_dir = opendir(input_path);
	char camPath[200] = "";
	p_file = readdir(input_dir);
	imageVector.clear();    //清空
	do
	{
		// in case . or ..
		if (p_file != NULL && p_file->d_name[0] == '.')
		{
			p_file = readdir(input_dir);
		}
		if (p_file != NULL && p_file->d_name[0] == '.')
		{
			p_file = readdir(input_dir);
		}
		if (p_file != NULL) {
			strcpy_s(camPath, input_path);
			strcat_s(camPath, p_file->d_name);
			//cout << camPath << endl;
			imageVector.push_back(camPath);
			p_file = readdir(input_dir);
		}
		else {
			break;
		}		
	} while (1);
	cout << imageVector.size() << endl;
	return true;
}


int main() {
	

	try {
		std::threadpool executor{ ThreadCount };

		std::string strWorkDir = "D:\\vs2015projects\\ThreadPoolExample\\bin\\";
		std::string strDataDir = "D:\\vs2015projects\\ThreadPoolExample\\images\\";
		std::string strCalibPath = 
			"D:\\vs2015projects\\ThreadPoolExample\\bin\\calib\\calib_yunchuan_zhongguancun_24\\";
		std::string calibFileName = strCalibPath + "camera_params.xml";
		desk = cv::imread(strCalibPath + "table" + SYMBOL_SLASH + "1" + SYMBOL_SLASH + "frame_00000001.bmp");

		std::string strModelPath = "D:\\vs2015projects\\ballDetect_dll\\models\\";

		bd.initParams(strCalibPath + "camera_params.xml", strModelPath + "train.model", desk);
		//if (initParams(calibFileName, desk) != 1) {
		//	cout << "初始化参数失败，请检测路径!" << endl;
		//	return 0;
		//}
		bd.initUnDistort(desk);

		vector<string> arraystring(1000);
		if (!readBmpFiles(strDataDir, arraystring)) {
			cout << "读取图片列表失败，请检查目录!" << endl;
			return 0;
		}


		int ImageCount = arraystring.size();
		cout << "******图片共有%d张 " << ImageCount << endl;
		int IndexCount = 0;
		if (ImageCount % ThreadCount != 0) {
			IndexCount = ImageCount / ThreadCount + 1;
		}
		else {
			IndexCount = ImageCount / ThreadCount;
		}

		cout << "IndexCount = %d " << IndexCount << endl;
		FUTUREVECTOR futurevector(ThreadCount);
		for (int i = 0; i < ThreadCount; i++)
		{
			std::vector<string> tempVector(IndexCount);
			if (i < ThreadCount - 1) {
				tempVector.insert(tempVector.begin(), arraystring.begin() + IndexCount * i, arraystring.begin() + IndexCount * (i + 1));
			}
			else  //最后一个要兜底，可能没有那么多图片，计算要调整 
			{
				tempVector.insert(tempVector.begin(), arraystring.begin() + IndexCount * i, arraystring.end());
			}			
			std::cout << "vec " << i << ": ";
			//for (int j = 0; j < tempVector.size(); j++)
			//	std::cout << tempVector[j] << " ";
			//std::cout << std::endl;
			std::future<void> future = executor.commit(RemapImage, tempVector);
			//futurevector.push_back(future);
		}

		std::cout << " =======  sleep ========= " << std::this_thread::get_id() << std::endl;
		std::this_thread::sleep_for(std::chrono::microseconds(900));

		std::cout << " =======  commit all ========= " << std::this_thread::get_id() << " idlsize=" << executor.idlCount() << std::endl;

		std::cout << " =======  sleep ========= " << std::this_thread::get_id() << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(3));

		//方法二，使用迭代器将容器中数据输出
		for (int j = 0; j < futurevector.size(); j++)
			futurevector[j].get();

		std::cout << "end... " << std::this_thread::get_id() << std::endl;

		return 0;
	}
	catch (std::exception& e) {
		std::cout << "Exception...  " << std::this_thread::get_id() << e.what() << std::endl;
	}
}
