#ifndef __ENTERSYSPROC_H__
#define __ENTERSYSPROC_H__


#include <WinSock2.h>
#include <mysql.h>
#include <Windows.h>
#include "ProtoDef.h"



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

	/*
	* 回复消息处理。iRes为0为正常执行sql语句
	*/
	void LoginReplyHandle(SOCKET SocketId, MYSQL_RES *MysqlRes, int iRes);
	void RegisterReplyHandle(SOCKET SocketId, MYSQL_RES *MysqlRes, int iRes);
};

#endif