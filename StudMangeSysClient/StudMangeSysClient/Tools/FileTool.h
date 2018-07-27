#ifndef __FILETOOL_H__
#define __FILETOOL_H__

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

class FileTool
{
public:
	FileTool();
	~FileTool();

	static bool ReadFileToStrVec(vector<string>& vecStr, string strPath);
};

#endif