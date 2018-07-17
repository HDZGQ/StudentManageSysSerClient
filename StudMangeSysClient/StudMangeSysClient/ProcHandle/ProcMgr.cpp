#include "ProcMgr.h"


ProcMgr::ProcMgr() : 
		m_EnterSysProc(PROC_DEF_ENTERSYSPROC),
		m_CommonSysProc(PROC_DEF_COMMONSYSPROC),
		m_ScoreSysProc(PROC_DEF_SCORESYSPROC),
		m_UserInfoSysProc(PROC_DEF_USERINFOSYSPROC),
		m_AuthoritySysProc(PROC_DEF_AUTHORITYSYSPROC),
		m_AddScoreSysProc(PROC_DEF_ADDSCORESYSPROC),
		m_UpdateScoreSysProc(PROC_DEF_UPDATESCORESYSPROC),
		m_SelectScoreSysProc(PROC_DEF_SELECTSCORESYSPROC),
		m_DeleteScoreSysProc(PROC_DEF_DELETESCORESYSPROC),
		m_AlterSubjectsSysProc(PROC_DEF_ALTERSUBJECTSSYSPROC)
{
	
}

ProcMgr::~ProcMgr()
{
}

bool ProcMgr::initProc()
{
	m_mProcMgr.insert(pair<ProcDef, BaseProc*>(PROC_DEF_ENTERSYSPROC, &m_EnterSysProc));
	m_mProcMgr.insert(pair<ProcDef, BaseProc*>(PROC_DEF_COMMONSYSPROC, &m_CommonSysProc));
	m_mProcMgr.insert(pair<ProcDef, BaseProc*>(PROC_DEF_SCORESYSPROC, &m_ScoreSysProc));
	m_mProcMgr.insert(pair<ProcDef, BaseProc*>(PROC_DEF_USERINFOSYSPROC, &m_UserInfoSysProc));
	m_mProcMgr.insert(pair<ProcDef, BaseProc*>(PROC_DEF_AUTHORITYSYSPROC, &m_AuthoritySysProc));
	m_mProcMgr.insert(pair<ProcDef, BaseProc*>(PROC_DEF_ADDSCORESYSPROC, &m_AddScoreSysProc));
	m_mProcMgr.insert(pair<ProcDef, BaseProc*>(PROC_DEF_UPDATESCORESYSPROC, &m_UpdateScoreSysProc));
	m_mProcMgr.insert(pair<ProcDef, BaseProc*>(PROC_DEF_SELECTSCORESYSPROC, &m_SelectScoreSysProc));
	m_mProcMgr.insert(pair<ProcDef, BaseProc*>(PROC_DEF_DELETESCORESYSPROC, &m_DeleteScoreSysProc));
	m_mProcMgr.insert(pair<ProcDef, BaseProc*>(PROC_DEF_ALTERSUBJECTSSYSPROC, &m_AlterSubjectsSysProc));

	return true;
}

bool ProcMgr::ProcSwitch(ProcDef nProcDef, bool bChooseAgain)
{
	if (nProcDef <= PROC_DEF_START || nProcDef >= PROC_DEF_END)
		return false;

	m_mProcMgr[nProcDef]->OnStart(bChooseAgain);

	return true;
}

bool ProcMgr::Recv(void* vpData, unsigned int DataLen, /*int iMianId,*/ int iAssistId)
{
	map<ProcDef, BaseProc*>::iterator iter = m_mProcMgr.begin();
	for (; iter != m_mProcMgr.end(); iter++)
	{
		if (iter->second->IsRunning())
		{
			iter->second->StartRecv(vpData, DataLen, /*iMianId,*/ iAssistId);
		}
	}

	return true;
}

bool ProcMgr::Notify()
{
	while (true)
	{
		map<ProcDef, BaseProc*>::iterator iter = m_mProcMgr.begin();
		for (; iter != m_mProcMgr.end(); iter++)
		{
			if (iter->second->IsShow())
			{
				iter->second->StartProc();
			}
		}
	}

	return true;
}

