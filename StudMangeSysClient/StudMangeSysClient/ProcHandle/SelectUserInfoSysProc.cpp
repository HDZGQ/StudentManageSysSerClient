#include "SelectUserInfoSysProc.h"

SelectUserInfoSysProc::SelectUserInfoSysProc(ProcDef nProcDef) : BaseProc(nProcDef)
{
	initMapChoose();
}

SelectUserInfoSysProc::~SelectUserInfoSysProc()
{

}

bool SelectUserInfoSysProc::initMapChoose()
{
	m_mChoose.insert(pair<int, ChooseData>(GetMaxRealChoose(), ChooseData("批量查询用户信息", OPER_PER_SELECTBATCHUSERINFO, PROC_DEF_SELECTUSERINFOSYSPROC)));
	m_mChoose.insert(pair<int, ChooseData>(++GetMaxRealChoose(), ChooseData("单条查询用户信息", OPER_PER_SELECTSINGLEUSERINFO, PROC_DEF_SELECTUSERINFOSYSPROC)));
	m_mChoose.insert(pair<int, ChooseData>(++GetMaxRealChoose(), ChooseData("返回", OPER_PER_INVALID, PROC_DEF_USERINFOSYSPROC)));

	return true;
}

void SelectUserInfoSysProc::EndProc()
{
	__super::EndProc();
}

void SelectUserInfoSysProc::StartRecv(void* vpData, unsigned int DataLen, /*int iMianId,*/ int iAssistId)
{

}

void SelectUserInfoSysProc::EndRecv()
{
	__super::EndRecv();
}

void SelectUserInfoSysProc::SwitchToOper(OperPermission CurOper)
{
	__super::SwitchToOper(CurOper);
}
