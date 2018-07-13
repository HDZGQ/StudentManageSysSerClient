#ifndef __USERINFOMGR_H__
#define __USERINFOMGR_H__

#include <map>
#include "xSingleton.h"
#include "PublicDef.h"

using namespace std;

class UserInfoMgr : public xSingleton<UserInfoMgr>
{
public: 
	UserInfoMgr();
	~UserInfoMgr();

	bool InsertInfo(unsigned __int64 socketId, UserInfo userInfo);
	bool InsertInfo(unsigned __int64 socketId, string strIP, unsigned short sPort);

	bool RemoveInfoBySocketId(unsigned __int64 socketId);
	bool RemoveInfoByUserId(unsigned int iUserId);

	bool SetUserIdBySocketId(unsigned __int64 socketId, unsigned int iUserId);
	unsigned int GetUserIdBySocketId(unsigned __int64 socketId);

	bool SetAuthorityBySocketId(unsigned __int64 socketId, string strAuthority);
	string GetAuthorityBySocketId(unsigned __int64 socketId);

	bool SetAccountBySocketId(unsigned __int64 socketId, string strAccount);
	string GetAccountBySocketId(unsigned __int64 socketId);

	bool SetRegNeedCountBySocketId(unsigned __int64 socketId, short sRegNeedCount);
	short GetRegNeedCountBySocketId(unsigned __int64 socketId);
private:
	map<unsigned __int64, UserInfo> m_mUserInfo;
};

#endif