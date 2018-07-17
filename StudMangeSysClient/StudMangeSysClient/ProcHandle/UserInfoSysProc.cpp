#include "UserInfoSysProc.h"

UserInfoSysProc::UserInfoSysProc(ProcDef nProcDef) : BaseProc(nProcDef)
{
	initMapChoose();
}

UserInfoSysProc::~UserInfoSysProc()
{

}

bool UserInfoSysProc::initMapChoose()
{
	m_mChoose.insert(pair<int, ChooseData>(GetMaxRealChoose(), ChooseData("增加用户信息", OPER_PER_INVALID, PROC_DEF_ADDUSERINFOSYSPROC)));
	m_mChoose.insert(pair<int, ChooseData>(++GetMaxRealChoose(), ChooseData("更改用户信息", OPER_PER_INVALID, PROC_DEF_UPDATEUSERINFOSYSPROC)));
	m_mChoose.insert(pair<int, ChooseData>(++GetMaxRealChoose(), ChooseData("查询用户信息", OPER_PER_INVALID, PROC_DEF_SELECTUSERINFOSYSPROC)));
	m_mChoose.insert(pair<int, ChooseData>(++GetMaxRealChoose(), ChooseData("删除用户信息", OPER_PER_INVALID, PROC_DEF_DELETEUSERINFOSYSPROC)));
	m_mChoose.insert(pair<int, ChooseData>(++GetMaxRealChoose(), ChooseData("返回", OPER_PER_INVALID, PROC_DEF_COMMONSYSPROC)));

	return true;
}

void UserInfoSysProc::EndProc()
{
	__super::EndProc();
}

void UserInfoSysProc::StartRecv(void* vpData, unsigned int DataLen, /*int iMianId,*/ int iAssistId)
{

}

void UserInfoSysProc::EndRecv()
{
	__super::EndRecv();
}

void UserInfoSysProc::SwitchToOper(OperPermission CurOper)
{
	__super::SwitchToOper(CurOper);
}
