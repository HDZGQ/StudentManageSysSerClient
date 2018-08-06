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

	//用户性接口，存在同步
public:
	bool InsertInfo(unsigned __int64 socketId, UserInfo userInfo);
	bool InsertInfo(unsigned __int64 socketId, string strIP, unsigned short sPort, PER_HANDLE_DATA* PerHandleData);

	bool RemoveInfoBySocketId(unsigned __int64 socketId);
	bool RemoveInfoByUserId(unsigned int iUserId);

	bool InitNotNetDataBySocketId(unsigned __int64 socketId); //初始化非网络数据

	bool SetUserIdBySocketId(unsigned __int64 socketId, unsigned int iUserId);
	unsigned int GetUserIdBySocketId(unsigned __int64 socketId);

	unsigned __int64 GetSocketIdByAccount(string strAccount);

	bool SetAuthorityBySocketId(unsigned __int64 socketId, string strAuthority);
	string GetAuthorityBySocketId(unsigned __int64 socketId);

	bool SetAuthorityByAccount(string strAccount, string strAuthority);
	string GetAuthorityByAccount(string strAccount);

	bool SetNewAccountByAccount(string strAccount, string strNewAccount);

	bool FindAccount(string strAccount); //查找该账号

	bool IsHaveOneAuthorityBySocketId(unsigned __int64 socketId, OperPermission OperPer);

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

	//公共性接口，不存在同步
public:
	void GetDefaultAuthorityByIdent(IdentType Ident, vector<OperPermission>& vecOper); //根据身份标识获取默认权限
	void GetAuthorityByStrAuthority(string strAuthority, vector<OperPermission>& vecOper, unsigned char bType, string strSpl="|"); //通过权限字符串，根据bType获取对象对象的权限vector；bType 1现有的  2可增加的

	string GetEnglishUserInfoFieldByField(int iFeildId);
	string GetChineseUserInfoFieldByField(int iFeildId);
	bool GetCanUpdateField(short iMyIdent, string strMyAccount, short iObjIdent, string strObjAccount, bool* pCanUpdate, int iCanUpdateMaxCount); //根据权限，获取操作用户可以更改对象用户的字段

	//非动态数据
protected:
	void initAllOperPer();
	void InitUserInfoFieldName(); //1-account 2-password 3-name 4-sex 5-Ident 6-major 7-grade

	//所有权限集合
	map<OperPermission, string> m_mAllOperPer;

	//用户信息字段
	map<int, string> m_mUserInfoFieldName;
	int m_maxUserInfoFieldID;

	//动态数据，存在同步
private:
	map<unsigned __int64, UserInfo> m_mUserInfo;
};

#endif