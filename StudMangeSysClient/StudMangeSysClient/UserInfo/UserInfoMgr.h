#ifndef __USERINFOMGR_H__
#define __USERINFOMGR_H__

#include <map>
#include <vector>
#include "xSingleton.h"
#include "ProtoDef.h"
#include "ProcDef.h"

using namespace std;

class UserInfoMgr : public xSingleton<UserInfoMgr>
{
public:
	UserInfoMgr();
	~UserInfoMgr();

	void SetSomeInfo(string name, string sAccount, unsigned int userId, short sIdent, short sSex);

	void SetUserName(string name);
	string GetUserName();

	void SetUserAccount(string sAccount);
	string GetUserAccount();

	void SetUserUserId(unsigned int userId);
	unsigned int GetUserUserId();

	void SetUserIdent(short sIdent);
	short GetUserIdent();

	void SetUserSex(short sSex);
	short GetUserSex();

	void SetUserMark(short sMark);
	short GetUserMark();

	//用户本人自己权限处理
	bool CheckOperPerValid(OperPermission OperPer);
	bool InitVOperPer();
	bool SetVOperPer(string str);
	//所有权限操作
	bool CanFindInAllOperPer(OperPermission OperPer);
	string GetDescByOperPer(OperPermission OperPer); //获取权限的中文描述

	string GetChineseNameByType(SubjectsType sType);
	SubjectsType GetTypeByEnglishName(string strName);
	bool CanFindSubjectsType(SubjectsType sType);
protected:
	void initAllSubjects();
	void initAllOperPer();

private:
	string sName;
	string sAccount;
	unsigned int iUserId;
	short sIdent;
	short sSex;

	//用户动态记录
	short sMark;

	//用户自己权限
	vector<OperPermission> m_vOperPer;
	//所有权限集合
	map<OperPermission, string> m_mAllOperPer;

	//所有科目
	map<SubjectsType, SubjectsData> m_mAllSubjects;
};

#endif