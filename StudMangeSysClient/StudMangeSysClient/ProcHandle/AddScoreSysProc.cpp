#include "AddScoreSysProc.h"

AddScoreSysProc::AddScoreSysProc(ProcDef nProcDef) : BaseProc(nProcDef)
{
	initMapChoose();
}

AddScoreSysProc::~AddScoreSysProc()
{

}


bool AddScoreSysProc::initMapChoose()
{
	m_mChoose.insert(pair<int, ChooseData>(GetMaxRealChoose(), ChooseData("单科批量增加成绩", OPER_PER_ADDBATCHSCOREBYONESUBJECT, PROC_DEF_ADDSCORESYSPROC)));
	m_mChoose.insert(pair<int, ChooseData>(++GetMaxRealChoose(), ChooseData("现有所有科目批量增加成绩", OPER_PER_ADDBATCHSCOREBYSUBJECTS, PROC_DEF_ADDSCORESYSPROC)));
	m_mChoose.insert(pair<int, ChooseData>(++GetMaxRealChoose(), ChooseData("单科单条增加成绩", OPER_PER_ADDSINGLESCOREBYONESUBJECT, PROC_DEF_ADDSCORESYSPROC)));
	m_mChoose.insert(pair<int, ChooseData>(++GetMaxRealChoose(), ChooseData("现有所有科目单条增加成绩", OPER_PER_ADDSINGLESCOREBYSUBJECTS, PROC_DEF_ADDSCORESYSPROC)));
	m_mChoose.insert(pair<int, ChooseData>(++GetMaxRealChoose(), ChooseData("返回", OPER_PER_INVALID, PROC_DEF_SCORESYSPROC)));

	return true;
}

void AddScoreSysProc::EndProc()
{
	__super::EndProc();
}

void AddScoreSysProc::StartRecv(void* vpData, unsigned int DataLen, /*int iMianId,*/ int iAssistId)
{

}

void AddScoreSysProc::EndRecv()
{
	__super::EndRecv();
}

void AddScoreSysProc::SwitchToOper(OperPermission CurOper)
{
	__super::SwitchToOper(CurOper);
}
