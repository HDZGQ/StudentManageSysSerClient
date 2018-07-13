#include "CommonSysProc.h"
#include "ProcMgr.h"
#include "CheckTool.h"
#include "TCPHandle.h"

CommonSysProc::CommonSysProc(ProcDef nProcDef) : BaseProc(nProcDef)
{
	initMapChoose();
}

CommonSysProc::~CommonSysProc()
{

}

bool CommonSysProc::initMapChoose()
{
	m_mChoose.insert(pair<int, ChooseData>(GetMaxRealChoose(), ChooseData("成绩", OPER_PER_INVALID, PROC_DEF_SCORESYSPROC)));
	m_mChoose.insert(pair<int, ChooseData>(++GetMaxRealChoose(), ChooseData("用户信息", OPER_PER_INVALID, PROC_DEF_COMMONSYSPROC)));
	m_mChoose.insert(pair<int, ChooseData>(++GetMaxRealChoose(), ChooseData("返回", OPER_PER_INVALID, PROC_DEF_USERINFOSYSPROC)));

	return true;
}

void CommonSysProc::StartProc()
{
	__super::StartProc();
}

void CommonSysProc::EndProc()
{
	__super::EndProc();
}

void CommonSysProc::StartRecv(void* vpData, unsigned int DataLen, /*int iMianId,*/ int iAssistId)
{

}

void CommonSysProc::EndRecv()
{
	__super::EndRecv();
}

void CommonSysProc::SwitchToOper(OperPermission CurOper)
{

}