#include "AddUserInfoSysProc.h"
#include "ProcMgr.h"
#include "UserInfoMgr.h"
#include "CheckTool.h"
#include "StringTool.h"
#include "TCPHandle.h"

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
	bool bRes = false;
	switch(iAssistId)
	{
	case ASSIST_ID_ADD_SINGLE_USERINFO_ACK:
		bRes = AddSingleUserInfoRecvHandle(vpData, DataLen);
		break;
	case ASSIST_ID_ADD_BATCH_USERINFO_ACK:
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

void AddUserInfoSysProc::EndRecv()
{
	__super::EndRecv();
}

void AddUserInfoSysProc::SwitchToOper(OperPermission CurOper)
{
	__super::SwitchToOper(CurOper);


	switch(CurOper)
	{
	case OPER_PER_ADDSINGLEUSERINFO: //单条增加用户信息请求
		AddSingleUserInfoChooseHandle();
		break;
	case OPER_PER_ADDBATCHUSERINFO: //单科单条增加成绩  操作前请求处理
		break;
	default:
		printf("%s 没有该操作！\n", __FUNCTION__);
		ProcMgr::GetInstance()->ProcSwitch(GetMyProcDef(), true);  
		break;
	}
}

void AddUserInfoSysProc::AddSingleUserInfoChooseHandle()
{
	string strName;
	cout<<"请输入添加对象的姓名："<<endl;
	cin>>strName;
	if (!(CheckTool::CheckStringLen(strName, 30) && CheckTool::CheckStringByValid(strName, "")))
	{
		OperInputErrorHandle();
		return;
	}

	string strAccount;
	cout<<"请输入添加对象的用户名（用于登录）："<<endl;
	cin>>strAccount;
	if (!(CheckTool::CheckStringLen(strAccount, 30) && CheckTool::CheckStringByValid(strAccount, "A~Z|a~z|0~9")))
	{
		OperInputErrorHandle();
		return;
	}	

	string strPassword;
	cout<<"请输入添加对象的密码："<<endl;
	cin>>strPassword;
	if (!(CheckTool::CheckStringLen(strPassword, 30) && CheckTool::CheckStringByValid(strPassword, "A~Z|a~z|0~9|_|-")))
	{
		OperInputErrorHandle();	
		return;
	}

	string strSex;
	cout<<"请输入添加对象的性别（0-男  1-女）："<<endl;
	cin>>strSex;
	if (!(CheckTool::CheckStringLen(strSex, 1) && CheckTool::CheckStringByValid(strSex, "0|1")))
	{
		OperInputErrorHandle();
		return;
	}

	string strIdent;
	string strValid;
	if (UserInfoMgr::GetInstance()->GetUserIdent() == 2 || UserInfoMgr::GetInstance()->GetUserIdent() == 1) //学生被添加权限这种情况，也可以使用该功能。但是只能和老师的权限一样
	{
		//cout<<"请输入添加对象的职业（1-学生）："<<endl;
		strIdent = "1";
		strValid = "1";
	}
	else if (UserInfoMgr::GetInstance()->GetUserIdent() == 3) //管理员选择
	{
		cout<<"请输入添加对象的职业（1-学生 2-教师）："<<endl;
		cin>>strIdent;
		strValid = "1|2";
	}
	else
	{
		printf("不合法的身边标识\n");
		OperInputErrorHandle(true, 1);
		return;
	}
	if (!(CheckTool::CheckStringLen(strIdent, 1) && CheckTool::CheckStringByValid(strIdent, strValid)))
	{
		OperInputErrorHandle();
		return;
	}

	string strMajor;
	cout<<"请输入添加对象的专业："<<endl;
	cin>>strMajor;
	if (!(CheckTool::CheckStringLen(strMajor, 40) && CheckTool::CheckStringByValid(strMajor, "")))
	{
		OperInputErrorHandle();
		return;
	}	

	string strGrade;
	cout<<"请输入添加对象的班级："<<endl;
	cin>>strGrade;
	if (!(CheckTool::CheckStringLen(strGrade, 30) && CheckTool::CheckStringByValid(strGrade, "")))
	{
		OperInputErrorHandle();
		return;
	}	

	//发送服务端
	CS_MSG_ADD_SINGLE_USERINFO_REQ SendReq;
	SendReq.sSex = (unsigned char)atoi(strSex.c_str());
	SendReq.sIdent = (unsigned char)atoi(strIdent.c_str());
	strcpy_s(SendReq.cName, sizeof(SendReq.cName), strName.c_str());
	strcpy_s(SendReq.cAccount, sizeof(SendReq.cAccount), strAccount.c_str());
	strcpy_s(SendReq.cPWD, sizeof(SendReq.cPWD), strPassword.c_str());
	strcpy_s(SendReq.cMajor, sizeof(SendReq.cMajor), strMajor.c_str());
	strcpy_s(SendReq.cGrade, sizeof(SendReq.cGrade), strGrade.c_str());
	
	TCPHandle::GetInstance()->Send(&SendReq, sizeof(SendReq), /*MAIN_ID_LOGINREGISTER,*/ ASSIST_ID_ADD_SINGLE_USERINFO_REQ);
}

bool AddUserInfoSysProc::AddSingleUserInfoRecvHandle(void* vpData, unsigned int DataLen)
{
	if (OPER_PER_ADDSINGLEUSERINFO != GetCurOper())
	{
		printf("不是进行该操作[%d]，当前进行的操作是[%d] error\n", OPER_PER_ADDSINGLEUSERINFO, GetCurOper());
		SetIEndFlag(-1);
		return false;
	}
	if (DataLen != sizeof(CS_MSG_ADD_SINGLE_USERINFO_ACK))
	{
		printf("DataLen[%u] error, It should be [%u]\n", DataLen, sizeof(CS_MSG_ADD_SINGLE_USERINFO_ACK));
		SetIEndFlag(-1);
		return false;
	}
	CS_MSG_ADD_SINGLE_USERINFO_ACK* RecvMSG = (CS_MSG_ADD_SINGLE_USERINFO_ACK*) vpData;
	if (RecvMSG->bSucceed)
	{
		printf("单条添加账号[%s]用户信息成功\n", RecvMSG->cAccount);
		SetIEndFlag(1);
	}
	else
	{
		printf("单条增加用户信息失败\n");
		SetIEndFlag(-1);
		return false;
	}

	return true;
}