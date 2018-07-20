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
	void CheckCanAlterSubjectRecvHandle(SOCKET SocketId, void* vpData, unsigned int DataLen); //获取科目
	void AlterSubjectRecvHandle(SOCKET SocketId, void* vpData, unsigned int DataLen); //接收增减科目请求
	void AddSingleScoreRecvHandle(SOCKET SocketId, void* vpData, unsigned int DataLen); //单条增加成绩（包括单科和全科）请求


	/*
	* 回复消息处理。strRecord需要从前面操作中带到返回函数的数据
	*/
	void AlterSubjectReplyHandle(SOCKET SocketId, MYSQL_RES *MysqlRes, string strRecord); //数据库处理后，返回增减科目结果
	void AddSingleScoreReplyHandle(SOCKET SocketId, MYSQL_RES *MysqlRes, string strRecord); //数据库处理后，返回单条增加成绩（包括单科和全科）结果
};

#endif