#ifndef __COMMON_UTILS_H__
#define __COMMON_UTILS_H__

#ifdef _MSC_VER
#pragma once
#endif // _MSC_VER

#include <string>
#include <vector>

// ------------------------------------------------------
// CommonUtils：静态公共工具函数类
// 
// 添加函数说明：
// 1. 不使用KPZ内部的数据结构，即不依赖KPZ其他头文件
// 2. 函数都是静态形式，使用CommonUtils::函数名称 进行调用
// 3. 属于经常重复性使用的工具函数
// ------------------------------------------------------

class CommonUtils
{
public:
	CommonUtils();
	~CommonUtils();

public:
	// 获取当前可执行程序所在的路径
	static std::string getCurExeFilePath();

	// 获取当前应用名称
	static std::string getCurExeFileName();

	// 检查目录是否存在
	static bool isDirExist(std::string strDirPath);

	// 检查文件是否存在
	static bool isFileExist(std::string strDirPath);

	// 创建目录
	static bool createDir(const char* szDirPath);

	// 获取目录下的文件列表
	static void getFileList(const std::string strDirPath, std::vector<std::string>& files);

	// 获取当前格式化时间
	// 举例：
	// （1）%Y/%m/%d %H:%M:%S，对应：2019/03/02 10:29:34
	// （2）%m.%d_%H%M%S，对应：03.12_140245
	static std::string getCurTimeFormat(std::string strFormat);

	// 获取当前时间（格式：20191108-124827）
	static std::string getCurTime();

	// 获取当前日期（格式：2018_06_11）
	static std::string getCurDate();

	// 获取前N天的日期（格式：20180611）
	static std::string getDate(int daysBefore);

	// 获取Unix时间戳，如1573642010，秒级别
	static std::string getUnixTimestamp();

	// 检查app是否已经运行
	static bool isCurAppRunning();

	// 字符串分割
	static void split(const std::string& str, const std::string& pattern, std::vector<std::string>& vecRet);

	// 创建GUID，返回字符串，格式：{434E93EA-3654-4C2F-821E-B6DFF968FFFF}
	// Windows采用大写，Linux采用小写
	static std::string createGuid();

	// 获取本机IP地址
	static std::string getIpAddress();
};

#endif // __COMMON_UTILS_H__