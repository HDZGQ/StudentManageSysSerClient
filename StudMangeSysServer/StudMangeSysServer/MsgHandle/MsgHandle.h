#ifndef __MSGHANDLE_H__
#define __MSGHANDLE_H__

#include <queue>
#include "NetDef.h" //含WinSock2.h，所以在windows.h头文件之前包含
#include <Windows.h> 
#include "xSingleton.h"
#include "ProtoDef.h"

//消息队列处理线程
DWORD WINAPI MsgQueueThread(LPVOID IpParameter); 

class MsgHandleMgr : public xSingleton<MsgHandleMgr>
{
public:
	MsgHandleMgr();
	~MsgHandleMgr();

	void InputMsgQueue(unsigned __int64 SocketId, char* RecvMsg, unsigned int DataLen, Assist_ID AssistId);

	void MsgQueueHandle();

private:
	queue<RecvMsgData> m_RecvMsgQueue;
};


#endif