#include "AddUserInfoSysProc.h"

AddUserInfoSysProc::AddUserInfoSysProc(ProcDef nProcDef) : BaseProc(nProcDef)
{
	initMapChoose();
}

AddUserInfoSysProc::~AddUserInfoSysProc()
{

}

bool AddUserInfoSysProc::initMapChoose()
{
	m_mChoose.insert(pair<int, ChooseData>(GetMaxRealChoose(), ChooseData("批量增加用户信息", OPER_PER_ADDBATCHUSERINFO, PROC_DEF_ADDUSERINFOSYSPROC)));
	m_mChoose.insert(pair<int, ChooseData>(++GetMaxRealChoose(), ChooseData("单条增加用户信息", OPER_PER_ADDSINGLEUSERINFO, PROC_DEF_ADDUSERINFOSYSPROC)));
	m_mChoose.insert(pair<int, ChooseData>(++GetMaxRealChoose(), ChooseData("返回", OPER_PER_INVALID, PROC_DEF_USERINFOSYSPROC)));

	return true;
}

void AddUserInfoSysProc::EndProc()
{
	__super::EndProc();
}

void AddUserInfoSysProc::StartRecv(void* vpData, unsigned int DataLen, /*int iMianId,*/ int iAssistId)
{

}

void AddUserInfoSysProc::EndRecv()
{
	__super::EndRecv();
}

void AddUserInfoSysProc::SwitchToOper(OperPermission CurOper)
{
	__super::SwitchToOper(CurOper);
}
