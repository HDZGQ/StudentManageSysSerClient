#include "UserInfoMgr.h"
#include "LockTools.h"
#include "StringTool.h"

UserInfoMgr::UserInfoMgr()
{
	initAllOperPer();
}

UserInfoMgr::~UserInfoMgr()
{

}

void UserInfoMgr::initAllOperPer()
{
	m_mAllOperPer.insert(pair<OperPermission, string>(OPER_PER_LOGIN, "登录权限"));
	m_mAllOperPer.insert(pair<OperPermission, string>(OPER_PER_REGISTER, "注册权限"));

	m_mAllOperPer.insert(pair<OperPermission, string>(OPER_PER_ADDBATCHSCOREBYONESUBJECT, "单科批量增加成绩"));
	m_mAllOperPer.insert(pair<OperPermission, string>(OPER_PER_ADDBATCHSCOREBYSUBJECTS, "现有所有科目"));
	m_mAllOperPer.insert(pair<OperPermission, string>(OPER_PER_ADDSINGLESCOREBYONESUBJECT, "单科单条增加成绩"));
	m_mAllOperPer.insert(pair<OperPermission, string>(OPER_PER_ADDSINGLESCOREBYSUBJECTS, "现有所有科目单条增加成绩"));
	//m_mAllOperPer.insert(pair<OperPermission, string>(OPER_PER_UPDATEBATCHSCOREBYONESUBJECT, "单科批量更改成绩"));
	//m_mAllOperPer.insert(pair<OperPermission, string>(OPER_PER_UPDATEBATCHSCOREBYSUBJECTS, "现有所有科目批量更改成绩"));
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
	//m_mAllOperPer.insert(pair<OperPermission, string>(OPER_PER_UPDATEBATCHUSERINFOBYONE, "单个字段批量更改用户信息"));
	//m_mAllOperPer.insert(pair<OperPermission, string>(OPER_PER_UPDATEBATCHUSERINFOBYMORE, "部分固定字段批量更改用户信息"));
	m_mAllOperPer.insert(pair<OperPermission, string>(OPER_PER_UPDATESINGLEUSERINFOEBYONE, "单个字段单条更改用户信息"));
	m_mAllOperPer.insert(pair<OperPermission, string>(OPER_PER_UPDATESINGLEUSERINFOBYMORE, "部分固定字段单条更改用户信息"));
	m_mAllOperPer.insert(pair<OperPermission, string>(OPER_PER_SELECTBATCHUSERINFO, "批量查询用户信息"));
	m_mAllOperPer.insert(pair<OperPermission, string>(OPER_PER_SELECTSINGLEUSERINFO, "单条查询用户信息"));
	m_mAllOperPer.insert(pair<OperPermission, string>(OPER_PER_DELETEBATCHUSERINFO, "批量删除用户信息"));
	m_mAllOperPer.insert(pair<OperPermission, string>(OPER_PER_DELETESINGLEUSERINFO, "单条删除用户信息"));

	m_mAllOperPer.insert(pair<OperPermission, string>(OPER_PER_ADDAUTHORITY, "增加某用户一种或者多种权限"));
	m_mAllOperPer.insert(pair<OperPermission, string>(OPER_PER_DELETEAUTHORITY, "删除某用户一种或者多种权限"));
}

bool UserInfoMgr::InsertInfo(unsigned __int64 socketId, UserInfo userInfo)
{
	map<unsigned __int64, UserInfo>::iterator iter = m_mUserInfo.find(socketId);
	if (iter == m_mUserInfo.end())
	{
		UserInfoLock::GetInstance()->Lock();
		m_mUserInfo.insert(pair<unsigned __int64, UserInfo>(socketId, userInfo));
		UserInfoLock::GetInstance()->Unlock();

		return true;
	}
	
	return false;
}

bool UserInfoMgr::InsertInfo(unsigned __int64 socketId, string strIP, unsigned short sPort, PER_HANDLE_DATA* PerHandleData)
{
	map<unsigned __int64, UserInfo>::iterator iter = m_mUserInfo.find(socketId);
	if (iter == m_mUserInfo.end())
	{
		UserInfo userInfo;
		userInfo.strIP = strIP;
		userInfo.sPort = sPort;
		userInfo.PerHandleData = PerHandleData;
		
		UserInfoLock::GetInstance()->Lock();
		m_mUserInfo.insert(pair<unsigned __int64, UserInfo>(socketId, userInfo));
		UserInfoLock::GetInstance()->Unlock();

		printf("%s  socketId[%llu]  strIP[%s]  sPort[%d]\n", __FUNCTION__, socketId, strIP.c_str(), sPort);

		return true;
	}

	return false;
}

bool UserInfoMgr::RemoveInfoBySocketId(unsigned __int64 socketId)
{
	map<unsigned __int64, UserInfo>::iterator iter = m_mUserInfo.find(socketId);
	if (iter != m_mUserInfo.end())
	{
		UserInfoLock::GetInstance()->Lock();

		closesocket((SOCKET)iter->first);
		if (NULL != iter->second.PerHandleData)
			GlobalFree(iter->second.PerHandleData);
		if (NULL != iter->second.PerIoData)
			GlobalFree(iter->second.PerIoData);

		m_mUserInfo.erase(iter);

		UserInfoLock::GetInstance()->Unlock();

		printf("%s  socketId[%llu]\n", __FUNCTION__, socketId);
	}

	return true;
}

bool UserInfoMgr::RemoveInfoByUserId(unsigned int iUserId)
{
	map<unsigned __int64, UserInfo>::iterator iter = m_mUserInfo.begin();
	for (; iter != m_mUserInfo.end(); iter++)
	{
		if (iter->second.iUserId == iUserId)
		{
			UserInfoLock::GetInstance()->Lock();

			closesocket((SOCKET)iter->first);
			if (NULL != iter->second.PerHandleData)
				GlobalFree(iter->second.PerHandleData);
			if (NULL != iter->second.PerIoData)
				GlobalFree(iter->second.PerIoData);

			m_mUserInfo.erase(iter);

			UserInfoLock::GetInstance()->Unlock();

			printf("%s  iUserId[%u]\n", __FUNCTION__, iUserId);

			break;
		}
	}

	return true;
}

bool UserInfoMgr::SetUserIdBySocketId(unsigned __int64 socketId, unsigned int iUserId)
{
	map<unsigned __int64, UserInfo>::iterator iter = m_mUserInfo.find(socketId);
	if (iter != m_mUserInfo.end())
	{
		UserInfoLock::GetInstance()->Lock();
		m_mUserInfo[socketId].iUserId = iUserId;
		UserInfoLock::GetInstance()->Unlock();

		return true;
	}

	return false;
}

unsigned int UserInfoMgr::GetUserIdBySocketId(unsigned __int64 socketId)
{
	map<unsigned __int64, UserInfo>::iterator iter = m_mUserInfo.find(socketId);
	if (iter != m_mUserInfo.end())
	{
		UserInfoLock::GetInstance()->Lock();
		return m_mUserInfo[socketId].iUserId;
		UserInfoLock::GetInstance()->Unlock();
	}

	return 0; //学号肯定要设置大于0的
}

bool UserInfoMgr::SetAuthorityBySocketId(unsigned __int64 socketId, string strAuthority)
{
	map<unsigned __int64, UserInfo>::iterator iter = m_mUserInfo.find(socketId);
	if (iter != m_mUserInfo.end())
	{
		UserInfoLock::GetInstance()->Lock();
		m_mUserInfo[socketId].strAuthority = strAuthority;
		UserInfoLock::GetInstance()->Unlock();

		return true;
	}

	return false;
}

string UserInfoMgr::GetAuthorityBySocketId(unsigned __int64 socketId)
{
	map<unsigned __int64, UserInfo>::iterator iter = m_mUserInfo.find(socketId);
	if (iter != m_mUserInfo.end())
	{
		UserInfoLock::GetInstance()->Lock();
		return m_mUserInfo[socketId].strAuthority;
		UserInfoLock::GetInstance()->Unlock();
	}

	return ""; 
}

bool UserInfoMgr::SetAuthorityByAccount(string strAccount, string strAuthority)
{
	map<unsigned __int64, UserInfo>::iterator iter = m_mUserInfo.begin();
	for (; iter != m_mUserInfo.end(); iter++)
	{
		if (iter->second.strAccount == strAccount)
		{
			UserInfoLock::GetInstance()->Lock();
			iter->second.strAuthority = strAuthority;
			UserInfoLock::GetInstance()->Unlock();

			return true;
		}
	}

	return false;
}

string UserInfoMgr::GetAuthorityByAccount(string strAccount)
{
	map<unsigned __int64, UserInfo>::iterator iter = m_mUserInfo.begin();
	for (; iter != m_mUserInfo.end(); iter++)
	{
		if (iter->second.strAccount == strAccount)
		{
			UserInfoLock::GetInstance()->Lock();
			return iter->second.strAuthority;
			UserInfoLock::GetInstance()->Unlock();
		}
	}

	return "";
}

bool UserInfoMgr::SetAccountBySocketId(unsigned __int64 socketId, string strAccount)
{
	map<unsigned __int64, UserInfo>::iterator iter = m_mUserInfo.find(socketId);
	if (iter != m_mUserInfo.end())
	{
		UserInfoLock::GetInstance()->Lock();
		m_mUserInfo[socketId].strAccount = strAccount;
		UserInfoLock::GetInstance()->Unlock();

		return true;
	}

	return false;
}

string UserInfoMgr::GetAccountBySocketId(unsigned __int64 socketId)
{
	map<unsigned __int64, UserInfo>::iterator iter = m_mUserInfo.find(socketId);
	if (iter != m_mUserInfo.end())
	{
		UserInfoLock::GetInstance()->Lock();
		return m_mUserInfo[socketId].strAccount;
		UserInfoLock::GetInstance()->Unlock();
	}

	return ""; 
}

bool UserInfoMgr::SetIdentBySocketId(unsigned __int64 socketId, short sIdent)
{
	map<unsigned __int64, UserInfo>::iterator iter = m_mUserInfo.find(socketId);
	if (iter != m_mUserInfo.end())
	{
		UserInfoLock::GetInstance()->Lock();
		m_mUserInfo[socketId].sIdent = sIdent;
		UserInfoLock::GetInstance()->Unlock();

		return true;
	}

	return false;
}

short UserInfoMgr::GetIdentBySocketId(unsigned __int64 socketId)
{
	map<unsigned __int64, UserInfo>::iterator iter = m_mUserInfo.find(socketId);
	if (iter != m_mUserInfo.end())
	{
		UserInfoLock::GetInstance()->Lock();
		return m_mUserInfo[socketId].sIdent;
		UserInfoLock::GetInstance()->Unlock();
	}

	return 0; //非法值
}

bool UserInfoMgr::SetRegNeedCountBySocketId(unsigned __int64 socketId, short sRegNeedCount)
{
	map<unsigned __int64, UserInfo>::iterator iter = m_mUserInfo.find(socketId);
	if (iter != m_mUserInfo.end())
	{
		UserInfoLock::GetInstance()->Lock();
		m_mUserInfo[socketId].sRegNeedCount = sRegNeedCount;
		UserInfoLock::GetInstance()->Unlock();

		return true;
	}

	return false;
}

short UserInfoMgr::GetRegNeedCountBySocketId(unsigned __int64 socketId)
{
	map<unsigned __int64, UserInfo>::iterator iter = m_mUserInfo.find(socketId);
	if (iter != m_mUserInfo.end())
	{
		UserInfoLock::GetInstance()->Lock();
		return m_mUserInfo[socketId].sRegNeedCount;
		UserInfoLock::GetInstance()->Unlock();
	}

	return -1; 
}

bool UserInfoMgr::SetStrTmpDataBySocketId(unsigned __int64 socketId, string strTmpData)
{
	map<unsigned __int64, UserInfo>::iterator iter = m_mUserInfo.find(socketId);
	if (iter != m_mUserInfo.end())
	{
		UserInfoLock::GetInstance()->Lock();
		m_mUserInfo[socketId].strTmpData = strTmpData;
		UserInfoLock::GetInstance()->Unlock();

		return true;
	}

	return false;
}

string UserInfoMgr::GetStrTmpDataBySocketId(unsigned __int64 socketId)
{
	map<unsigned __int64, UserInfo>::iterator iter = m_mUserInfo.find(socketId);
	if (iter != m_mUserInfo.end())
	{
		UserInfoLock::GetInstance()->Lock();
		return m_mUserInfo[socketId].strTmpData;
		UserInfoLock::GetInstance()->Unlock();
	}

	return ""; 
}

bool UserInfoMgr::SetPerHandleDataBySocketId(unsigned __int64 socketId, PER_HANDLE_DATA* PerHandleData)
{
	map<unsigned __int64, UserInfo>::iterator iter = m_mUserInfo.find(socketId);
	if (iter != m_mUserInfo.end())
	{
		UserInfoLock::GetInstance()->Lock();
		m_mUserInfo[socketId].PerHandleData = PerHandleData;
		UserInfoLock::GetInstance()->Unlock();

		return true;
	}

	return false;
}

PER_HANDLE_DATA* UserInfoMgr::GetPerHandleDataBySocketId(unsigned __int64 socketId)
{
	map<unsigned __int64, UserInfo>::iterator iter = m_mUserInfo.find(socketId);
	if (iter != m_mUserInfo.end())
	{
		UserInfoLock::GetInstance()->Lock();
		return m_mUserInfo[socketId].PerHandleData;
		UserInfoLock::GetInstance()->Unlock();
	}

	return NULL; 
}

bool UserInfoMgr::SetPerIoDataBySocketId(unsigned __int64 socketId, LPPER_IO_OPERATION_DATA PerIoData)
{
	map<unsigned __int64, UserInfo>::iterator iter = m_mUserInfo.find(socketId);
	if (iter != m_mUserInfo.end())
	{
		UserInfoLock::GetInstance()->Lock();
		m_mUserInfo[socketId].PerIoData = PerIoData;
		UserInfoLock::GetInstance()->Unlock();

		return true;
	}

	return false;
}

LPPER_IO_OPERATION_DATA UserInfoMgr::GetPerIoDataBySocketId(unsigned __int64 socketId)
{
	map<unsigned __int64, UserInfo>::iterator iter = m_mUserInfo.find(socketId);
	if (iter != m_mUserInfo.end())
	{
		UserInfoLock::GetInstance()->Lock();
		return m_mUserInfo[socketId].PerIoData;
		UserInfoLock::GetInstance()->Unlock();
	}

	return NULL; 
}

void UserInfoMgr::GetDefaultAuthorityByIdent(IdentType Ident, vector<OperPermission>& vecOper)
{
	vecOper.clear();
	if (IDENT_TYPE_STUDENT == Ident)
	{
		vecOper.push_back(OPER_PER_LOGIN);
		vecOper.push_back(OPER_PER_REGISTER);

		vecOper.push_back(OPER_PER_SELECTSINGLESCOREBYONESUBJECT);
		vecOper.push_back(OPER_PER_SELECTSINGLESCOREBYSUBJECTS);

		vecOper.push_back(OPER_PER_UPDATESINGLEUSERINFOEBYONE);
		vecOper.push_back(OPER_PER_UPDATESINGLEUSERINFOBYMORE);
		vecOper.push_back(OPER_PER_SELECTSINGLEUSERINFO);
	}
	else if (IDENT_TYPE_TEACHER == Ident)
	{
		vecOper.push_back(OPER_PER_LOGIN);
		vecOper.push_back(OPER_PER_REGISTER);

		vecOper.push_back(OPER_PER_ADDBATCHSCOREBYONESUBJECT);
		vecOper.push_back(OPER_PER_ADDBATCHSCOREBYSUBJECTS);
		vecOper.push_back(OPER_PER_ADDSINGLESCOREBYONESUBJECT);
		vecOper.push_back(OPER_PER_ADDSINGLESCOREBYSUBJECTS);
		vecOper.push_back(OPER_PER_UPDATEBATCHSCOREBYONESUBJECT);
		vecOper.push_back(OPER_PER_UPDATEBATCHSCOREBYSUBJECTS);
		vecOper.push_back(OPER_PER_UPDATESINGLESCOREBYONESUBJECT);
		vecOper.push_back(OPER_PER_UPDATESINGLESCOREBYSUBJECTS);
		vecOper.push_back(OPER_PER_SELECTBATCHSCOREBYONESUBJECT);
		vecOper.push_back(OPER_PER_SELECTBATCHSCOREBYSUBJECTS);
		vecOper.push_back(OPER_PER_SELECTSINGLESCOREBYONESUBJECT);
		vecOper.push_back(OPER_PER_SELECTSINGLESCOREBYSUBJECTS);
		vecOper.push_back(OPER_PER_DELETEBATCHSCORE);
		vecOper.push_back(OPER_PER_DELETESINGLESCORE);
		vecOper.push_back(OPER_PER_ALTERADDONESCORESUBJECT);
		vecOper.push_back(OPER_PER_ALTERDELETEONESCORESUBJECT);

		vecOper.push_back(OPER_PER_ADDBATCHUSERINFO);
		vecOper.push_back(OPER_PER_ADDSINGLEUSERINFO);
		vecOper.push_back(OPER_PER_UPDATEBATCHUSERINFOBYONE);
		vecOper.push_back(OPER_PER_UPDATEBATCHUSERINFOBYMORE);
		vecOper.push_back(OPER_PER_UPDATESINGLEUSERINFOEBYONE);
		vecOper.push_back(OPER_PER_UPDATESINGLEUSERINFOBYMORE);
		vecOper.push_back(OPER_PER_SELECTBATCHUSERINFO);
		vecOper.push_back(OPER_PER_SELECTSINGLEUSERINFO);
		vecOper.push_back(OPER_PER_DELETEBATCHUSERINFO);
		vecOper.push_back(OPER_PER_DELETESINGLEUSERINFO);
	}
	else if (IDENT_TYPE_ADMIN == Ident)
	{
		vecOper.push_back(OPER_PER_LOGIN);
		vecOper.push_back(OPER_PER_REGISTER);

		vecOper.push_back(OPER_PER_ADDBATCHSCOREBYONESUBJECT);
		vecOper.push_back(OPER_PER_ADDBATCHSCOREBYSUBJECTS);
		vecOper.push_back(OPER_PER_ADDSINGLESCOREBYONESUBJECT);
		vecOper.push_back(OPER_PER_ADDSINGLESCOREBYSUBJECTS);
		vecOper.push_back(OPER_PER_UPDATEBATCHSCOREBYONESUBJECT);
		vecOper.push_back(OPER_PER_UPDATEBATCHSCOREBYSUBJECTS);
		vecOper.push_back(OPER_PER_UPDATESINGLESCOREBYONESUBJECT);
		vecOper.push_back(OPER_PER_UPDATESINGLESCOREBYSUBJECTS);
		vecOper.push_back(OPER_PER_SELECTBATCHSCOREBYONESUBJECT);
		vecOper.push_back(OPER_PER_SELECTBATCHSCOREBYSUBJECTS);
		vecOper.push_back(OPER_PER_SELECTSINGLESCOREBYONESUBJECT);
		vecOper.push_back(OPER_PER_SELECTSINGLESCOREBYSUBJECTS);
		vecOper.push_back(OPER_PER_DELETEBATCHSCORE);
		vecOper.push_back(OPER_PER_DELETESINGLESCORE);
		vecOper.push_back(OPER_PER_ALTERADDONESCORESUBJECT);
		vecOper.push_back(OPER_PER_ALTERDELETEONESCORESUBJECT);

		vecOper.push_back(OPER_PER_ADDBATCHUSERINFO);
		vecOper.push_back(OPER_PER_ADDSINGLEUSERINFO);
		vecOper.push_back(OPER_PER_UPDATEBATCHUSERINFOBYONE);
		vecOper.push_back(OPER_PER_UPDATEBATCHUSERINFOBYMORE);
		vecOper.push_back(OPER_PER_UPDATESINGLEUSERINFOEBYONE);
		vecOper.push_back(OPER_PER_UPDATESINGLEUSERINFOBYMORE);
		vecOper.push_back(OPER_PER_SELECTBATCHUSERINFO);
		vecOper.push_back(OPER_PER_SELECTSINGLEUSERINFO);
		vecOper.push_back(OPER_PER_DELETEBATCHUSERINFO);
		vecOper.push_back(OPER_PER_DELETESINGLEUSERINFO);

		vecOper.push_back(OPER_PER_ADDAUTHORITY);
		vecOper.push_back(OPER_PER_DELETEAUTHORITY);
	}
	else
	{
		printf("%s  身份标识错误Ident[%d]\n", __FUNCTION__, Ident);
	}
}

void UserInfoMgr::GetAuthorityByStrAuthority(string strAuthority, vector<OperPermission>& vecOper, unsigned char bType, string strSpl)
{
	vecOper.clear();
	if (strAuthority.empty() || bType < 1 || bType > 2)
	{
		return;
	}

	vector<string> vecStrMyAuthority = StringTool::Splite(strAuthority, strSpl);
	vector<OperPermission> vecOperTmp;
	for (unsigned i =0; i<vecStrMyAuthority.size(); i++)
	{
		vecOperTmp.push_back((OperPermission)atoi(vecStrMyAuthority.at(i).c_str()));
	}

	if (2 == bType)
	{
		for (map<OperPermission, string>::iterator iter=m_mAllOperPer.begin(); iter != m_mAllOperPer.end(); iter++)
		{
			bool bExist = false;
			bool bCountiune = false;
			for (unsigned i=0; i<vecOperTmp.size(); i++)
			{
				if (iter->first == OPER_PER_ADDAUTHORITY || iter->first == OPER_PER_DELETEAUTHORITY)
				{
					bCountiune = true;
					break;
				}
				if (vecOperTmp.at(i) == iter->first)
				{
					bExist = true;
					break;
				}
			}
			if (!bExist && !bCountiune)
			{
				vecOper.push_back(iter->first);
			}
		}
	}
	else
	{
		vecOper.assign(vecOperTmp.begin(), vecOperTmp.end());
	}
}
