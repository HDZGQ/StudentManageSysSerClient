#include "ScoreSysProc.h"

ScoreSysProc::ScoreSysProc(ProcDef nProcDef) : BaseProc(nProcDef)
{
	initMapChoose();
}

ScoreSysProc::~ScoreSysProc()
{

}

bool ScoreSysProc::initMapChoose()
{
	m_mChoose.insert(pair<int, ChooseData>(GetMaxRealChoose(), ChooseData("增加成绩", OPER_PER_INVALID, PROC_DEF_ADDSCORESYSPROC)));
	m_mChoose.insert(pair<int, ChooseData>(++GetMaxRealChoose(), ChooseData("更改成绩", OPER_PER_INVALID, PROC_DEF_UPDATESCORESYSPROC)));
	m_mChoose.insert(pair<int, ChooseData>(++GetMaxRealChoose(), ChooseData("查询成绩", OPER_PER_INVALID, PROC_DEF_SELECTSCORESYSPROC)));
	m_mChoose.insert(pair<int, ChooseData>(++GetMaxRealChoose(), ChooseData("删除成绩", OPER_PER_INVALID, PROC_DEF_DELETESCORESYSPROC)));
	m_mChoose.insert(pair<int, ChooseData>(++GetMaxRealChoose(), ChooseData("增减科目", OPER_PER_INVALID, PROC_DEF_ALTERSUBJECTSSYSPROC)));
	m_mChoose.insert(pair<int, ChooseData>(++GetMaxRealChoose(), ChooseData("返回", OPER_PER_INVALID, PROC_DEF_COMMONSYSPROC)));

	return true;
}

void ScoreSysProc::EndProc()
{
	__super::EndProc();
}

void ScoreSysProc::StartRecv(void* vpData, unsigned int DataLen, /*int iMianId,*/ int iAssistId)
{

}
void ScoreSysProc::EndRecv()
{
	__super::EndRecv();
}

void ScoreSysProc::SwitchToOper(OperPermission CurOper)
{
	__super::SwitchToOper(CurOper);
}