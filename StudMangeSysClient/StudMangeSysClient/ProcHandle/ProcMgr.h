#ifndef __PROCMGR_H__
#define __PROCMGR_H__

#include <vector>
#include <map>
#include "xSingleton.h"
#include "EnterSysProc.h"
#include "CommonSysProc.h"
#include "ScoreSysProc.h"
#include "UserInfoSysProc.h"
#include "AuthoritySysProc.h"
#include "AddScoreSysProc.h"
#include "UpdateScoreSysProc.h"
#include "SelectScoreSysProc.h"
#include "DeleteScoreSysProc.h"
#include "AlterSubjectsSysProc.h"
#include "AddUserInfoSysProc.h"
#include "UpdateUserInfoSysProc.h"
#include "SelectUserInfoSysProc.h"
#include "DeleteUserInfoSysProc.h"


class ProcMgr : public xSingleton<ProcMgr>
{
public:
	ProcMgr();
	~ProcMgr();

	bool initProc();

	bool ProcSwitch(ProcDef nProcDef, bool bChooseAgain = false);

	bool Recv(void* vpData, unsigned int DataLen, /*int iMianId,*/ int iAssistId);

	bool Notify();

protected:
	map<ProcDef, BaseProc*> m_mProcMgr;

private:
	EnterSysProc m_EnterSysProc;
	CommonSysProc m_CommonSysProc;
	ScoreSysProc m_ScoreSysProc;
	UserInfoSysProc m_UserInfoSysProc;
	AuthoritySysProc m_AuthoritySysProc;
	AddScoreSysProc m_AddScoreSysProc;
	UpdateScoreSysProc m_UpdateScoreSysProc;
	SelectScoreSysProc m_SelectScoreSysProc;
	DeleteScoreSysProc m_DeleteScoreSysProc;
	AlterSubjectsSysProc m_AlterSubjectsSysProc;
	AddUserInfoSysProc m_AddUserInfoSysProc;
	UpdateUserInfoSysProc m_UpdateUserInfoSysProc;
	SelectUserInfoSysProc m_SelectUserInfoSysProc;
	DeleteUserInfoSysProc m_DeleteUserInfoSysProc;
};
#endif