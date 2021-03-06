#ifndef __STRINGTOOL_H__
#define __STRINGTOOL_H__

#include <iostream>
#include <string>
#include <vector>

#include "PublicDef.h"

using namespace std;

class StringTool
{
public:
	StringTool();
	~StringTool();

	static vector<string> Splite(string str, string strSpl="|");

	//组合数组成带分隔符的字符串
	static string CombVecToStr(vector<OperPermission> vecOper, string strCom="|");
	static string CombVecToStr(vector<int> vec, string strCom="|");
	static string CombVecToStr(vector<string> vec, string strCom="|");
	static string CombArrayToStr(unsigned char *upArr, unsigned iLen, string strCom="|");

	static string ToLowercase(string strSrc);
	static string ToUppecase(string strSrc);

	static string NumberToStr(int n);

	//通过分隔符把这两个字符串分割后组合成mysql update 的set语句部分
	static string CombToSqlUpdateSetStr(string strField, string strValue, string strSpl="|");
	//把两个vector组合成mysql update 的set语句部分
	static string CombToSqlUpdateSetStr(vector<string> vecStrField, vector<string> vecStrValue);

	static bool StrSpliteToUcArray(unsigned char *arr, unsigned int iLen, string str, string strSpl="|");

	/*
	* 功能：设置字符串域宽，不足用别的字符填充（字节算）
	* 参数描述：str被操作字符串；iWidth宽度；fillChar填充字符，域宽长度刚好或者过长，不填充；dirFlag填充方式，0右填充，1左填充
	*/
	static string SetStringFieldWidth(string str, unsigned iWidth, char fillChar=' ', unsigned char dirFlag=0); 
};

#endif