#ifndef __USERINFOMGR_H__
#define __USERINFOMGR_H__

#include <vector>
#include "xSingleton.h"
#include "ProtoDef.h"

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

	bool CheckOperPerValid(OperPermission OperPer);
	bool SetVOperPer(string str);
private:
	string sName;
	string sAccount;
	unsigned int iUserId;
	short sIdent;
	short sSex;

	vector<OperPermission> m_vOperPer;
};

#endif