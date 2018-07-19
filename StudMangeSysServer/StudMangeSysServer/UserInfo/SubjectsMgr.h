#ifndef __SUBJECTSMGR_H__
#define __SUBJECTSMGR_H__

#include <WinSock2.h>
#include <mysql.h>
#include <Windows.h>
#include <vector>
#include <map>
#include "xSingleton.h"
#include "PublicDef.h"

using namespace std;

class SubjectsMgr : public xSingleton<SubjectsMgr>
{
public:
	SubjectsMgr();
	~SubjectsMgr();

	string GetEnglishNameByType(SubjectsType sType); 
	string GetChineseNameByType(SubjectsType sType);
	SubjectsType GetTypeByEnglishName(string strName);
	SubjectsType GetTypeByChineseName(string strName);

	vector<SubjectsType> GetVExistSubjects();
	vector<SubjectsType> GetVNotExistSubjects();

	bool DeleteOneExistSubject(SubjectsType sType);
	bool AddOneExistSubject(SubjectsType sType);


	//服务端开启就调用
	void GetExistSubjectHandle();

	/*
	* 数据库返回处理。strRecord需要从前面操作中带到返回函数的数据
	*/
	void GetExistSubjectReplyHandle(SOCKET SocketId, MYSQL_RES *MysqlRes, string strRecord);

protected:
	void init();

private:
	vector<SubjectsType> m_vExistSubjects;

	map<SubjectsType, SubjectsData> m_mAllSubjects;
};

#endif