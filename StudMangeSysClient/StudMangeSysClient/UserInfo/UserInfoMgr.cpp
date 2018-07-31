#include "UserInfoMgr.h"
#include "CheckTool.h"
#include "StringTool.h"

UserInfoMgr::UserInfoMgr()
{
	initAllSubjects();
	initAllOperPer();

	InitVOperPer();

	sName = "";
	sAccount = "";
	iUserId = 0;
	sIdent = 1;
	sSex = 0;

	sMark = 0;
}

UserInfoMgr::~UserInfoMgr()
{

}

void UserInfoMgr::initAllSubjects()
{
	m_mAllSubjects.insert(pair<SubjectsType, SubjectsData>(SUBJECTS_TYPE_CHINESE, SubjectsData("chinese", "语文")));
	m_mAllSubjects.insert(pair<SubjectsType, SubjectsData>(SUBJECTS_TYPE_MATH, SubjectsData("math", "数学")));
	m_mAllSubjects.insert(pair<SubjectsType, SubjectsData>(SUBJECTS_TYPE_ENGLISH, SubjectsData("english", "英语")));
	m_mAllSubjects.insert(pair<SubjectsType, SubjectsData>(SUBJECTS_TYPE_PHYSICS, SubjectsData("physics", "物理")));
	m_mAllSubjects.insert(pair<SubjectsType, SubjectsData>(SUBJECTS_TYPE_CHEMISTRY, SubjectsData("chemistry", "化学")));
	m_mAllSubjects.insert(pair<SubjectsType, SubjectsData>(SUBJECTS_TYPE_BIOLOGY, SubjectsData("biology", "生物")));
	m_mAllSubjects.insert(pair<SubjectsType, SubjectsData>(SUBJECTS_TYPE_HISTORY, SubjectsData("history", "历史")));
	m_mAllSubjects.insert(pair<SubjectsType, SubjectsData>(SUBJECTS_TYPE_POLITICS, SubjectsData("politics", "政治")));
	m_mAllSubjects.insert(pair<SubjectsType, SubjectsData>(SUBJECTS_TYPE_GEOGRAPHY, SubjectsData("geography", "地理")));
}

void UserInfoMgr::initAllOperPer()
{
	m_mAllOperPer.insert(pair<OperPermission, string>(OPER_PER_LOGIN, "登录权限"));
	m_mAllOperPer.insert(pair<OperPermission, string>(OPER_PER_REGISTER, "注册权限"));

	m_mAllOperPer.insert(pair<OperPermission, string>(OPER_PER_ADDBATCHSCOREBYONESUBJECT, "单科批量增加成绩"));
	m_mAllOperPer.insert(pair<OperPermission, string>(OPER_PER_ADDBATCHSCOREBYSUBJECTS, "现有所有科目"));
	m_mAllOperPer.insert(pair<OperPermission, string>(OPER_PER_ADDSINGLESCOREBYONESUBJECT, "单科单条增加成绩"));
	m_mAllOperPer.insert(pair<OperPermission, string>(OPER_PER_ADDSINGLESCOREBYSUBJECTS, "现有所有科目单条增加成绩"));
	m_mAllOperPer.insert(pair<OperPermission, string>(OPER_PER_UPDATEBATCHSCOREBYONESUBJECT, "单科批量更改成绩"));
	m_mAllOperPer.insert(pair<OperPermission, string>(OPER_PER_UPDATEBATCHSCOREBYSUBJECTS, "现有所有科目批量更改成绩"));
	m_mAllOperPer.insert(pair<OperPermission, string>(OPER_PER_UPDATESINGLESCOREBYONESUBJECT, "单科单条更改成绩"));
	m_mAllOperPer.insert(pair<OperPermission, string>(OPER_PER_UPDATESINGLESCOREBYSUBJECTS, "现有所有科目单条更改成绩"));
	m_mAllOperPer.insert(pair<OperPermission, string>(OPER_PER_SELECTBATCHSCOREBYONESUBJECT, "单科批量查询成绩"));
	m_mAllOperPer.insert(pair<OperPermission, string>(OPER_PER_SELECTBATCHSCOREBYSUBJECTS, "现有所有科目批量查询成绩"));
	m_mAllOperPer.insert(pair<OperPermission, string>(OPER_PER_SELECTSINGLESCOREBYONESUBJECT, "单科单条查询成绩"));
	m_mAllOperPer.insert(pair<OperPermission, string>(OPER_PER_SELECTSINGLESCOREBYSUBJECTS, "现有所有科目单条查询成绩"));
	m_mAllOperPer.insert(pair<OperPermission, string>(OPER_PER_DELETEBATCHSCORE, "批量删除成绩"));
	m_mAllOperPer.insert(pair<OperPermission, string>(OPER_PER_DELETESINGLESCORE, "单条删除成绩"));
	m_mAllOperPer.insert(pair<OperPermission, string>(OPER_PER_ALTERADDONESCORESUBJECT, "增加成绩科目"));
	m_mAllOperPer.insert(pair<OperPermission, string>(OPER_PER_ALTERDELETEONESCORESUBJECT, "删除成绩科目"));

	m_mAllOperPer.insert(pair<OperPermission, string>(OPER_PER_ADDBATCHUSERINFO, "批量增加用户信息"));
	m_mAllOperPer.insert(pair<OperPermission, string>(OPER_PER_ADDSINGLEUSERINFO, "单条增加用户信息"));
	m_mAllOperPer.insert(pair<OperPermission, string>(OPER_PER_UPDATEBATCHUSERINFOBYONE, "单个字段批量更改用户信息"));
	m_mAllOperPer.insert(pair<OperPermission, string>(OPER_PER_UPDATEBATCHUSERINFOBYMORE, "部分固定字段批量更改用户信息"));
	m_mAllOperPer.insert(pair<OperPermission, string>(OPER_PER_UPDATESINGLEUSERINFOEBYONE, "单个字段单条更改用户信息"));
	m_mAllOperPer.insert(pair<OperPermission, string>(OPER_PER_UPDATESINGLEUSERINFOBYMORE, "部分固定字段单条更改用户信息"));
	m_mAllOperPer.insert(pair<OperPermission, string>(OPER_PER_SELECTBATCHUSERINFO, "批量查询用户信息"));
	m_mAllOperPer.insert(pair<OperPermission, string>(OPER_PER_SELECTSINGLEUSERINFO, "单条查询用户信息"));
	m_mAllOperPer.insert(pair<OperPermission, string>(OPER_PER_DELETEBATCHUSERINFO, "批量删除用户信息"));
	m_mAllOperPer.insert(pair<OperPermission, string>(OPER_PER_DELETESINGLEUSERINFO, "单条删除用户信息"));

	m_mAllOperPer.insert(pair<OperPermission, string>(OPER_PER_ADDAUTHORITY, "增加某用户一种或者多种权限"));
	m_mAllOperPer.insert(pair<OperPermission, string>(OPER_PER_DELETEAUTHORITY, "删除某用户一种或者多种权限"));
}


void UserInfoMgr::SetSomeInfo(string name, string sAccount, unsigned int userId, short sIdent, short sSex)
{
	this->sName = name;
	this->sAccount = sAccount;
	this->iUserId = userId;
	this->sIdent = sIdent;
	this->sSex = sSex;
}

void UserInfoMgr::SetUserName(string name)
{
	this->sName = name;
}

string UserInfoMgr::GetUserName()
{
	return sName;
}

void UserInfoMgr::SetUserAccount(string sAccount)
{
	this->sAccount = sAccount;
}

string UserInfoMgr::GetUserAccount()
{
	return sAccount;
}

void UserInfoMgr::SetUserUserId(unsigned int userId)
{
	this->iUserId = userId;
}

unsigned int UserInfoMgr::GetUserUserId()
{
	return iUserId;
}

void UserInfoMgr::SetUserIdent(short sIdent)
{
	this->sIdent = sIdent;
}

short UserInfoMgr::GetUserIdent()
{
	return sIdent;
}

void UserInfoMgr::SetUserSex(short sSex)
{
	this->sSex = sSex;
}

short UserInfoMgr::GetUserSex()
{
	return sSex;
}

void UserInfoMgr::SetUserMark(short sMark)
{
	this->sMark = sMark;
}

short UserInfoMgr::GetUserMark()
{
	return sMark;
}

bool UserInfoMgr::CheckOperPerValid(OperPermission OperPer)
{
	vector<OperPermission>::iterator iter = m_vOperPer.begin();
	for (; iter != m_vOperPer.end(); iter++)
	{
		if (OperPer == *iter)
			return true;
	}

	return false;
}

bool UserInfoMgr::InitVOperPer()
{
	m_vOperPer.clear();

	m_vOperPer.push_back(OPER_PER_LOGIN);
	m_vOperPer.push_back(OPER_PER_REGISTER);

	return true;
}

bool UserInfoMgr::SetVOperPer(string str)
{
	if (str.empty())
		return false;

	vector<string> vecStr = StringTool::Splite(str, "|");

	vector<OperPermission> vOperPerTmp;
	for (vector<string>::iterator iter = vecStr.begin();iter != vecStr.end(); iter++)
	{
		if (iter->empty() || !CheckTool::CheckStringByValid(*iter, "0~9"))
			return false;

		int iValue = atoi(iter->c_str());
		vOperPerTmp.push_back((OperPermission)iValue);
	}

	//去重
	for (vector<OperPermission>::iterator iter = m_vOperPer.begin(); iter != m_vOperPer.end(); iter++)
	{
		for (vector<OperPermission>::iterator iterTmp = vOperPerTmp.begin(); iterTmp != vOperPerTmp.end();)
		{
			if (*iter == *iterTmp)
			{
				iterTmp = vOperPerTmp.erase(iterTmp);
			}
			else
			{
				iterTmp++;
			}
		}
	}

	m_vOperPer.insert(m_vOperPer.end(), vOperPerTmp.begin(), vOperPerTmp.end());

	return true;
}

bool UserInfoMgr::CanFindInAllOperPer(OperPermission OperPer)
{
	map<OperPermission, string>::iterator iter = m_mAllOperPer.begin();
	for (; iter != m_mAllOperPer.end(); iter++)
	{
		if (OperPer == iter->first)
			return true;
	}

	return false;
}

string UserInfoMgr::GetDescByOperPer(OperPermission OperPer)
{
	if (OperPer <= OPER_PER_START || OperPer >= OPER_PER_END)
	{
		return "";
	}

	return m_mAllOperPer[OperPer];
}

string UserInfoMgr::GetChineseNameByType(SubjectsType sType)
{
	if (sType <= SUBJECTS_TYPE_START || sType >= SUBJECTS_TYPE_END)
	{
		return "";
	}

	return m_mAllSubjects[sType].strChineseName;
}

SubjectsType UserInfoMgr::GetTypeByEnglishName(string strName)
{
	map<SubjectsType, SubjectsData>::iterator iter = m_mAllSubjects.begin();
	for (; iter != m_mAllSubjects.end(); iter++)
	{
		if (StringTool::ToLowercase(iter->second.strEnglishName) == StringTool::ToLowercase(strName))
		{
			return iter->first;
		}
	}

	return SUBJECTS_TYPE_INVALID;
}

bool UserInfoMgr::CanFindSubjectsType(SubjectsType sType)
{
	if (sType <= SUBJECTS_TYPE_START || sType >= SUBJECTS_TYPE_END || m_mAllSubjects.find(sType) == m_mAllSubjects.end())
	{
		return false;
	}

	return true;
}