#ifndef __USERINFOMGR_H__
#define __USERINFOMGR_H__

#include <map>
#include <vector>
#include "xSingleton.h"
#include "PublicDef.h"
#include "NetDef.h"

using namespace std;

class UserInfoMgr : public xSingleton<UserInfoMgr>
{
public: 
	UserInfoMgr();
	~UserInfoMgr();

	bool InsertInfo(unsigned __int64 socketId, UserInfo userInfo);
	bool InsertInfo(unsigned __int64 socketId, string strIP, unsigned short sPort, PER_HANDLE_DATA* PerHandleData);

	bool RemoveInfoBySocketId(unsigned __int64 socketId);
	bool RemoveInfoByUserId(unsigned int iUserId);

	bool SetUserIdBySocketId(unsigned __int64 socketId, unsigned int iUserId);
	unsigned int GetUserIdBySocketId(unsigned __int64 socketId);

	bool SetAuthorityBySocketId(unsigned __int64 socketId, string strAuthority);
	string GetAuthorityBySocketId(unsigned __int64 socketId);

	bool SetAuthorityByAccount(string strAccount, string strAuthority);
	string GetAuthorityByAccount(string strAccount);

	bool SetAccountBySocketId(unsigned __int64 socketId, string strAccount);
	string GetAccountBySocketId(unsigned __int64 socketId);

	bool SetIdentBySocketId(unsigned __int64 socketId, short sIdent);
	short GetIdentBySocketId(unsigned __int64 socketId);

	bool SetRegNeedCountBySocketId(unsigned __int64 socketId, short sRegNeedCount);
	short GetRegNeedCountBySocketId(unsigned __int64 socketId);

	bool SetStrTmpDataBySocketId(unsigned __int64 socketId, string strTmpData);
	string GetStrTmpDataBySocketId(unsigned __int64 socketId);

	bool SetPerHandleDataBySocketId(unsigned __int64 socketId, PER_HANDLE_DATA* PerHandleData);
	PER_HANDLE_DATA* GetPerHandleDataBySocketId(unsigned __int64 socketId);

	bool SetPerIoDataBySocketId(unsigned __int64 socketId, LPPER_IO_OPERATION_DATA PerIoData);
	LPPER_IO_OPERATION_DATA GetPerIoDataBySocketId(unsigned __int64 socketId);

	void GetDefaultAuthorityByIdent(IdentType Ident, vector<OperPermission>& vecOper);
	void GetAuthorityByStrAuthority(string strAuthority, vector<OperPermission>& vecOper, unsigned char bType, string strSpl="|"); //通过权限字符串，根据bType获取对象对象的权限vector；bType 1现有的  2可增加的

protected:
	void initAllOperPer();

	//所有权限集合
	map<OperPermission, string> m_mAllOperPer;

private:
	map<unsigned __int64, UserInfo> m_mUserInfo;
};

#endif