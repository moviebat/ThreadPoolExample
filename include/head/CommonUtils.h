#ifndef __COMMON_UTILS_H__
#define __COMMON_UTILS_H__

#ifdef _MSC_VER
#pragma once
#endif // _MSC_VER

#include <string>
#include <vector>

// ------------------------------------------------------
// CommonUtils����̬�������ߺ�����
// 
// ��Ӻ���˵����
// 1. ��ʹ��KPZ�ڲ������ݽṹ����������KPZ����ͷ�ļ�
// 2. �������Ǿ�̬��ʽ��ʹ��CommonUtils::�������� ���е���
// 3. ���ھ����ظ���ʹ�õĹ��ߺ���
// ------------------------------------------------------

class CommonUtils
{
public:
	CommonUtils();
	~CommonUtils();

public:
	// ��ȡ��ǰ��ִ�г������ڵ�·��
	static std::string getCurExeFilePath();

	// ��ȡ��ǰӦ������
	static std::string getCurExeFileName();

	// ���Ŀ¼�Ƿ����
	static bool isDirExist(std::string strDirPath);

	// ����ļ��Ƿ����
	static bool isFileExist(std::string strDirPath);

	// ����Ŀ¼
	static bool createDir(const char* szDirPath);

	// ��ȡĿ¼�µ��ļ��б�
	static void getFileList(const std::string strDirPath, std::vector<std::string>& files);

	// ��ȡ��ǰ��ʽ��ʱ��
	// ������
	// ��1��%Y/%m/%d %H:%M:%S����Ӧ��2019/03/02 10:29:34
	// ��2��%m.%d_%H%M%S����Ӧ��03.12_140245
	static std::string getCurTimeFormat(std::string strFormat);

	// ��ȡ��ǰʱ�䣨��ʽ��20191108-124827��
	static std::string getCurTime();

	// ��ȡ��ǰ���ڣ���ʽ��2018_06_11��
	static std::string getCurDate();

	// ��ȡǰN������ڣ���ʽ��20180611��
	static std::string getDate(int daysBefore);

	// ��ȡUnixʱ�������1573642010���뼶��
	static std::string getUnixTimestamp();

	// ���app�Ƿ��Ѿ�����
	static bool isCurAppRunning();

	// �ַ����ָ�
	static void split(const std::string& str, const std::string& pattern, std::vector<std::string>& vecRet);

	// ����GUID�������ַ�������ʽ��{434E93EA-3654-4C2F-821E-B6DFF968FFFF}
	// Windows���ô�д��Linux����Сд
	static std::string createGuid();

	// ��ȡ����IP��ַ
	static std::string getIpAddress();
};

#endif // __COMMON_UTILS_H__