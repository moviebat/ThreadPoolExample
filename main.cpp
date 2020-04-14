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

typedef vector<std::future<void>> FUTUREVECTOR;
unsigned short ThreadCount = 5;   //线程池大小
BilliardsRemap m_remap;
cv::Mat desk;
string strDataDir, strCalibPath;

void RemapImage(vector<string> vectorstring)
{
	printf("  hello, remap1 !  %d\n", std::this_thread::get_id());		
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
			cout << "cv::remap exception, error code: " << e.code << ", error: " << e.err << endl;
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
		clock_t start = clock();
		std::threadpool executor{ ThreadCount };
		if (initParams() != 1) {
			cout << "初始化参数失败，请检测路径!" << endl;
			return 0;
		}

		vector<string> arraystring(1000);
		if (!readBmpFiles(arraystring)) {
			cout << "读取图片列表失败，请检查目录!" << endl;
			return 0;
		}

		int ImageCount = arraystring.size();
		cout << "******图片共有张数： " << ImageCount << endl;
		int IndexCount = 0;
		if (ImageCount % ThreadCount != 0) {
			IndexCount = ImageCount / ThreadCount + 1;
		}
		else {
			IndexCount = ImageCount / ThreadCount;
		}

		cout << "IndexCount = " << IndexCount << endl;
		FUTUREVECTOR futurevector(0);
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
			std::future<void> future = executor.commit(RemapImage, tempVector);
			//futurevector.push_back(future);
		}

		//方法二，使用迭代器将容器中数据输出
		//for (int j = 0; j < futurevector.size(); j++)
		//	futurevector[j].get();
		
		//for (vector<string>::iterator it = vectorstring.begin(); it != vectorstring.end(); it++) {
		//	cout << *it << endl;
		//}

		//for (int j = 0; j < vectorstring.size(); j++)
		//	std::cout << vectorstring[j] << " ";
		//std::cout << std::endl;
		clock_t end = clock();
		cout << (end - start) << "ms" << endl;

		std::cout << "end... " << std::this_thread::get_id() << std::endl;
		return 0;
	}
	catch (std::exception& e) {
		std::cout << "Exception...  " << std::this_thread::get_id() << e.what() << std::endl;
	}
}
