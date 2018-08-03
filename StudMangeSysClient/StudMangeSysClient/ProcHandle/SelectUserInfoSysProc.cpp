#include "SelectUserInfoSysProc.h"
#include "ProcMgr.h"
#include "CheckTool.h"
#include "StringTool.h"
#include "TCPHandle.h"
#include "UserInfoMgr.h"

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
	bool bRes = false;
	switch(iAssistId)
	{
	case ASSIST_ID_SELECT_SINGLE_USERINFO_ACK:
		bRes = SelectSingleUserInfoRecvHandle(vpData, DataLen);
		break;
	case ASSIST_ID_SELECT_BATCH_USERINFO_ACK:
		bRes = SelectBatchUserInfoRecvHandle(vpData, DataLen);
		break;
	default:
		printf("%s iAssistId[%d] error\n", __FUNCTION__, iAssistId);
		//ProcMgr::GetInstance()->ProcSwitch(GetMyProcDef(), true); //重新登录注册
		SetIEndFlag(-1);
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

void SelectUserInfoSysProc::EndRecv()
{
	__super::EndRecv();
}

void SelectUserInfoSysProc::SwitchToOper(OperPermission CurOper)
{
	__super::SwitchToOper(CurOper);


	switch(CurOper)
	{
	case OPER_PER_SELECTBATCHUSERINFO: //批量查询用户信息
		SelectBatchUserInfoChooseHandle();
		break;
	case OPER_PER_SELECTSINGLEUSERINFO: //单条查询用户信息
		SelectSingleUserInfoChooseHandle();
		break;
	default:
		printf("%s 没有该操作！\n", __FUNCTION__);
		ProcMgr::GetInstance()->ProcSwitch(GetMyProcDef(), true);  
		break;
	}
}

void SelectUserInfoSysProc::SelectSingleUserInfoChooseHandle()
{
	if (OPER_PER_SELECTSINGLEUSERINFO != GetCurOper())
	{
		printf("%s 不是进行该操作[%d]，当前进行的操作是[%d] error\n", __FUNCTION__, OPER_PER_SELECTSINGLEUSERINFO, GetCurOper());
		OperInputErrorHandle(true, 1);
		return;
	}

	CS_MSG_SELECT_SINGLE_USERINFO_REQ SendReq;
	SendReq.uRecordDataTmp = 0;
	short sIdent = UserInfoMgr::GetInstance()->GetUserIdent();

	
	/*
	* 身份标识和权限问题：
	* 1.哪些对象有这些权限；
	* 2.根据身份标识确定可查看的对象类型（即根据身份表示区分类型），一般是可查看自己和比自己身份标识低的用户；
	* 3.可查看的那些字段，那些字段不可查看；
	*/
	//根据用户当前身份标识得到可获取的不同身份标识的用户信息，教师和学生不能看到自己的密码；管理员可以看到教师和学生的密码，但是不能看到自己的；教师也不能看到学生的密码。密码可不可看，由服务端做屏蔽处理
	if (sIdent== 1)//单条的，只能查询自己，也只能更改自己的
	{
		string strAccount = UserInfoMgr::GetInstance()->GetUserAccount();
		strcpy_s(SendReq.cAccount, sizeof(SendReq.cAccount), strAccount.c_str());
	}
	else if (sIdent == 2 || sIdent == 3) //教师可以查询学生和自己的，也可以更改学生和自己的。管理员可以查询教师学生和自己的，也可以更改教师学生和自己的
	{
		string strInputTmp = "查询学生";
		if (sIdent == 3)
		{
			strInputTmp = "查询学生或教师";
		}
		printf("1-查询自己 2-%s\n", strInputTmp.c_str());
		string strChoose;
		cin>>strChoose;
		if (!(CheckTool::CheckStringLen(strChoose, 1) && CheckTool::CheckStringByValid(strChoose, "1|2")))
		{
			OperInputErrorHandle();
			return;
		}
		if (strChoose == "2")
		{
			cout<<"请输入查询对象的用户名："<<endl;
			string strAccount;
			cin>>strAccount;
			if (!(CheckTool::CheckStringLen(strAccount, 30) && CheckTool::CheckStringByValid(strAccount, "A~Z|a~z|0~9")))
			{
				OperInputErrorHandle();
				return;
			}
			strcpy_s(SendReq.cAccount, sizeof(SendReq.cAccount), strAccount.c_str());
		}
		else
		{
			string strAccount = UserInfoMgr::GetInstance()->GetUserAccount();
			strcpy_s(SendReq.cAccount, sizeof(SendReq.cAccount), strAccount.c_str());
		}
	}
	else
	{
		printf("%s 用户自身身份标识[%d]有误\n", __FUNCTION__, UserInfoMgr::GetInstance()->GetUserIdent());
		OperInputErrorHandle(true, 1);
		return;
	}


	TCPHandle::GetInstance()->Send(&SendReq, sizeof(SendReq), /*MAIN_ID_LOGINREGISTER,*/ ASSIST_ID_SELECT_SINGLE_USERINFO_REQ);
}

void SelectUserInfoSysProc::SelectBatchUserInfoChooseHandle()
{

}

bool SelectUserInfoSysProc::SelectSingleUserInfoRecvHandle(void* vpData, unsigned int DataLen)
{
	if (OPER_PER_SELECTSINGLEUSERINFO != GetCurOper())
	{
		printf("%s 不是进行该操作[%d]，当前进行的操作是[%d] error\n", __FUNCTION__, OPER_PER_SELECTSINGLEUSERINFO, GetCurOper());
		SetIEndFlag(-1);
		return false;
	}
	if (DataLen != sizeof(SC_MSG_SELECT_SINGLE_USERINFO_ACK))
	{
		printf("DataLen[%u] error, It should be [%u]\n", DataLen, sizeof(SC_MSG_SELECT_SINGLE_USERINFO_ACK));
		SetIEndFlag(-1);
		return false;
	}

	SC_MSG_SELECT_SINGLE_USERINFO_ACK* RecvMSG = (SC_MSG_SELECT_SINGLE_USERINFO_ACK*) vpData;
	if (RecvMSG->bResCode == 1)
	{
		printf("%s 单条查询失败\n", __FUNCTION__);
		SetIEndFlag(-1);
		return false;
	}
	else if (RecvMSG->bResCode == 2)
	{
		printf("%s 没有用户信息\n", __FUNCTION__);
		SetIEndFlag(-1);
		return false;
	}
	else if (RecvMSG->bResCode == 3)
	{
		printf("%s 身份标识不符合查询不了该用户\n", __FUNCTION__);
		SetIEndFlag(-1);
		return false;
	}
	else if (RecvMSG->bResCode == 4)
	{
		printf("%s 其他异常\n", __FUNCTION__);
		SetIEndFlag(-1);
		return false;
	}


	vector<string> vSelectFieldShow;
	vector<string> vSelectResultShow;
	vSelectFieldShow.push_back("学号");
	vSelectResultShow.push_back(StringTool::NumberToStr((int)RecvMSG->uUserID));
	vSelectFieldShow.push_back("姓名");
	vSelectResultShow.push_back(RecvMSG->cName);
	vSelectFieldShow.push_back("账号");
	vSelectResultShow.push_back(RecvMSG->cAccount);
	vSelectFieldShow.push_back("班级");
	vSelectResultShow.push_back(RecvMSG->cGrade);
	vSelectFieldShow.push_back("密码");
	vSelectResultShow.push_back(RecvMSG->cPWD);//由服务端做屏蔽处理
	vSelectFieldShow.push_back("性别");
	vSelectResultShow.push_back(RecvMSG->sSex==0?"男":(RecvMSG->sSex==1?"女":"NULL"));
	vSelectFieldShow.push_back("身份标识");
	vSelectResultShow.push_back(RecvMSG->sIdent==1?"学生":(RecvMSG->sSex==2?"教师":(RecvMSG->sSex==3?"管理员":"NULL")));
	vSelectFieldShow.push_back("专业");
	vSelectResultShow.push_back(RecvMSG->cMajor);

	ShowSelectSingleScoreResult(vSelectFieldShow, vSelectResultShow, true);
	SetIEndFlag(1);

	return true;
}

bool SelectUserInfoSysProc::SelectBatchUserInfoRecvHandle(void* vpData, unsigned int DataLen)
{

	return true;
}


void SelectUserInfoSysProc::ShowSelectSingleScoreResult(vector<string> vSelectFieldShow, vector<string> vSelectResultShow, bool bShow)
{
	if (vSelectFieldShow.empty() || vSelectFieldShow.empty())
	{
		printf("没有显示信息\n");
		return;
	}
	if (vSelectFieldShow.size() != vSelectFieldShow.size())
	{
		printf("字段与结果数量不相同\n");
		return;
	}

	string strFieldShow;
	string strResult;

	for (unsigned i=0; i<vSelectFieldShow.size(); i++)
	{
		if (i == vSelectFieldShow.size()-1)
		{
			strFieldShow += vSelectFieldShow.at(i);
			strResult += vSelectResultShow.at(i);
		}
		else
		{
			strFieldShow += StringTool::SetStringFieldWidth(vSelectFieldShow.at(i), 12) ;
			strResult += StringTool::SetStringFieldWidth(vSelectResultShow.at(i), 12);
		}

	}

	if (bShow)
	{
		printf("-------------------------------\n");
		printf("查询结果显示：\n");
		cout<<strFieldShow<<endl;
	}
	cout<<strResult<<endl;
}