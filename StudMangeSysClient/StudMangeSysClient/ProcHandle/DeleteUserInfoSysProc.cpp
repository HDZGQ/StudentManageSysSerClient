#include "DeleteUserInfoSysProc.h"
#include "TCPHandle.h"
#include "ProcMgr.h"
#include "CheckTool.h"
#include "StringTool.h"

DeleteUserInfoSysProc::DeleteUserInfoSysProc(ProcDef nProcDef) : BaseProc(nProcDef)
{
	initMapChoose();
}

DeleteUserInfoSysProc::~DeleteUserInfoSysProc()
{

}

bool DeleteUserInfoSysProc::initMapChoose()
{
	m_mChoose.insert(pair<int, ChooseData>(GetMaxRealChoose(), ChooseData("批量删除用户信息", OPER_PER_DELETEBATCHUSERINFO, PROC_DEF_DELETEUSERINFOSYSPROC)));
	m_mChoose.insert(pair<int, ChooseData>(++GetMaxRealChoose(), ChooseData("单条删除用户信息", OPER_PER_DELETESINGLEUSERINFO, PROC_DEF_DELETEUSERINFOSYSPROC)));
	m_mChoose.insert(pair<int, ChooseData>(++GetMaxRealChoose(), ChooseData("返回", OPER_PER_INVALID, PROC_DEF_USERINFOSYSPROC)));

	return true;
}

void DeleteUserInfoSysProc::EndProc()
{
	__super::EndProc();
}

void DeleteUserInfoSysProc::StartRecv(void* vpData, unsigned int DataLen, /*int iMianId,*/ int iAssistId)
{
	__super::StartRecv(vpData, DataLen, iAssistId);

	bool bRes = false;
	switch(iAssistId)
	{
	case ASSIST_ID_DELETE_USERINFO_ACK:
		bRes = DeleteUserInfoRecvHandle(vpData, DataLen);
		break;
	default:
		if (GetIEndFlag() == 0)
		{
			printf("%s iAssistId[%d] error\n", __FUNCTION__, iAssistId);
			//ProcMgr::GetInstance()->ProcSwitch(GetMyProcDef(), true); //重新登录注册
			SetIEndFlag(-1);
		}
		break;
	}


	if (GetIEndFlag() == -1)
	{
		ProcMgr::GetInstance()->ProcSwitch(GetMyProcDef(), true);
	}
	else if (GetIEndFlag() == 1)
	{
		ProcMgr::GetInstance()->ProcSwitch(GetNextProc()); //处理成功，切换到下一个界面
	}
	EndRecv(); //有使用GetNextProc()就要先切换再清空数据  但是可能这样做会有问题，如果切换回来同一个界面
}

void DeleteUserInfoSysProc::EndRecv()
{
	__super::EndRecv();
}

void DeleteUserInfoSysProc::SwitchToOper(OperPermission CurOper)
{
	__super::SwitchToOper(CurOper);


	switch(CurOper)
	{
	case OPER_PER_DELETEBATCHUSERINFO: //批量删除用户信息
	case OPER_PER_DELETESINGLEUSERINFO: //单条删除用户信息
		DeleteUserInfoChooseHandle();
		break;
	default:
		printf("%s 没有该操作！\n", __FUNCTION__);
		ProcMgr::GetInstance()->ProcSwitch(GetMyProcDef(), true);  
		break;
	}
}

//选择操作处理
void DeleteUserInfoSysProc::DeleteUserInfoChooseHandle()
{
	if (OPER_PER_DELETEBATCHUSERINFO != GetCurOper() && OPER_PER_DELETESINGLEUSERINFO != GetCurOper())
	{
		printf("%s 不是进行该操作[%d | %d]，当前进行的操作是[%d] error\n", __FUNCTION__, OPER_PER_DELETEBATCHUSERINFO, OPER_PER_DELETESINGLEUSERINFO, GetCurOper());
		OperInputErrorHandle(true, 1);
		return;
	}

	CS_MSG_DELETE_USERINFO_REQ SendReq;

	if (OPER_PER_DELETEBATCHUSERINFO == GetCurOper())
	{
		SendReq.sType = 2;

		printf("请输入需要批量删除成绩的用户id范围（格式 XXXXXXX-XXXXXXX）：\n");
		string strUserIdRange;
		cin>>strUserIdRange;
		vector<string> vecUserIdRange = StringTool::Splite(strUserIdRange, "-");
		if (!(vecUserIdRange.size()==2 &&  CheckTool::CheckStringLen(vecUserIdRange.at(0), 6) &&  CheckTool::CheckStringLen(vecUserIdRange.at(1), 6) && \
			CheckTool::CheckStringByValid(vecUserIdRange.at(0), "0~9") && CheckTool::CheckStringByValid(vecUserIdRange.at(1), "0~9") && \
			(unsigned)atoi(vecUserIdRange.at(0).c_str())>=100001 && (unsigned)atoi(vecUserIdRange.at(0).c_str())<=999999 && \
			(unsigned)atoi(vecUserIdRange.at(1).c_str())>=100001 && (unsigned)atoi(vecUserIdRange.at(1).c_str())<=999999 && \
			(unsigned)atoi(vecUserIdRange.at(0).c_str())<=(unsigned)atoi(vecUserIdRange.at(1).c_str())))
		{
			OperInputErrorHandle();
			return;
		}
		SendReq.uUserid[0] = (unsigned)atoi(vecUserIdRange.at(0).c_str());
		SendReq.uUserid[1] = (unsigned)atoi(vecUserIdRange.at(1).c_str());
		SendReq.uUserid[2] = 1;
	}
	else
	{
		SendReq.sType = 1;

		string strAccount;
		cout<<"请输入您要删除成绩的用户名（也就是账号）："<<endl;
		cin>>strAccount;
		if (!(CheckTool::CheckStringLen(strAccount, 30) && CheckTool::CheckStringByValid(strAccount, "A~Z|a~z|0~9")))
		{
			OperInputErrorHandle();
			return;
		}
		strcpy_s(SendReq.cAccount, sizeof(SendReq.cAccount), strAccount.c_str());
	}

	TCPHandle::GetInstance()->Send(&SendReq, sizeof(SendReq), /*MAIN_ID_LOGINREGISTER,*/ ASSIST_ID_DELETE_USERINFO_REQ);
}

//返回结果处理
bool DeleteUserInfoSysProc::DeleteUserInfoRecvHandle(void* vpData, unsigned int DataLen)
{
	if (OPER_PER_DELETEBATCHUSERINFO != GetCurOper() && OPER_PER_DELETESINGLEUSERINFO != GetCurOper())
	{
		printf("%s 不是进行该操作[%d | %d]，当前进行的操作是[%d] error\n", __FUNCTION__, OPER_PER_DELETEBATCHUSERINFO, OPER_PER_DELETESINGLEUSERINFO, GetCurOper());
		SetIEndFlag(-1);
		return false;
	}

	if (DataLen != sizeof(SC_MSG_DELETE_USERINFO_ACK))
	{
		printf("DataLen[%u] error, It should be [%u]\n", DataLen, sizeof(SC_MSG_DELETE_USERINFO_ACK));
		SetIEndFlag(-1);
		return false;
	}

	SC_MSG_DELETE_USERINFO_ACK* RecvMSG = (SC_MSG_DELETE_USERINFO_ACK*) vpData;
	if (RecvMSG->bSucceed)
	{
		if (RecvMSG->sType == 1)
		{
			printf("单条删除用户信息成功\n");
			SetIEndFlag(1);
		}
		else if (RecvMSG->sType == 2)
		{
			printf("批量删除用户信息成功\n");
			SetIEndFlag(1);
		}
		else
		{
			printf("返回删除用户信息类型错误 RecvMSG->sType=%d\n",RecvMSG->sType);
			SetIEndFlag(-1);
			return false;
		}
	}
	else
	{
		printf("%s 删除用户信息返回不成功\n", __FUNCTION__);
		SetIEndFlag(-1);
		return false;
	}

	return true;
}