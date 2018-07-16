#ifndef __PROCMGR_H__
#define __PROCMGR_H__

#include "xSingleton.h"
#include "EventMonitor.h"
#include "EnterSysProc.h"
#include "PublicDef.h"
//#include "NetDef.h"

class ProcMgr : public xSingleton<ProcMgr>
{
public:
	ProcMgr();
	~ProcMgr();

	bool InitMoniter();

	CEventProxy<Assist_ID, SOCKET, void*, unsigned int>& GetRecvHandleMoniter();
	CEventProxy<OperPermission, SOCKET, MYSQL_RES*, int>& GetReplyHandleMoniter();

private:
	CEventProxy<Assist_ID, SOCKET, void*, unsigned int> m_RecvHandleMoniter;
	CEventProxy<OperPermission, SOCKET, MYSQL_RES*, int> m_ReplyHandleMoniter;

	EnterSysProc m_EnterSysProc;
};



#endif