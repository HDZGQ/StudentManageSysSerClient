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
	void SelectSingleUserInfoRecvHandle(SOCKET SocketId, void* vpData, unsigned int DataLen); //单条查询用户信息请求
	void SelectBatchUserInfoRecvHandle(SOCKET SocketId, void* vpData, unsigned int DataLen); //批量查询用户信息请求
	void UpdateSingleUserInfoRecvHandle(SOCKET SocketId, void* vpData, unsigned int DataLen); //单条更新用户信息请求
	void DeleteUserInfoRecvHandle(SOCKET SocketId, void* vpData, unsigned int DataLen); //删除用户信息（包括单条和批量）请求

	/*
	* 回复消息处理。strRecord需要从前面操作中带到返回函数的数据
	*/
	void AddSingleUserInfoReplyHandle(SOCKET SocketId, MYSQL_RES *MysqlRes, string strRecord); //单条增加用户信息回复
	void AddBatchUserInfoReplyHandle(SOCKET SocketId, MYSQL_RES *MysqlRes, string strRecord); //批量增加用户信息回复
	void SelectSingleUserInfoReplyHandle(SOCKET SocketId, MYSQL_RES *MysqlRes, string strRecord); //单条查询用户信息回复
	void SelectBatchUserInfoReplyHandle(SOCKET SocketId, MYSQL_RES *MysqlRes, string strRecord); //批量查询用户信息回复
	void UpdateSingleUserInfoReplyHandle(SOCKET SocketId, MYSQL_RES *MysqlRes, string strRecord); //单条更新用户信息回复
	void DeleteUserInfoReplyHandle(SOCKET SocketId, MYSQL_RES *MysqlRes, string strRecord); ///删除用户信息（包括单条和批量）回复
};

#endif