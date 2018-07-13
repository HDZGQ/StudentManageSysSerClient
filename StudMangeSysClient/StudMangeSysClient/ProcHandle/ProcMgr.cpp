#include "ProcMgr.h"
#include "CheckTool.h"

ProcMgr::ProcMgr() : 
		m_EnterSysProc(PROC_DEF_ENTERSYSPROC),
		m_CommonSysProc(PROC_DEF_COMMONSYSPROC)
{
	m_vOperPer.push_back(OPER_PER_LOGIN);
	m_vOperPer.push_back(OPER_PER_REGISTER);
}

ProcMgr::~ProcMgr()
{
}

bool ProcMgr::initProc()
{
	m_mProcMgr.insert(pair<ProcDef, BaseProc*>(PROC_DEF_ENTERSYSPROC, &m_EnterSysProc));
	m_mProcMgr.insert(pair<ProcDef, BaseProc*>(PROC_DEF_COMMONSYSPROC, &m_CommonSysProc));

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
}

bool ProcMgr::CheckOperPerValid(OperPermission OperPer)
{
	vector<OperPermission>::iterator iter = m_vOperPer.begin();
	for (; iter != m_vOperPer.end(); iter++)
	{
		if (OperPer == *iter)
			return true;
	}

	return false;
}

bool ProcMgr::SetVOperPer(string str)
{
	if (str.empty())
		return false;

	vector<string> vecStr = CheckTool::Splite(str, "|");

	vector<OperPermission> vOperPerTmp;
	for (vector<string>::iterator iter = vecStr.begin();iter != vecStr.end(); iter++)
	{
		if (iter->empty() || !CheckTool::CheckStringByValid(*iter, "0~9"))
			return false;
		
		int iValue = atoi(iter->c_str());
		vOperPerTmp.push_back((OperPermission)iValue);
	}

	m_vOperPer.insert(m_vOperPer.end(), vOperPerTmp.begin(), vOperPerTmp.end());

	return true;
}