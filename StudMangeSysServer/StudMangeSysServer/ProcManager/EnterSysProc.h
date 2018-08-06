#ifndef __ENTERSYSPROC_H__
#define __ENTERSYSPROC_H__

#include <string>
#include <WinSock2.h>
#include <mysql.h>
#include <Windows.h>
#include "ProtoDef.h"

using namespace std;

class EnterSysProc
{
public:
	EnterSysProc();
	~EnterSysProc();

	/*
	* 接收消息处理
	*/
	void LoginRecvHandle(SOCKET SocketId, void* vpData, unsigned int DataLen);
	void RegisterRecvHandle(SOCKET SocketId, void* vpData, unsigned int DataLen);
	void ExitSysRecvHandle(SOCKET SocketId, void* vpData, unsigned int DataLen); //只是用户不在系统内，但是长连接还在

	/*
	* 回复消息处理。strRecord需要从前面操作中带到返回函数的数据
	*/
	void LoginReplyHandle(SOCKET SocketId, MYSQL_RES *MysqlRes, string strRecord);
	void RegisterReplyHandle(SOCKET SocketId, MYSQL_RES *MysqlRes, string strRecord);
};

#endif