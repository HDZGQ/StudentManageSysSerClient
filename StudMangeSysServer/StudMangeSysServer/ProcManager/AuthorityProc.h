#ifndef __AUTHORITBYPROC_H__
#define __AUTHORITBYPROC_H__

#include <string>
#include <WinSock2.h>
#include <mysql.h>
#include <Windows.h>
#include "ProtoDef.h"

using namespace std;

class AuthorityProc
{
public:
	AuthorityProc();
	~AuthorityProc();

	/*
	* 接收消息处理
	*/
	void GetAuthoritRecvHandle(SOCKET SocketId, void* vpData, unsigned int DataLen); //获取现有或可增加权限请求
	void EditAuthoritRecvHandle(SOCKET SocketId, void* vpData, unsigned int DataLen); //增删权限请求

	/*
	* 回复消息处理。strRecord需要从前面操作中带到返回函数的数据
	*/
	void GetAuthoritReplyHandle(SOCKET SocketId, MYSQL_RES *MysqlRes, string strRecord); //获取现有或可增加权限回复
	void EditAuthoritReplyHandle(SOCKET SocketId, MYSQL_RES *MysqlRes, string strRecord); //增删权限回复  --对象要重登录才有效
};

#endif