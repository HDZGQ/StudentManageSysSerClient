#include "AuthoritySysProc.h"
#include "ProcMgr.h"
#include "CheckTool.h"
#include "StringTool.h"
#include "TCPHandle.h"
#include "UserInfoMgr.h"

AuthoritySysProc::AuthoritySysProc(ProcDef nProcDef) : BaseProc(nProcDef)
{
	initMapChoose();
}

AuthoritySysProc::~AuthoritySysProc()
{

}

bool AuthoritySysProc::initMapChoose()
{
	m_mChoose.insert(pair<int, ChooseData>(GetMaxRealChoose(), ChooseData("增加用户权限", OPER_PER_ADDAUTHORITY, PROC_DEF_AUTHORITYSYSPROC)));
	m_mChoose.insert(pair<int, ChooseData>(++GetMaxRealChoose(), ChooseData("删除用户权限", OPER_PER_DELETEAUTHORITY, PROC_DEF_AUTHORITYSYSPROC)));
	m_mChoose.insert(pair<int, ChooseData>(++GetMaxRealChoose(), ChooseData("返回", OPER_PER_INVALID, PROC_DEF_COMMONSYSPROC)));

	return true;
}

void AuthoritySysProc::EndProc()
{
	__super::EndProc();
}

void AuthoritySysProc::StartRecv(void* vpData, unsigned int DataLen, /*int iMianId,*/ int iAssistId)
{
	bool bRes = false;
	switch(iAssistId)
	{
	case ASSIST_ID_GET_AUTHORITY_ACK:
		bRes = GetAuthoritbyAfterEditAuthoritbyRecvHandle(vpData, DataLen);
		break;
	case ASSIST_ID_EDIT_AUTHORITY_ACK:
		bRes = EditAuthoritRecvHandle(vpData, DataLen);
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

void AuthoritySysProc::EndRecv()
{
	__super::EndRecv();
}

void AuthoritySysProc::SwitchToOper(OperPermission CurOper)
{
	switch(CurOper)
	{
	case OPER_PER_ADDAUTHORITY: //增加某用户一种或者多种权限
	case OPER_PER_DELETEAUTHORITY: //删除某用户一种或者多种权限
		GetAuthoritbyAfterEditAuthoritbyChooseHandle();
		break;
	default:
		printf("%s 没有该操作！\n", __FUNCTION__);
		ProcMgr::GetInstance()->ProcSwitch(GetMyProcDef(), true);  
		break;
	}
}

void AuthoritySysProc::GetAuthoritbyAfterEditAuthoritbyChooseHandle()
{
	if (OPER_PER_ADDAUTHORITY != GetCurOper() && OPER_PER_DELETEAUTHORITY != GetCurOper())
	{
		printf("不是进行该操作[%d | %d]，当前进行的操作是[%d] error\n", OPER_PER_ADDAUTHORITY, OPER_PER_DELETEAUTHORITY, GetCurOper());
		OperInputErrorHandle(true, 1);
		return;
	}

	CS_MSG_GET_AUTHORITY_REQ SendReq;
	if (OPER_PER_ADDAUTHORITY == GetCurOper())
	{
		SendReq.sType = 2;
	}
	else
	{
		SendReq.sType = 1;
	}

	cout<<"请输入获取可增删权限对象的用户名："<<endl;
	string strAccount;
	cin>>strAccount;
	if (!(CheckTool::CheckStringLen(strAccount, 30) && CheckTool::CheckStringByValid(strAccount, "A~Z|a~z|0~9")))
	{
		OperInputErrorHandle();
		return;
	}
	strcpy_s(SendReq.cAccount, sizeof(SendReq.cAccount), strAccount.c_str());

	TCPHandle::GetInstance()->Send(&SendReq, sizeof(SendReq), /*MAIN_ID_LOGINREGISTER,*/ ASSIST_ID_GET_AUTHORITY_REQ);
}

void AuthoritySysProc::EditAuthoritChooseHandle(unsigned char sType, char* cAccount, unsigned char* cAuthority, unsigned char cAuthorityCount)
{
	if (cAccount == NULL || cAuthority == NULL || 0 == cAuthorityCount || cAuthorityCount > MAXAUTHORITBYCOUNT)
	{
		printf("%s 对象的账号字段或权限字段为NULL，或者权限数目异常[%u]\n", __FUNCTION__, (unsigned)cAuthorityCount);
		SetIEndFlag(-1);
		return;
	}
	if (sType < 1 || sType > 2)
	{
		printf("%s 权限操作类型不对sType[%u]\n", __FUNCTION__, (unsigned)sType);
		SetIEndFlag(-1);
		return;
	}

	//先检查传送过来的权限是否合法
	for (unsigned char i=0; i<cAuthorityCount; i++)
	{
		if (!UserInfoMgr::GetInstance()->CanFindInAllOperPer((OperPermission)cAuthority[i]))
		{
			printf("%s 操作权限不合法[%u]\n", __FUNCTION__, (unsigned)cAuthority[i]);
			SetIEndFlag(-1);
			return;
		}
	}
	//先显示可增删权限，供选择
	ShowAuthoritby(cAuthority, cAuthorityCount);
	
	if (sType == 1)
	{
		printf("请输入您要删除对象的权限（格式为 XX|XX|XX）：\n");
	}
	else
	{
		printf("请输入您要增加对象的权限（格式为 XX|XX|XX）：\n");
	}
	string strOperPer;
	cin>>strOperPer;
	if (!(CheckTool::CheckStringLen(strOperPer, 90) && CheckStringVaildRemoveSpl(strOperPer, "|", "0~9"))) 
	{
		OperInputErrorHandle(false);
		return;
	}
	vector<string> vecStrOperPer = StringTool::Splite(strOperPer);

	CS_MSG_EDIT_AUTHORITY_REQ SendReq;
	SendReq.sType = sType;
	SendReq.cAuthorityCount = 0;
	for (; SendReq.cAuthorityCount<vecStrOperPer.size() && SendReq.cAuthorityCount<MAXAUTHORITBYCOUNT; SendReq.cAuthorityCount++)
	{
		SendReq.cAuthority[SendReq.cAuthorityCount] = (unsigned char)atoi(vecStrOperPer.at(SendReq.cAuthorityCount).c_str());
	}
	strcpy_s(SendReq.cAccount, sizeof(SendReq.cAccount), cAccount);

	TCPHandle::GetInstance()->Send(&SendReq, sizeof(SendReq), /*MAIN_ID_LOGINREGISTER,*/ ASSIST_ID_EDIT_AUTHORITY_REQ);
}

bool AuthoritySysProc::GetAuthoritbyAfterEditAuthoritbyRecvHandle(void* vpData, unsigned int DataLen)
{
	if (OPER_PER_ADDAUTHORITY != GetCurOper() && OPER_PER_DELETEAUTHORITY != GetCurOper())
	{
		printf("不是进行该操作[%d | %d]，当前进行的操作是[%d] error\n", OPER_PER_ADDAUTHORITY, OPER_PER_DELETEAUTHORITY, GetCurOper());
		SetIEndFlag(-1);
		return false;
	}
	if (DataLen != sizeof(CS_MSG_GET_AUTHORITY_ACK))
	{
		printf("DataLen[%u] error, It should be [%u]\n", DataLen, sizeof(CS_MSG_GET_AUTHORITY_ACK));
		SetIEndFlag(-1);
		return false;
	}

	CS_MSG_GET_AUTHORITY_ACK* RecvMSG = (CS_MSG_GET_AUTHORITY_ACK*) vpData;
	if (RecvMSG->bSucceed)
	{
		EditAuthoritChooseHandle(RecvMSG->sType, RecvMSG->cAccount, RecvMSG->cAuthority, RecvMSG->cAuthorityCount);
	}
	else
	{
		printf("获取用户可增删权限返回不成功\n");
		SetIEndFlag(-1);
		return false;
	}

	return true;
}

bool AuthoritySysProc::EditAuthoritRecvHandle(void* vpData, unsigned int DataLen)
{
	if (OPER_PER_ADDAUTHORITY != GetCurOper() && OPER_PER_DELETEAUTHORITY != GetCurOper())
	{
		printf("不是进行该操作[%d | %d]，当前进行的操作是[%d] error\n", OPER_PER_ADDAUTHORITY, OPER_PER_DELETEAUTHORITY, GetCurOper());
		SetIEndFlag(-1);
		return false;
	}
	if (DataLen != sizeof(CS_MSG_EDIT_AUTHORITY_ACK))
	{
		printf("DataLen[%u] error, It should be [%u]\n", DataLen, sizeof(CS_MSG_EDIT_AUTHORITY_ACK));
		SetIEndFlag(-1);
		return false;
	}

	CS_MSG_EDIT_AUTHORITY_ACK* RecvMSG = (CS_MSG_EDIT_AUTHORITY_ACK*) vpData;
	if (RecvMSG->sType < 1 || RecvMSG->sType > 2)
	{
		printf("%s 返回权限操作类型不对RecvMSG->sType[%u]\n", __FUNCTION__, (unsigned)RecvMSG->sType);
		SetIEndFlag(-1);
		return false;
	}

	if (RecvMSG->bSucceed)
	{
		printf("给账号[%s]%s权限成功\n", RecvMSG->cAccount, (RecvMSG->sType==1?"删除":"增加"));
		SetIEndFlag(1);
	}
	else
	{
		printf("增删权限操作返回不成功\n");
		SetIEndFlag(-1);
		return false;
	}

	return true;
}


void AuthoritySysProc::ShowAuthoritby(unsigned char* cAuthority, unsigned char cAuthorityCount, int iField)
{
	if (NULL == cAuthority || 0 == cAuthorityCount)
	{
		return;
	}

	if (iField == 0)
		iField = 0xEFFFFFFF;
	else if (iField < 0)
		iField *= -1;

	unsigned char iCount=0;
	string str = "";
	char ch[60];
	for (; iCount<cAuthorityCount; iCount++)
	{
		if (iCount != 0 && iCount % iField == 0)
		{
			cout<<str<<endl;
			str = "";
		}
// 		if (iCount != 0 && iCount % iField != 0)
// 		{
// 			cout<<"\t\t";
// 		}
		
		memset(ch, 0 , sizeof(ch));
		sprintf_s(ch, sizeof(ch), "%u--%s", cAuthority[iCount], UserInfoMgr::GetInstance()->GetDescByOperPer((OperPermission)cAuthority[iCount]).c_str());
		str += StringTool::SetStringFieldWidth(ch, 35) ;
		//printf("%u--%s", cAuthority[iCount], UserInfoMgr::GetInstance()->GetDescByOperPer((OperPermission)cAuthority[iCount]).c_str());
	}
	if (iCount > 0)
	{
		cout<<str<<endl;
	}
}

bool AuthoritySysProc::FindOneInArray(unsigned char* pArray, unsigned char iCount, unsigned char iMaxCount, unsigned char Obj)
{
	bool bRes = false;
	if (!pArray || iCount==0 || iCount>iMaxCount)
	{
		return bRes;
	}

	unsigned char* pArrayTmp = new unsigned char[iCount];
	memcpy(pArrayTmp, pArray, iCount);

	for (unsigned char i=0; i<iCount; i++)
	{
		if (Obj == pArrayTmp[i])
		{
			bRes = true;
			break;
		}
	}

	delete []pArrayTmp;
	return bRes;
}

bool AuthoritySysProc::CheckStringVaildRemoveSpl(string str, string strSpl, string strVaild)
{
	vector<string> vecStr = StringTool::Splite(str, strSpl);
	string strTmp;
	for (unsigned i=0; i<vecStr.size(); i++)
	{
		strTmp += vecStr.at(i);
	}

	return CheckTool::CheckStringByValid(strTmp, strVaild);
}