#include "AlterSubjectsSysProc.h"

AlterSubjectsSysProc::AlterSubjectsSysProc(ProcDef nProcDef) : BaseProc(nProcDef)
{
	initMapChoose();
}

AlterSubjectsSysProc::~AlterSubjectsSysProc()
{

}

bool AlterSubjectsSysProc::initMapChoose()
{
	m_mChoose.insert(pair<int, ChooseData>(GetMaxRealChoose(), ChooseData("增加成绩科目", OPER_PER_ALTERADDONESCORESUBJECT, PROC_DEF_ALTERSUBJECTSSYSPROC)));
	m_mChoose.insert(pair<int, ChooseData>(++GetMaxRealChoose(), ChooseData("删除成绩科目", OPER_PER_ALTERDELETEONESCORESUBJECT, PROC_DEF_ALTERSUBJECTSSYSPROC)));
	m_mChoose.insert(pair<int, ChooseData>(++GetMaxRealChoose(), ChooseData("返回", OPER_PER_INVALID, PROC_DEF_SCORESYSPROC)));

	return true;
}

void AlterSubjectsSysProc::EndProc()
{
	__super::EndProc();
}

void AlterSubjectsSysProc::StartRecv(void* vpData, unsigned int DataLen, /*int iMianId,*/ int iAssistId)
{

}

void AlterSubjectsSysProc::EndRecv()
{
	__super::EndRecv();
}

void AlterSubjectsSysProc::SwitchToOper(OperPermission CurOper)
{
	__super::SwitchToOper(CurOper);
}
