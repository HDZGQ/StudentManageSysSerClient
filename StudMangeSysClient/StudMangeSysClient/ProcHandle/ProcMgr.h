#ifndef __PROCMGR_H__
#define __PROCMGR_H__

#include <vector>
#include <map>
#include "xSingleton.h"
#include "EnterSysProc.h"
#include "CommonSysProc.h"


class ProcMgr : public xSingleton<ProcMgr>
{
public:
	ProcMgr();
	~ProcMgr();

	bool initProc();

	bool ProcSwitch(ProcDef nProcDef, bool bChooseAgain = false);

	bool Recv(void* vpData, unsigned int DataLen, /*int iMianId,*/ int iAssistId);

	bool Notify();

	bool CheckOperPerValid(OperPermission OperPer);
	bool SetVOperPer(string str);
protected:
	map<ProcDef, BaseProc*> m_mProcMgr;

	vector<OperPermission> m_vOperPer;
private:
	EnterSysProc m_EnterSysProc;
	CommonSysProc m_CommonSysProc;
};
#endif