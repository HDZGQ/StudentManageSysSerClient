#include "MsgHandle.h"
#include "LockTools.h"
#include "ProcMgr.h"
#include "MysqlMgr.h"


DWORD WINAPI MsgQueueThread(LPVOID IpParameter)
{
	while (1)
	{
		//接收消息队列处理
		MsgHandleMgr::GetInstance()->MsgQueueHandle();

		//数据库消息队列处理
		MysqlMgr::GetInstance()->MsgQueueHandle();

		Sleep(100);
	}

	return 0;
}

MsgHandleMgr::MsgHandleMgr()
{
	InitializeCriticalSection(&_critical);
}

MsgHandleMgr::~MsgHandleMgr()
{
	DeleteCriticalSection(&_critical);
}

void MsgHandleMgr::InputMsgQueue(unsigned __int64 SocketId, char* RecvMsg, unsigned int DataLen, Assist_ID AssistId)
{
	//长度可以为0，但是消息必须为空。因为有些消息没有数据，只要收到这个通知即可
	if (NULL == RecvMsg && 0 != DataLen)
	{
		printf("接收的消息为空，但是长度不为0 DataLen[%u]\n", DataLen);
	}
	if (DataLen > DefaultRecvMSGLen) 
	{
		printf("接收的消息过大[%u]， 最大为[%u]\n", DataLen, DefaultRecvMSGLen);
		return;
	}
	if (AssistId <= ASSIST_ID_START || AssistId >= ASSIST_ID_END)
	{
		printf("AssistId[%d] 错误\n", AssistId);
		return;
	}

	//暂时不限制队列数量，如果要限制，只能把之前压入的先删除
	RecvMsgData recvMsg;
	recvMsg.SocketId =  SocketId;
	memcpy(recvMsg.RecvMsg, RecvMsg, DataLen);
	recvMsg.DataLen = DataLen;
	recvMsg.AssistID = AssistId;

	RecvMsgLock uLock(_critical);
	//RecvMsgLock::GetInstance()->Lock();
	m_RecvMsgQueue.push(recvMsg);
	//RecvMsgLock::GetInstance()->Unlock();
}

void MsgHandleMgr::MsgQueueHandle()
{
	/*
	* 有两种处理方式：
	* 1.先把整个队列弄个队列副本，然后清空原队列。只对副本处理，这样子不好一直占着资源，降低小弟
	* 2.第二种就是，从队列取一个数据，用完就删掉，很生动形象，但是一次只处理一次，如果并发大，会一直占着资源或者处理慢
	*/

	//每次处理一次
	if(!m_RecvMsgQueue.empty())
	{
		RecvMsgData recvMsgData = m_RecvMsgQueue.front();
		ProcMgr::GetInstance()->GetRecvHandleMoniter().DispatchEvent((Assist_ID)recvMsgData.AssistID, (SOCKET)recvMsgData.SocketId, recvMsgData.RecvMsg, recvMsgData.DataLen);

		RecvMsgLock uLock(_critical);
		//RecvMsgLock::GetInstance()->Lock();
		m_RecvMsgQueue.pop();
		//RecvMsgLock::GetInstance()->Unlock();
	}
	
}