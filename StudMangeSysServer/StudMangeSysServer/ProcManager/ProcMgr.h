#ifndef __PROCMGR_H__
#define __PROCMGR_H__

#include "xSingleton.h"
#include "EventMonitor.h"
#include "SubjectsMgr.h"
#include "EnterSysProc.h"
#include "ScoreProc.h"
#include "PublicDef.h"
//#include "NetDef.h"

class ProcMgr : public xSingleton<ProcMgr>
{
public:
	ProcMgr();
	~ProcMgr();

	bool InitMoniter();

	CEventProxy<Assist_ID, SOCKET, void*, unsigned int>& GetRecvHandleMoniter();
	CEventProxy<Assist_ID, SOCKET, MYSQL_RES*, string>& GetReplyHandleMoniter();

private:
	CEventProxy<Assist_ID, SOCKET, void*, unsigned int> m_RecvHandleMoniter;
	CEventProxy<Assist_ID, SOCKET, MYSQL_RES*, string> m_ReplyHandleMoniter;

	EnterSysProc m_EnterSysProc;
	ScoreProc m_ScoreProc;
};



#endif