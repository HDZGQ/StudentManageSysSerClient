#ifndef __USERINFOPROC_H__
#define __USERINFOPROC_H__

#include <string>
#include <WinSock2.h>
#include <mysql.h>
#include <Windows.h>
#include "ProtoDef.h"

using namespace std;

class UserInfoProc
{
public:
	UserInfoProc();
	~UserInfoProc();

	/*
	* 接收消息处理
	*/
	void AddSingleUserInfoRecvHandle(SOCKET SocketId, void* vpData, unsigned int DataLen); //单条增加用户信息请求
	void AddBatchUserInfoRecvHandle(SOCKET SocketId, void* vpData, unsigned int DataLen); //批量增加用户信息请求

	/*
	* 回复消息处理。strRecord需要从前面操作中带到返回函数的数据
	*/
	void AddSingleUserInfoReplyHandle(SOCKET SocketId, MYSQL_RES *MysqlRes, string strRecord); //单条增加用户信息回复
	void AddBatchUserInfoReplyHandle(SOCKET SocketId, MYSQL_RES *MysqlRes, string strRecord); //批量增加用户信息回复
};

#endif