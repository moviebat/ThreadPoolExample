#include "CommonUtils.h"
#include "CommonDefine.h"
#include <string>

#ifdef _MSC_VER
// get networkinfo
#include <winsock2.h> // include must before window.h
#include <iphlpapi.h>

#include <windows.h>
#include "dirent.h"
#include <direct.h> 
#include <io.h>

#pragma comment(lib, "iphlpapi.lib")
#pragma warning (disable: 4996)
#define mkdir _mkdir
#else
#include <limits.h>  
#include <stdio.h>  
#include <stdlib.h>
#include <string.h>  
#include <unistd.h> 
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

// get networkinfo
#include <ifaddrs.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#endif

#include <fstream>
#include <chrono>
#include <ctime>
#include <iostream>

#ifndef _MSC_VER

#define MAX_PATH 260

#endif

using namespace std;

CommonUtils::CommonUtils()
{
}


CommonUtils::~CommonUtils()
{
}

std::string CommonUtils::getCurExeFilePath()
{
	std::string strPath = "";

#ifdef _MSC_VER
	wchar_t exeFullPath[BUFSIZ] = { '\0' }; // Full path  
	GetModuleFileName(NULL, exeFullPath, BUFSIZ);

	char szString[BUFSIZ] = { '\0' };
	size_t convertedChars = 0;
	wcstombs_s(&convertedChars, szString, BUFSIZ, exeFullPath, _TRUNCATE);

	strPath = std::string(szString);    // Get full path of the file  
#else
	char exeFullPath[BUFSIZ] = { '\0' }; // Full path  
	readlink("/proc/self/exe", exeFullPath, BUFSIZ);

	//char szString[BUFSIZ];
	//size_t convertedChars = 0;
	//wcstombs(szString, exeFullPath, BUFSIZ);

	strPath = std::string(exeFullPath);    // Get full path of the file  
#endif // _MSC_VER

	int pos = (int)strPath.find_last_of(DEFAULT_SYMBOL_SLASH, strPath.length()) + 1;
	return strPath.substr(0, pos);  // Return the directory without the file name (with "\\" or "/")
}

std::string CommonUtils::getCurExeFileName()
{
	std::string strPath = "";

#ifdef _MSC_VER
	wchar_t exeFullPath[BUFSIZ] = { '\0' }; // Full path  
	GetModuleFileName(NULL, exeFullPath, BUFSIZ);

	char szString[BUFSIZ] = { '\0' };
	size_t convertedChars = 0;
	wcstombs_s(&convertedChars, szString, BUFSIZ, exeFullPath, _TRUNCATE);

	strPath = std::string(szString);    // Get full path of the file  
#else
	char exeFullPath[BUFSIZ] = { '\0' }; // Full path  
	readlink("/proc/self/exe", exeFullPath, BUFSIZ);

	//char szString[BUFSIZ];
	//size_t convertedChars = 0;
	//wcstombs(szString, exeFullPath, BUFSIZ);

	strPath = std::string(exeFullPath);    // Get full path of the file  
#endif // _MSC_VER

	int pos = (int)strPath.find_last_of(DEFAULT_SYMBOL_SLASH, strPath.length()) + 1;
	int posEnd = (int)strPath.find_last_of(".", strPath.length());
	if (posEnd != -1)
		return strPath.substr(pos, posEnd);
	else
		return strPath.substr(pos, strPath.length());
}

bool CommonUtils::isDirExist(std::string strDirPath)
{
#ifdef _MSC_VER
	bool bFind = false;
	intptr_t hFile = 0;
	struct _finddata_t fileinfo;
	std::string p;

	if ((hFile = _findfirst(p.assign(strDirPath).append("\\*").c_str(), &fileinfo)) != -1)
	{
		_findclose(hFile);
		bFind = true;
	}
	return bFind;
#else
	DIR *dir = opendir(strDirPath.c_str());
	if (dir == NULL)
		return false;

	closedir(dir);
	dir = NULL;
	return true;
#endif
}

bool CommonUtils::isFileExist(std::string strDirPath)
{
	fstream _file;
	_file.open(strDirPath, ios::in);
	if (!_file)
	{
		return false;
	}
	else
	{
		_file.close();
		return true;
	}
}

bool CommonUtils::createDir(const char* szDirPath)
{
	char path[MAX_PATH] = { '\0' };
	const char* pos = szDirPath;
	while ((pos = strchr(pos, DEFAULT_SYMBOL_SLASH)) != NULL)
	{
		memcpy(path, szDirPath, pos - szDirPath + 1);
		pos++;
		if (isDirExist(path))
		{
			continue;
		}
		else
		{
#ifdef _MSC_VER
			int ret = mkdir(path);
#else
			int ret = mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif // _MSC_VER
			if (ret == -1)
			{
				std::cout << "mkdir failed" << std::endl;
				return false;
			}
		}
	}
	pos = szDirPath + strlen(szDirPath) - 1;
	if (*pos != DEFAULT_SYMBOL_SLASH)
	{
#ifdef _MSC_VER
		return mkdir(szDirPath) == 0;
#else
		return mkdir(szDirPath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0;
#endif // _MSC_VER
	}
	return true;
}

void CommonUtils::getFileList(const std::string strDirPath, std::vector<std::string>& files)
{
	if (strDirPath.empty())
		return;

#ifdef _MSC_VER
	struct __finddata64_t fileinfo;
	std::string p;
	__int64 hFile = _findfirst64(p.assign(strDirPath).append("\\*").c_str(), &fileinfo);
	if (hFile == -1)
		return;

	do
	{
		if (fileinfo.attrib & _A_SUBDIR)
			continue;

		files.push_back(p.assign(strDirPath).append("\\").append(fileinfo.name));

	} while (0 == _findnext64(hFile, &fileinfo));
	_findclose(hFile);

#else 

	if (-1 == access(strDirPath.c_str(), 0))
		return;

	struct dirent **dirNames = NULL;
	int fileCount = scandir(strDirPath.c_str(), &dirNames, 0, alphasort);
	if (fileCount >= 3)
	{
		files.clear();
		for (int index = 0; index < fileCount; index++)
		{
			if ((strcmp(dirNames[index]->d_name, ".") == 0) || (strcmp(dirNames[index]->d_name, "..") == 0) || dirNames[index]->d_type != DT_REG)
				continue;

			std::string strFilePath = strDirPath + DEFAULT_SYMBOL_SLASH + dirNames[index]->d_name;
			files.push_back(strFilePath);
		}
	}

	for (int index = fileCount-1; index >= 0; index--)
	{
		free(dirNames[index]);
		dirNames[index] = NULL;
	}

	free(dirNames);
	dirNames = NULL;
#endif
}

std::string CommonUtils::getCurTimeFormat(std::string strFormat)
{
	auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	std::tm* now = std::localtime(&t);

	char tmp[128] = { '\0' };
	strftime(tmp, sizeof(tmp), strFormat.c_str(), now);

	std::string strTime(tmp);
	return strTime;
}

std::string CommonUtils::getCurTime()
{
	auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	std::tm* now = std::localtime(&t);

	char tmp[64] = { '\0' };
	sprintf(tmp, "%4d%02d%02d-%02d%02d%02d", now->tm_year + 1900, now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);

	std::string strTime(tmp);
	return strTime;
}

std::string CommonUtils::getCurDate()
{
	auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	std::tm* now = std::localtime(&t);

	char tmp[64] = { '\0' };
	sprintf(tmp, "%4d_%02d_%02d", now->tm_year + 1900, now->tm_mon + 1, now->tm_mday);

	std::string strDate(tmp);
	return strDate;
}

std::string CommonUtils::getDate(int daysBefore)
{
	auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) - 24 * 60 * 60 * daysBefore;
	std::tm* before = std::localtime(&t);

	char tmp[64] = { '\0' };
	sprintf(tmp, "%4d%02d%02d", before->tm_year + 1900, before->tm_mon + 1, before->tm_mday);

	std::string strDate(tmp);
	return strDate;
}

std::string CommonUtils::getUnixTimestamp()
{
	long timestamp = (long)time(0);
	char tmp[64] = { '\0' };
	sprintf(tmp, "%ld", timestamp);
	std::string strTimestamp(tmp);
	return strTimestamp;
}

bool CommonUtils::isCurAppRunning()
{
#ifdef _MSC_VER
	bool bFound = false;

	// Try to create a mutex with the app's name
	std::string strAppName = getCurExeFileName();
	HANDLE hMutexOneInstance = ::CreateMutexA(NULL, TRUE, strAppName.c_str());

	// Already there...means that we are already running an instance
	if (::GetLastError() == ERROR_ALREADY_EXISTS)
	{
		bFound = TRUE;
	}

	// Release the mutex
	if (hMutexOneInstance != NULL)
	{
		::ReleaseMutex(hMutexOneInstance);
	}

	return(bFound);
#else
	// TODO: Linux下版本实现
	return false;
#endif // _MSC_VER
}

void CommonUtils::split(const std::string& str, const std::string& pattern, std::vector<std::string>& vecRet)
{
	size_t last = 0;
	size_t index = str.find_first_of(pattern, last);
	while (index != std::string::npos)
	{
		vecRet.push_back(str.substr(last, index - last));
		last = index + 1;
		index = str.find_first_of(pattern, last);
	}
	if (index - last > 0)
	{
		vecRet.push_back(str.substr(last, index - last));
	}
}

std::string CommonUtils::createGuid()
{
	char buf[64] = { '\0' };

#ifdef _MSC_VER
	CoInitialize(NULL);
	GUID guid;
	if (S_OK == ::CoCreateGuid(&guid))
	{
		_snprintf(buf, sizeof(buf)
			, "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}"
			, guid.Data1
			, guid.Data2
			, guid.Data3
			, guid.Data4[0], guid.Data4[1]
			, guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5]
			, guid.Data4[6], guid.Data4[7]
		);
	}
	CoUninitialize();
#else
	const char *c = "89ab";
	char *p = buf;
	int n;
	for (n = 0; n < 16; ++n)
	{
		int b = rand() % 255;
		switch (n)
		{
		case 6:
			sprintf(p, "4%x", b % 15);
			break;
		case 8:
			sprintf(p, "%c%x", c[rand() % strlen(c)], b % 15);
			break;
		default:
			sprintf(p, "%02x", b);
			break;
		}

		p += 2;
		switch (n)
		{
		case 3:
		case 5:
		case 7:
		case 9:
			*p++ = '-';
			break;
		}
	}
	*p = 0;
#endif // _MSC_VER 

	return std::string(buf);
}

std::string CommonUtils::getIpAddress()
{
	std::string strIpAddress = "127.0.0.1";

#ifdef _MSC_VER
	// PIP_ADAPTER_INFO struct contains network information
	PIP_ADAPTER_INFO pIpAdapterInfo = new IP_ADAPTER_INFO();
	unsigned long adapter_size = sizeof(IP_ADAPTER_INFO);
	int ret = GetAdaptersInfo(pIpAdapterInfo, &adapter_size);

	if (ret == ERROR_BUFFER_OVERFLOW)
	{
		// overflow, use the output size to recreate the handler
		delete pIpAdapterInfo;
		pIpAdapterInfo = (PIP_ADAPTER_INFO)new BYTE[adapter_size];
		ret = GetAdaptersInfo(pIpAdapterInfo, &adapter_size);
	}
	
	if (ret == ERROR_SUCCESS)
	{
		//int card_index = 0;

		// may have many cards, it saved in linklist
		while (pIpAdapterInfo)
		{
			//std::cout << "---- " << "NetworkCard " << ++card_index << " ----" << std::endl;

			//std::cout << "Network Card Name: " << pIpAdapterInfo->AdapterName << std::endl;
			//std::cout << "Network Card Description: " << pIpAdapterInfo->Description << std::endl;

			// 保留无线网卡和有线网卡，然后根据ip不是0.0.0.0进行区分
			std::string strDesc = pIpAdapterInfo->Description;
			if (-1 == strDesc.find("Wireless") && -1 == strDesc.find("Realtek"))
			{
				pIpAdapterInfo = pIpAdapterInfo->Next;
				continue;
			}

			// get IP, one card may have many IPs
			PIP_ADDR_STRING pIpAddr = &(pIpAdapterInfo->IpAddressList);
			while (pIpAddr)
			{
				char local_ip[128] = { 0 };
				strcpy(local_ip, pIpAddr->IpAddress.String);
				
				std::string strIp = local_ip;
				if (strIp != "0.0.0.0")
					strIpAddress = local_ip;

				pIpAddr = pIpAddr->Next;
			}

			// local Mac
			//char local_mac[128] = { 0 };
			//int char_index = 0;
			//for (int i = 0; i < pIpAdapterInfo->AddressLength; i++)
			//{
			//	char temp_str[10] = { 0 };
			//	sprintf(temp_str, "%02X-", pIpAdapterInfo->Address[i]); // X for uppercase, x for lowercase
			//	strcpy(local_mac + char_index, temp_str);
			//	char_index += 3;
			//}
			//local_mac[17] = '\0'; // remove tail '-'

			//std::cout << "Local Mac: " << local_mac << std::endl;

			// here just need the first card info
			//break;
			// iterate next
			pIpAdapterInfo = pIpAdapterInfo->Next;
		}
	}

	if (pIpAdapterInfo)
		delete pIpAdapterInfo;

#else

	// get ip, works for linux and mac os, best method
	struct ifaddrs *ifAddrStruct = NULL;
	struct ifaddrs *ifa = NULL;

	getifaddrs(&ifAddrStruct);

	for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next)
	{
		if (!ifa->ifa_addr)
			continue;

		// check if IP4
		if (ifa->ifa_addr->sa_family == AF_INET)
		{
			void *tmpAddrPtr = &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
			char local_ip[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, tmpAddrPtr, local_ip, INET_ADDRSTRLEN);

			// actually only need external ip
			printf("%s IP: %s\n", ifa->ifa_name, local_ip);

			if (0 == strcmp(ifa->ifa_name, "enp2s0"))
			{
				strIpAddress = local_ip;
				break;
			}
			else if (0 == strcmp(ifa->ifa_name, "eth0"))
			{
				strIpAddress = local_ip;
			}
		}
	}

	if (ifAddrStruct)
		freeifaddrs(ifAddrStruct);


	//// get mac, need to create socket first, may not work for mac os
	//struct ifreq ifr;
	//int fd = socket(AF_INET, SOCK_DGRAM, 0);

	//char local_mac[128] = { 0 };

	//strcpy(ifr.ifr_name, "eth0"); // only need ethernet card
	//if (0 == ioctl(fd, SIOCGIFHWADDR, &ifr))
	//{
	//	char temp_str[10] = { 0 };
	//	memcpy(temp_str, ifr.ifr_hwaddr.sa_data, 6);
	//	sprintf(local_mac, "%02x-%02x-%02x-%02x-%02x-%02x", temp_str[0] & 0xff, temp_str[1] & 0xff, temp_str[2] & 0xff, temp_str[3] & 0xff, temp_str[4] & 0xff, temp_str[5] & 0xff);
	//}

	//printf("Local Mac: %s\n", local_mac);

#endif //_MSC_VER

	std::cout << "Local IP: " << strIpAddress << std::endl;
	return strIpAddress;
}
