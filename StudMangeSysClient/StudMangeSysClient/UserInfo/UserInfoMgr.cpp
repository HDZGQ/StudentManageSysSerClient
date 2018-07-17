#include "UserInfoMgr.h"
#include "CheckTool.h"

UserInfoMgr::UserInfoMgr()
{
	m_vOperPer.push_back(OPER_PER_LOGIN);
	m_vOperPer.push_back(OPER_PER_REGISTER);

	sName = "";
	sAccount = "";
	iUserId = 0;
	sIdent = 1;
	sSex = 0;
}

UserInfoMgr::~UserInfoMgr()
{

}

void UserInfoMgr::SetSomeInfo(string name, string sAccount, unsigned int userId, short sIdent, short sSex)
{
	sName = name;
	sAccount = sAccount;
	iUserId = userId;
	this->sIdent = sIdent;
	this->sSex = sSex;
}

void UserInfoMgr::SetUserName(string name)
{
	sName = name;
}

string UserInfoMgr::GetUserName()
{
	return sName;
}

void UserInfoMgr::SetUserAccount(string sAccount)
{
	sAccount = sAccount;
}

string UserInfoMgr::GetUserAccount()
{
	return sAccount;
}

void UserInfoMgr::SetUserUserId(unsigned int userId)
{
	iUserId = userId;
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

bool UserInfoMgr::SetVOperPer(string str)
{
	if (str.empty())
		return false;

	vector<string> vecStr = CheckTool::Splite(str, "|");

	vector<OperPermission> vOperPerTmp;
	for (vector<string>::iterator iter = vecStr.begin();iter != vecStr.end(); iter++)
	{
		if (iter->empty() || !CheckTool::CheckStringByValid(*iter, "0~9"))
			return false;

		int iValue = atoi(iter->c_str());
		vOperPerTmp.push_back((OperPermission)iValue);
	}

	//ШЅжи
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