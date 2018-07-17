#include "UpdateScoreSysProc.h"

UpdateScoreSysProc::UpdateScoreSysProc(ProcDef nProcDef) : BaseProc(nProcDef)
{
	initMapChoose();
}

UpdateScoreSysProc::~UpdateScoreSysProc()
{

}

bool UpdateScoreSysProc::initMapChoose()
{
	//m_mChoose.insert(pair<int, ChooseData>(GetMaxRealChoose(), ChooseData("单科批量更改成绩", OPER_PER_UPDATEBATCHSCOREBYONESUBJECT, PROC_DEF_UPDATESCORESYSPROC)));
	//m_mChoose.insert(pair<int, ChooseData>(++GetMaxRealChoose(), ChooseData("现有所有科目批量更改成绩", OPER_PER_UPDATEBATCHSCOREBYSUBJECTS, PROC_DEF_UPDATESCORESYSPROC)));
	m_mChoose.insert(pair<int, ChooseData>(GetMaxRealChoose(), ChooseData("单科单条更改成绩", OPER_PER_UPDATESINGLESCOREBYONESUBJECT, PROC_DEF_UPDATESCORESYSPROC)));
	m_mChoose.insert(pair<int, ChooseData>(++GetMaxRealChoose(), ChooseData("现有所有科目单条更改成绩", OPER_PER_UPDATESINGLESCOREBYSUBJECTS, PROC_DEF_UPDATESCORESYSPROC)));
	m_mChoose.insert(pair<int, ChooseData>(++GetMaxRealChoose(), ChooseData("返回", OPER_PER_INVALID, PROC_DEF_SCORESYSPROC)));

	return true;
}

void UpdateScoreSysProc::EndProc()
{
	__super::EndProc();
}

void UpdateScoreSysProc::StartRecv(void* vpData, unsigned int DataLen, /*int iMianId,*/ int iAssistId)
{

}

void UpdateScoreSysProc::EndRecv()
{
	__super::EndRecv();
}

void UpdateScoreSysProc::SwitchToOper(OperPermission CurOper)
{
	__super::SwitchToOper(CurOper);
}
