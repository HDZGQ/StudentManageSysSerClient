#include "ServerMgr.h"
#include "ProcMgr.h"
#include "MsgHandle.h"
#include "MysqlMgr.h"
#include "NetSysMgr.h"

ServerMgr::ServerMgr()
{

}

ServerMgr::~ServerMgr()
{

}

void ServerMgr::ServerRun()
{
	//初始化监控器。把功能类的接收处理函数和数据库返回结果后的处理函数压入到监控器
	ProcMgr::GetInstance()->InitMoniter();


	//开启消息队列处理线程。主要处理tcp接收的消息队列和mysql的消息队列
	HANDLE msgQueueThread = CreateThread(NULL, 0, MsgQueueThread, NULL, 0, NULL);
	CloseHandle(msgQueueThread);

	//连接数据库
	if (!MysqlMgr::GetInstance()->MysqlConn())
	{
		system("pause");
		exit(0);
	}

	//初始化socket等相关接口和数据结构。主线程监控客户端连接，工作线程处理系统接收好的消息
	NetSysMgr::GetInstance()->StartNet();
}