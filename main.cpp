#include "threadpool.h"
#include <iostream>
#include <windows.h>
#include <dirent.h>
#include <list>
#include<string>
#include<vector>;
#include <opencv2/opencv.hpp>
#include "BilliardsRemap.h"
#include "CommonUtils.h"
#include "CommonDefine.h"
#include "IniParser.h"

using namespace std;
using namespace cv;

unsigned short ThreadCount = 5;   //线程池大小
BilliardsRemap m_remap;
cv::Mat desk;
string strDataDir, strCalibPath;

void RemapImage(vector<string> vectorstring)
{	
	std::cout << "进入线程执行..." << std::this_thread::get_id() << endl;
	for (int index = 0; index < vectorstring.size(); index++)
	{
		cv::Mat m_curImage = cv::imread(vectorstring[index]);
		try
		{
			m_remap.remap(m_curImage);
			cv::imwrite(vectorstring[index], m_curImage);
		}
		catch (const cv::Exception& e)
		{
			std::cout << "cv::remap exception, error code: " << e.code << ", error: " << e.err << endl;
		}		
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

bool initParams() {
	std::cout << "正在初始化参数!" << endl;
	//从config配置文件中读取相关参数
	std::string strCfgFile = CommonUtils::getCurExeFilePath() + "config" + DEFAULT_SYMBOL_SLASH + "billCamera.cfg";
	if (!CommonUtils::isFileExist(strCfgFile))
	{
		std::cout << "Cannot find config/billiCamera.cfg.pls check." << endl;
		return false;
	}

	IniParser parser(strCfgFile);
	int error = parser.GetErrorCode();
	if (error < 0)
		return false;

	strCalibPath = parser.GetIniKeyString("camera", "calib_data_path",
		"D:\\vs2015projects\\ThreadPoolExample\\bin\\calib\\calib_yunchuan_zhongguancun_24\\");

	strDataDir = parser.GetIniKeyString("image", "data_dir",
		 "D:\\vs2015projects\\ThreadPoolExample\\images\\");

	std::string calibFileName = strCalibPath + "camera_params.xml";
	std::string strTablePath = strCalibPath + "table" + DEFAULT_SYMBOL_SLASH + "1" + DEFAULT_SYMBOL_SLASH + "frame_00000001.bmp";

	m_remap.initParams(calibFileName);
	m_remap.initUnDistort(strTablePath);
	//desk = cv::imread(strTablePath);
	return true;
}

//从给定的目录中遍历所有的BMP图片，返回vector<string>
bool readBmpFiles(vector<string>&imageVector)
{
	std::cout << "正在读取所有图片!" << endl;
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
	//cout << imageVector.size() << endl;
	return true;
}


int main() {
	
	try {
		clock_t start = clock();
		std::threadpool executor{ ThreadCount };

		if (initParams() != 1) {
			std::cout << "初始化参数失败，请检测路径!" << endl;
			return 0;
		}

		vector<string> arraystring(1000);
		if (!readBmpFiles(arraystring)) {
			std::cout << "读取图片列表失败，请检查目录!" << endl;
			return 0;
		}

		int ImageCount = arraystring.size();
		std::cout << "图片共有" << ImageCount << "张。" << endl;
		int IndexCount = 0;
		if (ImageCount % ThreadCount != 0) {
			IndexCount = ImageCount / ThreadCount + 1;
		}
		else {
			IndexCount = ImageCount / ThreadCount;
		}

		std::cout << "每个线程池大约" << IndexCount << "张。" << endl;
		std::vector< std::future<void> > results;
		for (int i = 0; i < ThreadCount; i++)
		{
			std::vector<string> tempVector(0);
			if (i < ThreadCount - 1) {
				tempVector.insert(tempVector.begin(), arraystring.begin() + IndexCount * i, arraystring.begin() + IndexCount * (i + 1));
			}
			else  //最后一个要兜底，可能没有那么多图片，计算要调整 
			{
				tempVector.insert(tempVector.begin(), arraystring.begin() + IndexCount * i, arraystring.end());
			}
			std::cout << "vec " << i << ": " << tempVector.size() << endl;
			//for (int j = 0; j < tempVector.size(); j++)
			//	std::cout << tempVector[j] << " ";
			//std::cout << std::endl;
			results.emplace_back(executor.commit(RemapImage, tempVector));
		}
		std::cout << "所有线程已经全部提交完毕。" << std::this_thread::get_id() << std::endl;
		
		//for (auto && result : results)
		//	std::cout << result.get() << ' ';
		//std::cout << std::endl;
		//for (vector<string>::iterator it = vectorstring.begin(); it != vectorstring.end(); it++) {
		//	cout << *it << endl;
		//}

		for (int j = 0; j < results.size(); j++)
			results[j].get();
		std::cout << std::endl;
		
		clock_t end = clock();
		std::cout << "成功结束，共消耗时间：" << (end - start) << "ms" << endl;
		
		return 0;
	}
	catch (std::exception& e) {
		std::cout << "Exception...  " << std::this_thread::get_id() << e.what() << std::endl;
	}
}
