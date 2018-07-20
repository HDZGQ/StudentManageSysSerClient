#include "CheckTool.h"
#include "StringTool.h"

static const string g_ValidArry[] = {"A~Z", "a~z", "0~9", "%c"};

typedef bool (*CHECKVALIDFORXXX)(char , string );
CHECKVALIDFORXXX CheckValidForXXX[] = {&CheckValidForUppecase, &CheckValidForLowercase, &CheckValidForNumber, &CheckValidForOneChar};

CheckTool::CheckTool()
{

}

CheckTool::~CheckTool()
{

}

bool CheckTool::CheckStringLen(string str, unsigned len)
{
	if (str.size() > len)
	{
		printf("长度过长，超过限定的%d个字符！\n", len);
		return false;
	}

	return true;
}


bool CheckTool::CheckStringByValid(string str, string validStr)
{
	if (validStr.empty())
		return true;
	
	vector<string> vecStr = StringTool::Splite(validStr, "|");

	if (!CheckValidString(vecStr))
		return false;

	for (unsigned i=0; i < str.size(); i++)
	{
		if (!CheckCharByValid(str.at(i), vecStr))
			return false;
	}

	return true;
}

bool CheckTool::CheckValidString(vector<string> strVec)
{
	if (strVec.empty())
		return true;
	
	unsigned int iCount[4];
	memset(iCount, 0, sizeof(iCount));
	vector<string>::iterator iter = strVec.begin();
	for (; iter!=strVec.end(); )
	{
		if (g_ValidArry[0] == *iter)
		{
			iCount[0]++;
			iter = strVec.erase(iter);
		}
		else if (g_ValidArry[1] == *iter)
		{
			iCount[1]++;
			iter = strVec.erase(iter);
		}
		else if (g_ValidArry[2] == *iter)
		{
			iCount[2]++;
			iter = strVec.erase(iter);
		}
		else
		{
			iCount[3] = iter->size()>iCount[3] ? iter->size() : iCount[3];
			iter++;
		}
	}
	//不能重复，单个匹配的合法性字符串长度必须为1
	if (iCount[0] > 1 || iCount[1] > 1 || iCount[2] > 1 || iCount[3] > 1)
		return false;

	if (strVec.size() <= 1)
		return true;

	for (unsigned i=0; i < strVec.size()-1; i++)
	{
		for (unsigned j=i+1; j < strVec.size(); j++)
		{
			if (strVec.at(i) == strVec.at(j))
			{
				return false;
			}
		}
	}

	return true;
}

bool CheckTool::CheckCharByValid(char ch, vector<string> &strVec)
{
	for (vector<string>::iterator iter = strVec.begin();iter != strVec.end(); iter++)
	{
		int iIndex = FindValidIndex(*iter);
		if (iIndex != -1 && (*CheckValidForXXX[iIndex])(ch, *iter))
		{
			return true;
		}
		else
		{
			if (iIndex == -1)
				return false;
		}
	}

	return false;
}

int CheckTool::FindValidIndex(string str)
{
	for (int iIndex = 0; iIndex < 4; iIndex++)
	{
		if (iIndex < 3 && g_ValidArry[iIndex] == str)
		{
			return iIndex;
		}
		else if (iIndex == 3 && str.size() == 1)
		{
			return iIndex;
		}
	}

	return -1;
}


bool CheckValidForNumber(char ch, string str)
{
	if (ch >= '0' && ch <= '9')
	{
		return true;
	}
	return false;
}

bool CheckValidForLowercase(char ch, string str)
{
	if (ch >= 'a' && ch <= 'z')
	{
		return true;
	}

	return false;
}

bool CheckValidForUppecase(char ch, string str)
{
	if (ch >= 'A' && ch <= 'Z')
	{
		return true;
	}

	return false;
}

bool CheckValidForOneChar(char ch, string str)
{
	if (str.size() != 1)
		return false;

	if (ch == str.at(0))
	{
		return true;
	}

	return false;
}