#ifndef __CHECKTOOL_H__
#define __CHECKTOOL_H__

#include <iostream>
#include <string>
#include <vector>

using namespace std;

class CheckTool
{
public:
	CheckTool();
	~CheckTool();

	static bool CheckStringLen(string str, unsigned len);

	//validStr是合法字符串，用分隔符|隔开。暂时只能判断只能含有这些就是合法的，并没有含有多少和在具体位置出现
	static bool CheckStringByValid(string str, string validStr);

	
private:
	static bool CheckValidString(vector<string> strVec);
	static bool CheckCharByValid(char ch, vector<string> &strVec);
	static int FindValidIndex(string str);


};

static bool CheckValidForNumber(char ch, string str);
static bool CheckValidForLowercase(char ch, string str);
static bool CheckValidForUppecase(char ch, string str);
static bool CheckValidForOneChar(char ch, string str);

#endif