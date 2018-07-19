#include "CheckTool.h"

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

vector<string> CheckTool::Splite(string str, string strSpl)
{
	vector<string> vecStr;

	if (str.empty())
		return vecStr;

	if (strSpl.empty())
	{
		vecStr.push_back(str);
		return vecStr;
	}

	int iPos = 0;
	string strTmp = "";
	while (1)
	{
		int iLen = str.find(strSpl, iPos);
		if (iLen == string::npos)
		{
			strTmp = str.substr(iPos);
			if (!strTmp.empty())
				vecStr.push_back(strTmp);
			break;
		}

		strTmp = str.substr(iPos, iLen-iPos);
		vecStr.push_back(strTmp);
		iPos = iLen + strSpl.size();
	}

	return vecStr;
}

string CheckTool::CombVecToStr(vector<OperPermission> vecOper, string strCom)
{
	string strRes = "";
	if (vecOper.empty())
		return strRes;
	
	for (unsigned i=0 ; i<vecOper.size(); i++)
	{
		if (i != 0)
		{
			strRes += strCom;
		}

		char ch[11];
		memset(ch, 0, sizeof(ch));
		sprintf_s(ch, sizeof(ch), "%d", (int)vecOper.at(i));
		strRes += ch;
	}

	return strRes;
}

string CheckTool::CombVecToStr(vector<int> vec, string strCom)
{
	string strRes = "";
	if (vec.empty())
		return strRes;

	for (unsigned i=0 ; i<vec.size(); i++)
	{
		if (i != 0)
		{
			strRes += strCom;
		}

		char ch[11];
		memset(ch, 0, sizeof(ch));
		sprintf_s(ch, sizeof(ch), "%d", vec.at(i));
		strRes += ch;
	}

	return strRes;
}


bool CheckTool::CheckStringByValid(string str, string validStr)
{
	if (validStr.empty())
		return true;
	
	vector<string> vecStr = Splite(validStr, "|");

	if (!CheckValidString(vecStr))
		return false;

	for (unsigned i=0; i < str.size(); i++)
	{
		if (!CheckCharByValid(str.at(i), vecStr))
			return false;
	}

	return true;
}

string CheckTool::ToLowercase(string strSrc)
{
	string strRes = strSrc;
	if (strSrc.empty())
	{
		return strRes;
	}

	for (unsigned i = 0; i < strSrc.size(); i++)
	{
		if (strSrc.at(i) >= 'A' && strSrc.at(i) <= 'Z')
		{
			strRes.at(i) = strRes.at(i)+32;
		}
	}

	return strRes;
}

string CheckTool::ToUppecase(string strSrc)
{
	string strRes = strSrc;
	if (strSrc.empty())
	{
		return strRes;
	}

	for (unsigned i = 0; i < strSrc.size(); i++)
	{
		if (strSrc.at(i) >= 'a' && strSrc.at(i) <= 'z')
		{
			strRes.at(i) = strRes.at(i)-32;
		}
	}

	return strRes;
}

string CheckTool::NumberToStr(int n)
{
	char ch[12];
	memset(ch, 0, sizeof(ch));
	sprintf_s(ch, sizeof(ch), "%d",n);

	string str = ch;
	return str;
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