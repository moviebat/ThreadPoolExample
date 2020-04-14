#ifndef __INI_PARSER_H__
#define __INI_PARSER_H__

#ifdef _MSC_VER
#pragma once
#pragma warning (disable: 4996)
#endif // _MSC_VER

#include <stdio.h>  
#include <string.h>  
#include <stdlib.h>
#include <iostream>
#include <vector>

using namespace std;

// ------------------------------------------------------
// IniParser£ºiniÎÄ¼þ¶ÁÐ´Àà
// ------------------------------------------------------

class IniParser
{
public:
	IniParser(std::string filePath);
	~IniParser();

public:
	string GetIniKeyString(const char *title, const char *key, const char *nullVal); 
	int GetIniKeyInt(const char *title, const char *key, int nullVal);
	float GetIniKeyFloat(const char *title, const char *key, float nullVal);

	void PutIniKeyString(const char *title, const char *key, char *val);
	void PutIniKeyInt(const char *title, const char *key, int val);
	void PutIniKeyFloat(const char *title, const char *key, float val);

	void SaveIni();
	int GetErrorCode();

private:
	void GetFileData(string filePath);

private:
	std::string m_filePath;
	std::vector<std::string> m_vData;
	int m_errorCode;
};

#endif // __INI_PARSER_H__