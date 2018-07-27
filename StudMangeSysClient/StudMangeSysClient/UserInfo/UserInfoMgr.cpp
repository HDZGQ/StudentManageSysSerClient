#include "UserInfoMgr.h"
#include "CheckTool.h"
#include "StringTool.h"

UserInfoMgr::UserInfoMgr()
{
	initAllSubjects();

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

string UserInfoMgr::GetChineseNameByType(SubjectsType sType)
{
	if (sType <= SUBJECTS_TYPE_START || sType >= SUBJECTS_TYPE_END)
	{
		return "";
	}

	return m_mAllSubjects[sType].strChineseName;
}

bool UserInfoMgr::CanFindSubjectsType(SubjectsType sType)
{
	if (sType <= SUBJECTS_TYPE_START || sType >= SUBJECTS_TYPE_END || m_mAllSubjects.find(sType) == m_mAllSubjects.end())
	{
		return false;
	}

	return true;
}