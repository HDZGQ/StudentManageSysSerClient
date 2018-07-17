#include "DeleteScoreSysProc.h"

DeleteScoreSysProc::DeleteScoreSysProc(ProcDef nProcDef) : BaseProc(nProcDef)
{
	initMapChoose();
}

DeleteScoreSysProc::~DeleteScoreSysProc()
{

}

bool DeleteScoreSysProc::initMapChoose()
{
	m_mChoose.insert(pair<int, ChooseData>(GetMaxRealChoose(), ChooseData("批量删除成绩", OPER_PER_DELETEBATCHSCORE, PROC_DEF_DELETESCORESYSPROC)));
	m_mChoose.insert(pair<int, ChooseData>(++GetMaxRealChoose(), ChooseData("单条删除成绩", OPER_PER_DELETESINGLESCORE, PROC_DEF_DELETESCORESYSPROC)));
	m_mChoose.insert(pair<int, ChooseData>(++GetMaxRealChoose(), ChooseData("返回", OPER_PER_INVALID, PROC_DEF_SCORESYSPROC)));

	return true;
}

void DeleteScoreSysProc::EndProc()
{
	__super::EndProc();
}

void DeleteScoreSysProc::StartRecv(void* vpData, unsigned int DataLen, /*int iMianId,*/ int iAssistId)
{

}

void DeleteScoreSysProc::EndRecv()
{
	__super::EndRecv();
}

void DeleteScoreSysProc::SwitchToOper(OperPermission CurOper)
{
	__super::SwitchToOper(CurOper);
}
