#ifndef __LOCKTOOLS_H__
#define __LOCKTOOLS_H__

#include <Windows.h> 
#include "xSingleton.h"

class BaseLock
{
public:
	BaseLock(CRITICAL_SECTION& cs) : _critical(cs)
	{
		//InitializeCriticalSection(&_critical);
		EnterCriticalSection(&_critical);
	}

	~BaseLock()
	{
		//DeleteCriticalSection(&_critical);
		LeaveCriticalSection(&_critical);
	}
/*
	void Lock()
	{
		EnterCriticalSection(&_critical);
	}

	void Unlock()
	{
		LeaveCriticalSection(&_critical);
	}
*/
protected:
	CRITICAL_SECTION  _critical;
};

class UserInfoLock : public BaseLock/*, public xSingleton<UserInfoLock>*/
{
public:
	UserInfoLock(CRITICAL_SECTION& cs):BaseLock(cs){}
	~UserInfoLock(){}
};

class RecvMsgLock : public BaseLock/*, public xSingleton<RecvMsgLock>*/
{
public:
	RecvMsgLock(CRITICAL_SECTION& cs):BaseLock(cs){}
	~RecvMsgLock(){}
};

class MysqlMsgLock : public BaseLock/*, public xSingleton<MysqlMsgLock>*/
{
public:
	MysqlMsgLock(CRITICAL_SECTION& cs):BaseLock(cs){}
	~MysqlMsgLock(){}
};

class ExistSubjectsLock : public BaseLock/*, public xSingleton<ExistSubjectsLock>*/
{
public:
	ExistSubjectsLock(CRITICAL_SECTION& cs):BaseLock(cs){}
	~ExistSubjectsLock(){}
};

#endif