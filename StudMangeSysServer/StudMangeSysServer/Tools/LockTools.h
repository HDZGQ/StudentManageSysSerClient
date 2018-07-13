#ifndef __LOCKTOOLS_H__
#define __LOCKTOOLS_H__

#include <Windows.h> 
#include "xSingleton.h"

class BaseLock
{
public:
	BaseLock()
	{
		InitializeCriticalSection(&_critical);
	}

	~BaseLock()
	{
		DeleteCriticalSection(&_critical);
	}

	void Lock()
	{
		EnterCriticalSection(&_critical);
	}

	void Unlock()
	{
		LeaveCriticalSection(&_critical);
	}

protected:
	CRITICAL_SECTION  _critical;
};

class UserInfoLock : public BaseLock, public xSingleton<UserInfoLock>
{
public:
	UserInfoLock(){}
	~UserInfoLock(){}
};

class RecvMsgLock : public BaseLock, public xSingleton<RecvMsgLock>
{
public:
	RecvMsgLock(){}
	~RecvMsgLock(){}
};

class MysqlMsgLock : public BaseLock, public xSingleton<MysqlMsgLock>
{
public:
	MysqlMsgLock(){}
	~MysqlMsgLock(){}
};

#endif