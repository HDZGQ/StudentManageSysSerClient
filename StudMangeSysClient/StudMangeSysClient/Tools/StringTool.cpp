#include "StringTool.h"

StringTool::StringTool()
{

}

StringTool::~StringTool()
{

}


vector<string> StringTool::Splite(string str, string strSpl)
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

string StringTool::CombVecToStr(vector<OperPermission> vecOper, string strCom)
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

string StringTool::CombVecToStr(vector<int> vec, string strCom)
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

string StringTool::CombVecToStr(vector<string> vec, string strCom)
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

		strRes += vec.at(i);
	}

	return strRes;
}

string StringTool::CombArrayToStr(unsigned char *upArr, unsigned iLen, string strCom)
{
	string strRes = "";
	if (0 == iLen)
		return strRes;

	for (unsigned i=0 ; i<iLen; i++)
	{
		if (i != 0)
		{
			strRes += strCom;
		}

		strRes += NumberToStr((int)upArr[i]);
	}

	return strRes;
}

string StringTool::ToLowercase(string strSrc)
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

string StringTool::ToUppecase(string strSrc)
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

string StringTool::NumberToStr(int n)
{
	char ch[12];
	memset(ch, 0, sizeof(ch));
	sprintf_s(ch, sizeof(ch), "%d",n);

	string str = ch;
	return str;
}

string StringTool::CombToSqlUpdateSetStr(string strField, string strValue, string strSpl)
{
	string strRes = "";

	vector<string> vecField = Splite(strField, strSpl);
	vector<string> vecValue = Splite(strValue, strSpl);

	if (vecField.size() == 0 || vecValue.size() == 0 || vecField.size() != vecValue.size())
	{
		return strRes;
	}

	char ch[30];
	for (unsigned i=0; i<vecField.size(); i++)
	{
		memset(ch, 0, sizeof(ch));
		sprintf_s(ch, sizeof(ch), "%s=%s", vecField.at(i).c_str(), vecValue.at(i).c_str());
		if (!strRes.empty())
		{
			strRes += ",";
		}
		strRes += ch;
	}

	return strRes;
}

bool StringTool::StrSpliteToIntArray(unsigned char *arr, unsigned int iLen, string str, string strSpl)
{
	if (NULL == arr || 0 == iLen || str.empty())
	{
		return false;
	}

	vector<string> vecStr = StringTool::Splite(str, strSpl);

	for (unsigned i = 0; i < vecStr.size() && i < iLen; i++)
	{
		arr[i] = (unsigned char)atoi(vecStr.at(i).c_str());
	}

	return true;
}

string StringTool::SetStringFieldWidth(string str, unsigned iWidth, char fillChar, unsigned char dirFlag)
{
	string strRes = str;
	if (strRes.empty() || 0 == iWidth || (dirFlag!=0 && dirFlag!=1))
	{
		return strRes;
	}

	unsigned strLen = strRes.size();
	if (strLen >= iWidth)
	{
		return strRes;
	}

	unsigned needLen = iWidth - strLen;
	string fillString;
	for (unsigned i=0; i<needLen; i++)
	{
		fillString.append(1, fillChar);
	}

	if (0 == dirFlag)
	{
		strRes += fillString;
	}
	else
	{
		strRes = fillString + strRes;
	}

	return strRes;
}