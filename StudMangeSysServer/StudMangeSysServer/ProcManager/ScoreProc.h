#ifndef __SCOREPROC_H__
#define __SCOREPROC_H__

#include <string>
#include <WinSock2.h>
#include <mysql.h>
#include <Windows.h>
#include "ProtoDef.h"

using namespace std;

class ScoreProc
{
public:
	ScoreProc();
	~ScoreProc();

	/*
	* 接收消息处理
	*/
	void CheckCanAlterSubjectRecvHandle(SOCKET SocketId, void* vpData, unsigned int DataLen);
	void AlterSubjectRecvHandle(SOCKET SocketId, void* vpData, unsigned int DataLen);


	/*
	* 回复消息处理。strRecord需要从前面操作中带到返回函数的数据
	*/
	void AlterSubjectReplyHandle(SOCKET SocketId, MYSQL_RES *MysqlRes, string strRecord);
};

#endif