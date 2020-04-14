#pragma once
#include "I_BilliardsComDef.h"
#include "svm.h"
#include <opencv2/highgui/highgui.hpp>

using namespace std;

struct pocketnode
{
	int x;
	int y;
	bool isBallPixel;
	struct pocketnode *before;
	struct pocketnode *next;
};

struct masknode
{
	int x;
	int y;
	struct masknode *before;
	struct masknode *next;
};

#ifdef _MSC_VER
#ifdef BALLDETECT_DLL_EXPORTS
#define EXPORT_API _declspec(dllexport)
#else
#define EXPORT_API __declspec(dllimport)
#endif // BALL_DETECT_EXPORTS
#else
#define EXPORT_API
#endif // _MSC_VER

class EXPORT_API BilliardsDetector
//class BilliardsDetector
{
public:
	 BilliardsDetector();
	 ~BilliardsDetector();
	 int initParams(const string& cameraParamsPath, const string& modelPath, cv::Mat desk);
	 vector<Ball> detect(cv::Mat prevFrame, cv::Mat currFrame, vector<Ball> lastResult);
	 vector<Ball> transCoordinate(vector<Ball> result);
	 void initUnDistort(cv::Mat img);
	 int setMask(cv::Mat img);
	 int testPocket(cv::Mat img, const string& imgPathToWrite);
	 int createPocketMask(bool noExistingMask, const string& pocketMaskPath);
	 bool testDeskMoved(cv::Mat img);
	 void setStatic(int thresh, int h, int s, int v);
	 void setFlag(int a, int b, int c);
	 void setMaskThresh(int h, int s, int v);
	 void segBall(cv::Mat img, int x, int y, const string& path);

private:
	void generate_uniform_lbp_cell(cv::Mat pic, float lbp[], int table[]);
	int getHopCount(uchar num);
	pocketnode * findPocketArea(int k, pocketnode *top);
	void findEdgeBallArea(int x, int y, cv::Mat &filledMap, int &count, int &minX, int &maxX, int &minY, int &maxY);
	masknode * findMask(masknode *mtop);
	void svmScale(float lbp[]);

public:
	svm_model * bm;
	cv::Mat map1;
	cv::Mat map2;

	cv::Mat intrinsicMatrix = cv::Mat::eye(3, 3, CV_64F);
	cv::Mat distCoeffs = cv::Mat::zeros(5, 1, CV_64F);
	cv::Mat range = cv::Mat(3, 2, CV_32S);
	cv::Mat center_range = cv::Mat(1, 5, CV_8U);

	cv::Mat K_inv = cv::Mat::eye(3, 3, CV_32F);
	cv::Mat P_inv = cv::Mat(3, 3, CV_32F);

	cv::Mat centerPoint = cv::Mat(3, 1, CV_32F);
	cv::Mat rightPoint = cv::Mat(3, 1, CV_32F);

	cv::Mat pocketTopLeft = cv::Mat(2, 1, CV_32F);
	cv::Mat pocketTopMiddle = cv::Mat(2, 1, CV_32F);
	cv::Mat pocketTopRight = cv::Mat(2, 1, CV_32F);
	cv::Mat pocketBottomLeft = cv::Mat(2, 1, CV_32F);
	cv::Mat pocketBottomMiddle = cv::Mat(2, 1, CV_32F);
	cv::Mat pocketBottomRight = cv::Mat(2, 1, CV_32F);

	cv::Mat pocketCenterTopLeft = cv::Mat(2, 1, CV_32F);
	cv::Mat pocketCenterTopMiddle = cv::Mat(2, 1, CV_32F);
	cv::Mat pocketCenterTopRight = cv::Mat(2, 1, CV_32F);
	cv::Mat pocketCenterBottomLeft = cv::Mat(2, 1, CV_32F);
	cv::Mat pocketCenterBottomMiddle = cv::Mat(2, 1, CV_32F);
	cv::Mat pocketCenterBottomRight = cv::Mat(2, 1, CV_32F);

	int deskleft;
	int deskright;
	int desktop;
	int deskbottom;

	cv::Mat deskTopLeft = cv::Mat(2, 1, CV_32F);
	cv::Mat deskTopRight = cv::Mat(2, 1, CV_32F);
	cv::Mat deskBottomLeft = cv::Mat(2, 1, CV_32F);
	cv::Mat deskBottomRight = cv::Mat(2, 1, CV_32F);

	float kTop = 0, kLeft = 0, kRight = 0, kBottom = 0;
	cv::Mat mask = cv::Mat(600, 960, CV_8U);
	cv::Mat mask0 = cv::Mat(600, 960, CV_8U);
	cv::Mat result = cv::Mat(600, 960, CV_8U);
	cv::Mat tmp = cv::Mat(600, 960, CV_8U);
	cv::Mat desk;
	cv::Mat hsvImg;
	cv::Mat pocketMask;
	cv::Mat deskVmin = cv::Mat(600, 960, CV_8U);

	int rec_width = 24;
	int rec_length = 24;

	int ytop = 45;
	int ybottom = 550;
	int xleft = 10;
	int xright = 950;

	int table[256];
	int pocketCount[6];
	int ball_color[9][6];
	int desk_color[6];
	int center_region[5];

	int pocket_radius;
	int ball_radius;

	double angle;

	int staticThresh = 15;
	int staticH = 10;
	int staticS = 30;
	int staticV = 20;

	int maskThreshH = 10;
	int maskThreshS = 30;
	int maskThreshV = 30;

	int flag = 1;
	int cflag = 1;
	int testflag = 1;
	int recflag = 1;

	int cenPktTop = 20;
	int cenPktBot = 570;
};
