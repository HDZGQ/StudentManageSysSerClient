#include "MsgHandle.h"
#include "LockTools.h"
#include "ProcMgr.h"
#include "MysqlMgr.h"


DWORD WINAPI MsgQueueThread(LPVOID IpParameter)
{
	while (1)
	{
		//������Ϣ���д���
		MsgHandleMgr::GetInstance()->MsgQueueHandle();

		//���ݿ���Ϣ���д���
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
	//���ȿ���Ϊ0��������Ϣ����Ϊ�ա���Ϊ��Щ��Ϣû�����ݣ�ֻҪ�յ����֪ͨ����
	if (NULL == RecvMsg && 0 != DataLen)
	{
		printf("���յ���ϢΪ�գ����ǳ��Ȳ�Ϊ0 DataLen[%u]\n", DataLen);
	}
	if (DataLen > DefaultRecvMSGLen) 
	{
		printf("���յ���Ϣ����[%u]�� ���Ϊ[%u]\n", DataLen, DefaultRecvMSGLen);
		return;
	}
	if (AssistId <= ASSIST_ID_START || AssistId >= ASSIST_ID_END)
	{
		printf("AssistId[%d] ����\n", AssistId);
		return;
	}

	//��ʱ�����ƶ������������Ҫ���ƣ�ֻ�ܰ�֮ǰѹ�����ɾ��
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
	* �����ִ���ʽ��
	* 1.�Ȱ���������Ū�����и�����Ȼ�����ԭ���С�ֻ�Ը������������Ӳ���һֱռ����Դ������С��
	* 2.�ڶ��־��ǣ��Ӷ���ȡһ�����ݣ������ɾ�������������󣬵���һ��ֻ����һ�Σ���������󣬻�һֱռ����Դ���ߴ�����
	*/

	//ÿ�δ���һ��
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