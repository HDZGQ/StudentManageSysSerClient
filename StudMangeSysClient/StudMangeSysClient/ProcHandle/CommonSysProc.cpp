#include "CommonSysProc.h"
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
	m_mChoose.insert(pair<int, ChooseData>(++GetMaxRealChoose(), ChooseData("用户信息", OPER_PER_INVALID, PROC_DEF_USERINFOSYSPROC)));
	m_mChoose.insert(pair<int, ChooseData>(++GetMaxRealChoose(), ChooseData("权限", OPER_PER_INVALID, PROC_DEF_AUTHORITYSYSPROC)));
	m_mChoose.insert(pair<int, ChooseData>(++GetMaxRealChoose(), ChooseData("返回", OPER_PER_INVALID, PROC_DEF_ENTERSYSPROC)));

	return true;
}

void CommonSysProc::EndProc()
{
	__super::EndProc();
}

void CommonSysProc::StartRecv(void* vpData, unsigned int DataLen, /*int iMianId,*/ int iAssistId)
{
	__super::StartRecv(vpData, DataLen, iAssistId);
}

void CommonSysProc::EndRecv()
{
	if (PROC_DEF_ENTERSYSPROC == GetProcDefByRealChoose(GetRealChooseByUserChoose(GetMyChoose())))
	{
		ExitSysHandle();
	}
	__super::EndRecv();
}

void CommonSysProc::SwitchToOper(OperPermission CurOper)
{
	__super::SwitchToOper(CurOper);


}

void CommonSysProc::ExitSysHandle()
{
	//发送服务端
	CS_MSG_EXIT_SYS_REQ SendReq;
	SendReq.bExit = true;
	TCPHandle::GetInstance()->Send(&SendReq, sizeof(SendReq), ASSIST_ID_EXIT_SYS_REQ);
}