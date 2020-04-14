#include "IniParser.h"
#include <sstream>


void trim(string &s)
{
	if (!s.empty())
	{
		if(-1 != s.find_last_of("\n"))
			s.replace(s.find_last_of("\n"), 1, "");
		s.erase(0, s.find_first_not_of(" "));
		s.erase(s.find_last_not_of(" ") + 1);
	}
}

IniParser::IniParser(std::string filePath)
	:m_filePath(filePath)
	, m_errorCode(0)
{
	GetFileData(filePath);
}

void IniParser::GetFileData(std::string filePath)
{
	char sLine[1024];

	FILE *fp;
	if (NULL == (fp = fopen(filePath.c_str(), "r"))) {
		printf("fopen %s error\n", filePath.c_str());
		m_errorCode = -1;
		return;
	}

	while (NULL != fgets(sLine, 1024, fp)) {
		m_vData.push_back(sLine);
	}


	fclose(fp);
}

int IniParser::GetErrorCode()
{
	return m_errorCode;
}

std::string IniParser::GetIniKeyString(const char *title, const char *key, const char *nullVal)
{

	int  flag = 0;
	char sTitle[32], *wTmp;
	static char sLine[1024];

	sprintf(sTitle, "[%s]", title);

	for (int i = 0; i < (int)m_vData.size(); i++)
	{
		memset(sLine, 0x0, 1024);
		memcpy(sLine, m_vData[i].c_str(), m_vData[i].length());
		// ÕâÊÇ×¢ÊÍÐÐ  
		if (0 == strncmp("//", sLine, 2)) continue;
		if ('#' == sLine[0])              continue;

		wTmp = strchr(sLine, '=');
		if ((NULL != wTmp) && (1 == flag)) {
			if (0 == strncmp(key, sLine, strlen(key))) {
				string keyval = wTmp + 1;
				
				trim(keyval);
				if (keyval == "")
				{
					return nullVal;
				}
				else
				{
					return keyval;
				}
			}
		}
		else {
			if (0 == strncmp(sTitle, sLine, strlen(sTitle))) {
				flag = 1;
			}
		}
	}

	return nullVal;
}

int IniParser::GetIniKeyInt(const char *title, const char *key, int nullVal)
{
	char sVal[32];
	int num;
	
	istringstream iss(GetIniKeyString(title, key, sVal));
	iss >> num;
	return num;
}


float IniParser::GetIniKeyFloat(const char *title, const char *key, float nullVal)
{
	char sVal[32];
	float num;
	
	sprintf(sVal, "%f", nullVal);

	istringstream iss(GetIniKeyString(title, key, sVal));
	iss >> num;
	return num;
}

void IniParser::PutIniKeyString(const char *title, const char *key, char *val)
{
	int  flag = 0;
	char sLine[4096], sTitle[32], *wTmp;

	sprintf(sTitle, "[%s]", title);

	for (int i = 0; i < (int)m_vData.size(); i++)
	{
		memset(sLine, 0x0, 4096);
		memcpy(sLine, m_vData[i].c_str(), m_vData[i].length());
		if (2 != flag) { 
			wTmp = strchr(sLine, '=');
			if ((NULL != wTmp) && (1 == flag)) {
				if (0 == strncmp(key, sLine, strlen(key))) {  
					flag = 2;  
					sprintf(wTmp + 1, "%s\n", val);
				}
			}
			else {
				if (0 == strncmp(sTitle, sLine, strlen(sTitle))) {  
					flag = 1;
				}
			}
		}
		sLine[strlen(sLine) + 1] = '\n';
		m_vData[i] = sLine;
	} 
}

void IniParser::PutIniKeyInt(const char *title, const char *key, int val)
{
	char sVal[32];
	sprintf(sVal, "%d", val);
	return PutIniKeyString(title, key, sVal);
}

void IniParser::PutIniKeyFloat(const char *title, const char *key, float val)
{
	char sVal[32];
	sprintf(sVal, "%.2f", val);
	return PutIniKeyString(title, key, sVal);
}

void IniParser::SaveIni()
{
	FILE *fp;
	char sLine[4096];

	if (NULL == (fp = fopen(m_filePath.c_str(), "w"))) {
		printf("fopen %s error\n", m_filePath.c_str());
		return;
	}

	for (int i = 0; i < (int)m_vData.size(); i++)
	{
		memset(sLine, 0x0, 4096);
		memcpy(sLine, m_vData[i].c_str(), m_vData[i].length() + 1);
		fputs(sLine, fp);
	}

	fclose(fp);

}

IniParser::~IniParser()
{
}