#include "UpdateUserInfoSysProc.h"

UpdateUserInfoSysProc::UpdateUserInfoSysProc(ProcDef nProcDef) : BaseProc(nProcDef)
{
	initMapChoose();
}

UpdateUserInfoSysProc::~UpdateUserInfoSysProc()
{

}


bool UpdateUserInfoSysProc::initMapChoose()
{
	//m_mChoose.insert(pair<int, ChooseData>(GetMaxRealChoose(), ChooseData("单个字段批量更改用户信息", OPER_PER_UPDATEBATCHUSERINFOBYONE, PROC_DEF_UPDATEUSERINFOSYSPROC)));
	//m_mChoose.insert(pair<int, ChooseData>(++GetMaxRealChoose(), ChooseData("部分固定字段批量更改用户信息", OPER_PER_UPDATEBATCHUSERINFOBYMORE, PROC_DEF_UPDATEUSERINFOSYSPROC)));
	m_mChoose.insert(pair<int, ChooseData>(GetMaxRealChoose(), ChooseData("单个字段单条更改用户信息", OPER_PER_UPDATESINGLEUSERINFOEBYONE, PROC_DEF_UPDATEUSERINFOSYSPROC)));
	m_mChoose.insert(pair<int, ChooseData>(++GetMaxRealChoose(), ChooseData("部分固定字段单条更改用户信息", OPER_PER_UPDATESINGLEUSERINFOBYMORE, PROC_DEF_UPDATEUSERINFOSYSPROC)));
	m_mChoose.insert(pair<int, ChooseData>(++GetMaxRealChoose(), ChooseData("返回", OPER_PER_INVALID, PROC_DEF_USERINFOSYSPROC)));

	return true;
}

void UpdateUserInfoSysProc::EndProc()
{
	__super::EndProc();
}

void UpdateUserInfoSysProc::StartRecv(void* vpData, unsigned int DataLen, /*int iMianId,*/ int iAssistId)
{

}

void UpdateUserInfoSysProc::EndRecv()
{
	__super::EndRecv();
}

void UpdateUserInfoSysProc::SwitchToOper(OperPermission CurOper)
{
	__super::SwitchToOper(CurOper);
}

