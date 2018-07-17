#include "SelectScoreSysProc.h"

SelectScoreSysProc::SelectScoreSysProc(ProcDef nProcDef) : BaseProc(nProcDef)
{
	initMapChoose();
}

SelectScoreSysProc::~SelectScoreSysProc()
{

}

bool SelectScoreSysProc::initMapChoose()
{
	m_mChoose.insert(pair<int, ChooseData>(GetMaxRealChoose(), ChooseData("单科批量查询成绩", OPER_PER_SELECTBATCHSCOREBYONESUBJECT, PROC_DEF_SELECTSCORESYSPROC)));
	m_mChoose.insert(pair<int, ChooseData>(++GetMaxRealChoose(), ChooseData("现有所有科目批量查询成绩", OPER_PER_SELECTBATCHSCOREBYSUBJECTS, PROC_DEF_SELECTSCORESYSPROC)));
	m_mChoose.insert(pair<int, ChooseData>(++GetMaxRealChoose(), ChooseData("单科单条查询成绩", OPER_PER_SELECTSINGLESCOREBYONESUBJECT, PROC_DEF_SELECTSCORESYSPROC)));
	m_mChoose.insert(pair<int, ChooseData>(++GetMaxRealChoose(), ChooseData("现有所有科目单条查询成绩", OPER_PER_SELECTSINGLESCOREBYSUBJECTS, PROC_DEF_SELECTSCORESYSPROC)));
	m_mChoose.insert(pair<int, ChooseData>(++GetMaxRealChoose(), ChooseData("返回", OPER_PER_INVALID, PROC_DEF_SCORESYSPROC)));

	return true;
}

void SelectScoreSysProc::EndProc()
{
	__super::EndProc();
}

void SelectScoreSysProc::StartRecv(void* vpData, unsigned int DataLen, /*int iMianId,*/ int iAssistId)
{

}

void SelectScoreSysProc::EndRecv()
{
	__super::EndRecv();
}

void SelectScoreSysProc::SwitchToOper(OperPermission CurOper)
{
	__super::SwitchToOper(CurOper);
}
