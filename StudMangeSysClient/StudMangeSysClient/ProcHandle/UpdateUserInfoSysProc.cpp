#include "UpdateUserInfoSysProc.h"
#include "ProcMgr.h"
#include "UserInfoMgr.h"
#include "TCPHandle.h"
#include "CheckTool.h"
#include "StringTool.h"

UpdateUserInfoSysProc::UpdateUserInfoSysProc(ProcDef nProcDef) : BaseProc(nProcDef)
{
	initMapChoose();
	m_maxUserInfoFieldID = 0;
	InitUserInfoFieldName();

	m_bUserInfoFieldCount = 0;
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
	__super::StartRecv(vpData, DataLen, iAssistId);

	bool bRes = false;
	switch(iAssistId)
	{
	case ASSIST_ID_SELECT_SINGLE_USERINFO_ACK:
		bRes = GetUserInfoAfterUpdateSingleUserInfoRecvHandle(vpData, DataLen);
		break;
	case ASSIST_ID_UPDATE_SINGLE_USERINFO_ACK:
		bRes = UpdateSingleUserInfoRecvHandle(vpData, DataLen);
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

void UpdateUserInfoSysProc::EndRecv()
{
	
	if (0 == GetIEndFlag())
		return;

	m_bUserInfoFieldCount = 0;
	memset(m_sUserInfoField, 0, sizeof(m_sUserInfoField));
	memset(m_cUserInfoValue, 0, sizeof(m_cUserInfoValue));

	__super::EndRecv();
}

void UpdateUserInfoSysProc::SwitchToOper(OperPermission CurOper)
{
	__super::SwitchToOper(CurOper);


	switch(CurOper)
	{
	case OPER_PER_UPDATESINGLEUSERINFOEBYONE: //单个字段单条更改用户信息
	case OPER_PER_UPDATESINGLEUSERINFOBYMORE: //部分固定字段单条更改用户信息
		GetUserInfoAfterUpdateSingleUserInfoChooseHandle();
		break;
	default:
		printf("%s 没有该操作！\n", __FUNCTION__);
		ProcMgr::GetInstance()->ProcSwitch(GetMyProcDef(), true);  
		break;
	}
}

void UpdateUserInfoSysProc::GetUserInfoAfterUpdateSingleUserInfoChooseHandle()
{
	if (OPER_PER_UPDATESINGLEUSERINFOEBYONE != GetCurOper() && OPER_PER_UPDATESINGLEUSERINFOBYMORE != GetCurOper())
	{
		printf("%s 不是进行该操作[%d | %d]，当前进行的操作是[%d] error\n", __FUNCTION__, OPER_PER_UPDATESINGLEUSERINFOEBYONE, OPER_PER_UPDATESINGLEUSERINFOBYMORE, GetCurOper());
		OperInputErrorHandle(true, 1);
		return;
	}

	CS_MSG_SELECT_SINGLE_USERINFO_REQ SendReq;
	SendReq.uRecordDataTmp = 1;
	short sIdent = UserInfoMgr::GetInstance()->GetUserIdent();

	/* 身份标识和权限问题：
	* 1.哪些对象有这些权限；
	* 2.根据身份标识确定可更改的对象类型（即根据身份表示区分类型），一般是可更改自己和比自己身份标识低的用户；
	* 3.可更改的那些字段，那些字段不可更改；
	*/
	//根据用户当前身份标识得到可获取的不同身份标识的用户信息，教师和学生不能看到自己的密码；管理员可以看到教师和学生的密码，但是不能看到自己的；教师也不能看到学生的密码；密码只能在单子段里面更改，因为需要确认一遍，不适合固定字段情况更改。密码可不可看，由服务端做屏蔽处理
	if (sIdent== 1)//单条的，只能查询自己，也只能更改自己的
	{
		string strAccount = UserInfoMgr::GetInstance()->GetUserAccount();
		strcpy_s(SendReq.cAccount, sizeof(SendReq.cAccount), strAccount.c_str());
	}
	else if (sIdent == 2 || sIdent == 3) //教师可以查询学生和自己的，也可以更改学生和自己的。管理员可以查询教师学生和自己的，也可以更改教师学生和自己的
	{
		string strInputTmp = "更改学生";
		if (sIdent == 3)
		{
			strInputTmp = "更改学生或教师";
		}
		printf("1-更改自己 2-%s\n", strInputTmp.c_str());
		string strChoose;
		cin>>strChoose;
		if (!(CheckTool::CheckStringLen(strChoose, 1) && CheckTool::CheckStringByValid(strChoose, "1|2")))
		{
			OperInputErrorHandle();
			return;
		}
		if (strChoose == "2")
		{
			cout<<"请输入更改对象的用户名："<<endl;
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

void UpdateUserInfoSysProc::UpdateSingleUserInfoByOneFieldChooseHandle(unsigned uUserID, char* pAccount, char* pPwd, char* pName, unsigned char uSex, unsigned char uIdent, char* pMajor, char* pGrade)
{
	if (uUserID < 1 || NULL == pAccount || uIdent < 1 || uIdent > 3)
	{
		printf("%s 用户ID为0或账号为NULL或身边标识uIdent[%u]异常\n", __FUNCTION__, uIdent);
		SetIEndFlag(-1);
		return;
	}

	CS_MSG_UPDATE_SINGLE_USERINFO_REQ SendReq;
	SendReq.sType = 1;
	strcpy_s(SendReq.cAccount, sizeof(SendReq.cAccount), pAccount);

	short sMyIdent = UserInfoMgr::GetInstance()->GetUserIdent();
	string strMyAccount = UserInfoMgr::GetInstance()->GetUserAccount();
	if (uIdent > sMyIdent)
	{
		printf("%s 无法更改身份标识比你高的用户\n", __FUNCTION__);
		SetIEndFlag(-1);
		return;
	}

	bool bIsMe = false;
	if (uIdent == sMyIdent && !strMyAccount.empty() && strMyAccount == string(pAccount))
	{
		bIsMe = true;
	}

	int iChooseFieldID = -1;
	if (sMyIdent == 1) //学生更改自己的用户信息
	{
		if ((iChooseFieldID=GetChooseUserInfoFieldId("1|2|3|4")) < 1) //可更改 1-account 2-password 3-name 4-sex
		{
			OperInputErrorHandle(false);
			return;
		}
	}
	else if(sMyIdent == 2)//教师更改对象用户信息
	{
		if (bIsMe) //教师更改自己的用户信息
		{
			if ((iChooseFieldID=GetChooseUserInfoFieldId("1|2|3|4")) < 1) //可更改 1-account 2-password 3-name 4-sex
			{
				OperInputErrorHandle(false);
				return;
			}
		}
		else if (uIdent == 1) //教师更改学生的用户信息
		{
			if ((iChooseFieldID=GetChooseUserInfoFieldId("1|3|4|6|7")) < 1) //可更改 1-account 3-name 4-sex 6-major 7-grade
			{
				OperInputErrorHandle(false);
				return;
			}
		}
		else
		{
			printf("%s 教师无法更改其他教师的用户信息\n", __FUNCTION__);
			SetIEndFlag(-1);
			return;
		}
	}
	else if(sMyIdent == 3)//管理员更改对象用户信息
	{
		if (bIsMe) //管理员更改自己的用户信息
		{
			if ((iChooseFieldID=GetChooseUserInfoFieldId("1|2|3|4|6|7")) < 1) //可更改 1-account 2-password 3-name 4-sex 6-major 7-grade
			{
				OperInputErrorHandle(false);
				return;
			}
		}
		else if (uIdent == 1 || uIdent == 2) //管理员更改学生或教师的用户信息
		{
			if ((iChooseFieldID=GetChooseUserInfoFieldId("1|2|3|4|5|6|7")) < 1) //可更改 1-account 2-password 3-name 4-sex 5-Ident 6-major 7-grade
			{
				OperInputErrorHandle(false);
				return;
			}
		}
		else //不会走这里，管理员只有一个。只是为了安全才做这个判断
		{
			printf("%s 管理员无法更改其他管理员的用户信息\n", __FUNCTION__); 
			SetIEndFlag(-1);
			return;
		}
	}
	else
	{
		printf("%s 用户本人身边标识sMyIdent[%u]异常\n", __FUNCTION__, sMyIdent);
		SetIEndFlag(-1);
		return;
	}

	if (iChooseFieldID < 1)
	{
		printf("%s 字段Id不正确\n", __FUNCTION__, sMyIdent);
		SetIEndFlag(-1);
		return;
	}

	//展示旧的用户信息
	printf("现在%s的%s为[%s]\n", bIsMe?"您":"该对象", GetChineseUserInfoFieldByField(iChooseFieldID).c_str(), GetOldUserInfoFieldValue(pAccount, pPwd, pName, uSex, uIdent, pMajor, pGrade, iChooseFieldID).c_str());


	//输入新的用户信息
	printf("请输入%s新的%s（性别：0男1女 身份标识：1学生2教师）：\n", bIsMe?"您":"对象", GetChineseUserInfoFieldByField(iChooseFieldID).c_str());
	string strNewFieldValue;
	cin>>strNewFieldValue;
	if (!(CheckUserInfoFieldValue(strNewFieldValue, iChooseFieldID)))
	{
		OperInputErrorHandle(false);
		return;
	}

	if (bIsMe && IsPwdUserInfoField(iChooseFieldID)) //如果是更改本人自己的密码，需要确认一次输入
	{
		printf("请再次输入密码：\n");
		string strNewFieldValueAgain;
		cin>>strNewFieldValueAgain;
		if (!(CheckUserInfoFieldValue(strNewFieldValueAgain, iChooseFieldID) && strNewFieldValue==strNewFieldValueAgain))
		{
			cout<<"再次输入的密码与第一次不相同"<<endl;
			OperInputErrorHandle(false);
			return;
		}
	}

	//保存临时数据
	m_bUserInfoFieldCount = 1;
	m_sUserInfoField[0] = (unsigned char)iChooseFieldID;
	strcpy_s(m_cUserInfoValue[0], sizeof(m_cUserInfoValue[0]), strNewFieldValue.c_str());
	/////////

	SendReq.bUserInfoFieldCount = 1;
	SendReq.sUserInfoField[0] = (unsigned char)iChooseFieldID;
	strcpy_s(SendReq.cUserInfoValue[0], sizeof(SendReq.cUserInfoValue[0]), strNewFieldValue.c_str());
	TCPHandle::GetInstance()->Send(&SendReq, sizeof(SendReq), /*MAIN_ID_LOGINREGISTER,*/ ASSIST_ID_UPDATE_SINGLE_USERINFO_REQ);
}

void UpdateUserInfoSysProc::UpdateSingleUserInfoByFixedFieldChooseHandle(unsigned uUserID, char* pAccount, char* pPwd, char* pName, unsigned char uSex, unsigned char uIdent, char* pMajor, char* pGrade)
{
	if (uUserID < 1 || NULL == pAccount || uIdent < 1 || uIdent > 3)
	{
		printf("%s 用户ID为0或账号为NULL或身边标识uIdent[%u]异常\n", __FUNCTION__, uIdent);
		SetIEndFlag(-1);
		return;
	}

	CS_MSG_UPDATE_SINGLE_USERINFO_REQ SendReq;
	SendReq.sType = 2;
	strcpy_s(SendReq.cAccount, sizeof(SendReq.cAccount), pAccount);

	short sMyIdent = UserInfoMgr::GetInstance()->GetUserIdent();
	string strMyAccount = UserInfoMgr::GetInstance()->GetUserAccount();
	if (uIdent > sMyIdent)
	{
		printf("%s 无法更改身份标识比你高的用户\n", __FUNCTION__);
		SetIEndFlag(-1);
		return;
	}

	bool bIsMe = false;
	if (uIdent == sMyIdent && !strMyAccount.empty() && strMyAccount == string(pAccount))
	{
		bIsMe = true;
	}

	map<int, string> mapNewAllFieldValue;
	string strFeild;
	if (sMyIdent == 1) //学生更改自己的用户信息
	{
		 strFeild = "1|2|3|4";
		 vector<string> strVecFeild = StringTool::Splite(strFeild);
		 printf("现在%s的用户信息：\n", bIsMe?"您":"该对象");
		 for (unsigned i=0; i<strVecFeild.size(); i++)
		 {
			 if (i > 0)
			 {
				 cout<<"\t";
			 }
			 int iFeildId =  (int)atoi(strVecFeild.at(i).c_str());
			 cout<<GetChineseUserInfoFieldByField(iFeildId)<<"（"<<GetOldUserInfoFieldValue(pAccount, pPwd, pName, uSex, uIdent, pMajor, pGrade, iFeildId)<<"）";
			 if (i == strVecFeild.size()-1)
			 {
				 cout<<endl;
			 }
		 }

		if (!InputAllUserInfoFieldValues(strFeild, mapNewAllFieldValue)) //可更改 1-account 2-password 3-name 4-sex
		{
			OperInputErrorHandle(false);
			return;
		}

		//printf("\n");
	}
	else if(sMyIdent == 2)//教师更改对象用户信息
	{
		if (bIsMe) //教师更改自己的用户信息
		{
			strFeild = "1|2|3|4";
			vector<string> strVecFeild = StringTool::Splite(strFeild);
			printf("现在%s的用户信息：\n", bIsMe?"您":"该对象");
			for (unsigned i=0; i<strVecFeild.size(); i++)
			 {
				 if (i > 0)
				 {
					 cout<<"\t";
				 }
				 int iFeildId =  (int)atoi(strVecFeild.at(i).c_str());
				 cout<<GetChineseUserInfoFieldByField(iFeildId)<<"（"<<GetOldUserInfoFieldValue(pAccount, pPwd, pName, uSex, uIdent, pMajor, pGrade, iFeildId)<<"）";
				 if (i == strVecFeild.size()-1)
				 {
					 cout<<endl;
				 }
			 }

			if (!InputAllUserInfoFieldValues(strFeild, mapNewAllFieldValue)) //可更改 1-account 2-password 3-name 4-sex
			{
				OperInputErrorHandle(false);
				return;
			}
		}
		else if (uIdent == 1) //教师更改学生的用户信息
		{
			strFeild = "1|3|4|6|7";
			vector<string> strVecFeild = StringTool::Splite(strFeild);
			printf("现在%s的用户信息：\n", bIsMe?"您":"该对象");
			for (unsigned i=0; i<strVecFeild.size(); i++)
			{
				if (i > 0)
				{
					cout<<"\t";
				}
				int iFeildId =  (int)atoi(strVecFeild.at(i).c_str());
				cout<<GetChineseUserInfoFieldByField(iFeildId)<<"（"<<GetOldUserInfoFieldValue(pAccount, pPwd, pName, uSex, uIdent, pMajor, pGrade, iFeildId)<<"）";
				if (i == strVecFeild.size()-1)
			 {
				 cout<<endl;
			 }
			}

			if (!InputAllUserInfoFieldValues(strFeild, mapNewAllFieldValue)) //可更改 1-account 3-name 4-sex 6-major 7-grade
			{
				OperInputErrorHandle(false);
				return;
			}
		}
		else
		{
			printf("%s 教师无法更改其他教师的用户信息\n", __FUNCTION__);
			SetIEndFlag(-1);
			return;
		}
	}
	else if(sMyIdent == 3)//管理员更改对象用户信息
	{
		if (bIsMe) //管理员更改自己的用户信息
		{
			strFeild = "1|2|3|4|6|7";
			vector<string> strVecFeild = StringTool::Splite(strFeild);
			printf("现在%s的用户信息：\n", bIsMe?"您":"该对象");
			for (unsigned i=0; i<strVecFeild.size(); i++)
			{
				if (i > 0)
				{
					cout<<"\t";
				}
				int iFeildId =  (int)atoi(strVecFeild.at(i).c_str());
				cout<<GetChineseUserInfoFieldByField(iFeildId)<<"（"<<GetOldUserInfoFieldValue(pAccount, pPwd, pName, uSex, uIdent, pMajor, pGrade, iFeildId)<<"）";
				if (i == strVecFeild.size()-1)
				 {
					 cout<<endl;
				 }
			}

			if (!InputAllUserInfoFieldValues(strFeild, mapNewAllFieldValue)) //可更改 1-account 2-password 3-name 4-sex 6-major 7-grade
			{
				OperInputErrorHandle(false);
				return;
			}
		}
		else if (uIdent == 1 || uIdent == 2) //管理员更改学生或教师的用户信息
		{
			strFeild = "1|2|3|4|5|6|7";
			vector<string> strVecFeild = StringTool::Splite(strFeild);
			printf("现在%s的用户信息：\n", bIsMe?"您":"该对象");
			for (unsigned i=0; i<strVecFeild.size(); i++)
			{
				if (i > 0)
				{
					cout<<"\t";
				}
				int iFeildId =  (int)atoi(strVecFeild.at(i).c_str());
				cout<<GetChineseUserInfoFieldByField(iFeildId)<<"（"<<GetOldUserInfoFieldValue(pAccount, pPwd, pName, uSex, uIdent, pMajor, pGrade, iFeildId)<<"）";
				if (i == strVecFeild.size()-1)
				 {
					 cout<<endl;
				 }
			}


			if (!InputAllUserInfoFieldValues(strFeild, mapNewAllFieldValue)) //可更改 1-account 2-password 3-name 4-sex 5-Ident 6-major 7-grade
			{
				OperInputErrorHandle(false);
				return;
			}
		}
		else //不会走这里，管理员只有一个。只是为了安全才做这个判断
		{
			printf("%s 管理员无法更改其他管理员的用户信息\n", __FUNCTION__); 
			SetIEndFlag(-1);
			return;
		}
	}
	else
	{
		printf("%s 用户本人身边标识sMyIdent[%u]异常\n", __FUNCTION__, sMyIdent);
		SetIEndFlag(-1);
		return;
	}

	if (mapNewAllFieldValue.empty())
	{
		printf("%s 输入为空\n", __FUNCTION__);
		SetIEndFlag(-1);
		return;
	}

	m_bUserInfoFieldCount = 0;//保存临时数据
	SendReq.bUserInfoFieldCount = 0;
	for (map<int, string>::iterator iter=mapNewAllFieldValue.begin(); iter!=mapNewAllFieldValue.end(); iter++, SendReq.bUserInfoFieldCount++, m_bUserInfoFieldCount++)
	{
		//保存临时数据
		m_sUserInfoField[m_bUserInfoFieldCount] = iter->first;
		strcpy_s(m_cUserInfoValue[m_bUserInfoFieldCount], sizeof(m_cUserInfoValue[m_bUserInfoFieldCount]), iter->second.c_str());
		////

		SendReq.sUserInfoField[SendReq.bUserInfoFieldCount] = iter->first;
		strcpy_s(SendReq.cUserInfoValue[SendReq.bUserInfoFieldCount], sizeof(SendReq.cUserInfoValue[SendReq.bUserInfoFieldCount]), iter->second.c_str());
	}

	TCPHandle::GetInstance()->Send(&SendReq, sizeof(SendReq), /*MAIN_ID_LOGINREGISTER,*/ ASSIST_ID_UPDATE_SINGLE_USERINFO_REQ);
}

bool UpdateUserInfoSysProc::GetUserInfoAfterUpdateSingleUserInfoRecvHandle(void* vpData, unsigned int DataLen)
{
	if (OPER_PER_UPDATESINGLEUSERINFOEBYONE != GetCurOper() && OPER_PER_UPDATESINGLEUSERINFOBYMORE != GetCurOper())
	{
		printf("%s 不是进行该操作[%d | %d]，当前进行的操作是[%d] error\n", __FUNCTION__, OPER_PER_UPDATESINGLEUSERINFOEBYONE, OPER_PER_UPDATESINGLEUSERINFOBYMORE, GetCurOper());
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

	if (OPER_PER_UPDATESINGLEUSERINFOEBYONE == GetCurOper())
	{
		UpdateSingleUserInfoByOneFieldChooseHandle(RecvMSG->uUserID, RecvMSG->cAccount, RecvMSG->cPWD, RecvMSG->cName, RecvMSG->sSex, RecvMSG->sIdent, RecvMSG->cMajor, RecvMSG->cGrade);
	}
	else
	{
		UpdateSingleUserInfoByFixedFieldChooseHandle(RecvMSG->uUserID, RecvMSG->cAccount, RecvMSG->cPWD, RecvMSG->cName, RecvMSG->sSex, RecvMSG->sIdent, RecvMSG->cMajor, RecvMSG->cGrade);
	}

	return true;
}

bool UpdateUserInfoSysProc::UpdateSingleUserInfoRecvHandle(void* vpData, unsigned int DataLen)
{
	if (OPER_PER_UPDATESINGLEUSERINFOEBYONE != GetCurOper() && OPER_PER_UPDATESINGLEUSERINFOBYMORE != GetCurOper())
	{
		printf("%s 不是进行该操作[%d | %d]，当前进行的操作是[%d] error\n", __FUNCTION__, OPER_PER_UPDATESINGLEUSERINFOEBYONE, OPER_PER_UPDATESINGLEUSERINFOBYMORE, GetCurOper());
		SetIEndFlag(-1);
		return false;
	}
	if (DataLen != sizeof(SC_MSG_UPDATE_SINGLE_USERINFO_ACK))
	{
		printf("DataLen[%u] error, It should be [%u]\n", DataLen, sizeof(SC_MSG_UPDATE_SINGLE_USERINFO_ACK));
		SetIEndFlag(-1);
		return false;
	}

	SC_MSG_UPDATE_SINGLE_USERINFO_ACK* RecvMSG = (SC_MSG_UPDATE_SINGLE_USERINFO_ACK*) vpData;
	if (RecvMSG->bSucceed)
	{
		string strTmp;
		if (UserInfoMgr::GetInstance()->GetUserAccount()==string(RecvMSG->cAccount))
		{
			strTmp = "自己";

			for (unsigned char i=0; i<m_bUserInfoFieldCount; i++)
			{
				if (m_sUserInfoField[i] == 1)
				{
					UserInfoMgr::GetInstance()->SetUserAccount(m_cUserInfoValue[i]);
				}
				else if (m_sUserInfoField[i] == 3)
				{
					UserInfoMgr::GetInstance()->SetUserName(m_cUserInfoValue[i]);
				}
				else if (m_sUserInfoField[i] == 4)
				{
					UserInfoMgr::GetInstance()->SetUserSex((short)atoi(m_cUserInfoValue[i]));
				}
			}	
		}
		else
		{
			strTmp = string("账号为[") + RecvMSG->cAccount + "]";
		}

		printf("更新%s的单条%s用户信息成功\n", strTmp.c_str(), RecvMSG->sType==1?"单字段":(RecvMSG->sType==2?"固定字段":"NULL"));
		SetIEndFlag(1);
	}
	else
	{
		SetIEndFlag(-1);
		printf("更改用户信息失败\n");
		return false;
	}
	
	return true;
}

void UpdateUserInfoSysProc::InitUserInfoFieldName()
{
	m_mUserInfoFieldName.insert(pair<int, string>(++m_maxUserInfoFieldID, "account|账号"));
	m_mUserInfoFieldName.insert(pair<int, string>(++m_maxUserInfoFieldID, "password|密码"));
	m_mUserInfoFieldName.insert(pair<int, string>(++m_maxUserInfoFieldID, "name|姓名"));
	m_mUserInfoFieldName.insert(pair<int, string>(++m_maxUserInfoFieldID, "sex|性别"));
	m_mUserInfoFieldName.insert(pair<int, string>(++m_maxUserInfoFieldID, "Ident|身份标识"));
	m_mUserInfoFieldName.insert(pair<int, string>(++m_maxUserInfoFieldID, "major|专业"));
	m_mUserInfoFieldName.insert(pair<int, string>(++m_maxUserInfoFieldID, "grade|班级"));
}

string UpdateUserInfoSysProc::GetEnglishUserInfoFieldByField(int iFeildId)
{
	if (iFeildId < 1 || iFeildId > m_maxUserInfoFieldID)
	{
		return "";
	}

	string strFieldName = m_mUserInfoFieldName[iFeildId];
	vector<string> strVec = StringTool::Splite(strFieldName);
	if (strVec.size() != 2)
	{
		return "";
	}

	return strVec.at(0);
}

string UpdateUserInfoSysProc::GetChineseUserInfoFieldByField(int iFeildId)
{
	if (iFeildId < 1 || iFeildId > m_maxUserInfoFieldID)
	{
		return "";
	}

	string strFieldName = m_mUserInfoFieldName[iFeildId];
	vector<string> strVec = StringTool::Splite(strFieldName);
	if (strVec.size() != 2)
	{
		return "";
	}

	return strVec.at(1);
}

bool UpdateUserInfoSysProc::IsPwdUserInfoField(int iField)
{
	return iField==2 ? true : false;
}

int UpdateUserInfoSysProc::GetChooseUserInfoFieldId(string strShowFieldID, int iField)
{
	if (strShowFieldID.empty())
	{
		return -1;
	}

	vector<string> strVecFieldId = StringTool::Splite(strShowFieldID);
	map<unsigned, string> strMapFieldNameChoose; //选项序号与字段id建立联系
	for (unsigned i=0; i<strVecFieldId.size(); i++)
	{
		strMapFieldNameChoose.insert(pair<unsigned, string>(i+1, strVecFieldId.at(i)));
	}

	unsigned iCount = 0;
	char chTmp[51];
	for (map<unsigned, string>::iterator iter=strMapFieldNameChoose.begin(); iter!=strMapFieldNameChoose.end(); iter++, iCount++)
	{
		if (iCount != 0 && iCount % iField == 0)
		{
			cout<<endl;
		}

		memset(chTmp, 0, sizeof(chTmp));
		sprintf_s(chTmp, sizeof(chTmp), "%u-%s", iter->first, GetChineseUserInfoFieldByField((int)atoi(iter->second.c_str())).c_str());
		cout<<StringTool::SetStringFieldWidth(chTmp, 16);	
		
	}
	if (iCount > 0)
	{
		cout<<endl;
	}
	
	cout<<"请选择需更改的用户字段："<<endl;
	string strFieldChoose;
	cin>>strFieldChoose;
	if (!(CheckTool::CheckStringLen(strFieldChoose, 1) && CheckTool::CheckStringByValid(strFieldChoose, "0~9") && strMapFieldNameChoose.find((unsigned)atoi(strFieldChoose.c_str()))!=strMapFieldNameChoose.end()))
	{
		return -1;
	}

	return (int)atoi(strMapFieldNameChoose[(unsigned)atoi(strFieldChoose.c_str())].c_str());
}

bool UpdateUserInfoSysProc::InputAllUserInfoFieldValues(string strShowFieldID, map<int, string>& mapNewAllFieldValue, int iField)
{
	mapNewAllFieldValue.clear();
	if (strShowFieldID.empty())
	{
		return false;
	}

	vector<string> strVecFieldId = StringTool::Splite(strShowFieldID);
	map<unsigned, string> strMapFieldNameChoose; //选项序号与字段id建立联系
	for (unsigned i=0; i<strVecFieldId.size(); i++)
	{
		strMapFieldNameChoose.insert(pair<unsigned, string>(i+1, strVecFieldId.at(i)));
	}

	unsigned iCount = 0;
	char chTmp[51];
	for (map<unsigned, string>::iterator iter=strMapFieldNameChoose.begin(); iter!=strMapFieldNameChoose.end(); iter++, iCount++)
	{
		if (iCount != 0 && iCount % iField == 0)
		{
			cout<<endl;
		}

		memset(chTmp, 0, sizeof(chTmp));
		sprintf_s(chTmp, sizeof(chTmp), "%u-%s", iter->first, GetChineseUserInfoFieldByField((int)atoi(iter->second.c_str())).c_str());
		cout<<StringTool::SetStringFieldWidth(chTmp, 16);	
	}
	if (iCount > 0)
	{
		cout<<endl;
	}

	printf("按照从左到右和从上到下的顺序输入对应新的用户信息（格式为 XX|XX|XX。 性别：0男1女 身份标识：1学生2教师）：\n");
	string strNewAllFieldValue;
	cin>>strNewAllFieldValue;
	vector<string> vecNewAllFieldValue = StringTool::Splite(strNewAllFieldValue);
	if (vecNewAllFieldValue.size() != strMapFieldNameChoose.size())
	{
		return false;
	}

	for (unsigned j=0; j<vecNewAllFieldValue.size(); j++)
	{
		int iFieldId = (int)atoi(strMapFieldNameChoose[j+1].c_str());
		if (!(CheckUserInfoFieldValue(vecNewAllFieldValue.at(j), iFieldId)))
		{
			mapNewAllFieldValue.clear();
			return false;
		}
		mapNewAllFieldValue.insert(pair<int, string>(iFieldId, vecNewAllFieldValue.at(j)));
	}

	return true;
}

bool UpdateUserInfoSysProc::CheckUserInfoFieldValue(string strFeildValue, int iFieldId)
{
	bool bRes = true;
	if (iFieldId == 1) //1-account
	{
		if (!(CheckTool::CheckStringLen(strFeildValue, 30) && CheckTool::CheckStringByValid(strFeildValue, "A~Z|a~z|0~9")))
		{
			bRes = false;
		}
	}
	else if (iFieldId == 2) //2-password
	{
		if (!(CheckTool::CheckStringLen(strFeildValue, 30) && CheckTool::CheckStringByValid(strFeildValue, "A~Z|a~z|0~9|_|-")))
		{
			bRes = false;
		}
	}
	else if (iFieldId == 3) //3-name
	{
		if (!(CheckTool::CheckStringLen(strFeildValue, 30) && CheckTool::CheckStringByValid(strFeildValue, "")))
		{
			bRes = false;
		}
	}
	else if (iFieldId == 4) //4-sex
	{
		if (!(CheckTool::CheckStringLen(strFeildValue, 1) && CheckTool::CheckStringByValid(strFeildValue, "0|1")))
		{
			bRes = false;
		}
	}
	else if (iFieldId == 5) //5-Ident
	{
		if (!(CheckTool::CheckStringLen(strFeildValue, 1) && CheckTool::CheckStringByValid(strFeildValue, "1|2|3")))
		{
			bRes = false;
		}
	}
	else if (iFieldId == 6) //6-major
	{
		if (!(CheckTool::CheckStringLen(strFeildValue, 40) && CheckTool::CheckStringByValid(strFeildValue, "")))
		{
			bRes = false;
		}	
	}
	else if (iFieldId == 7) //7-grade
	{
		if (!(CheckTool::CheckStringLen(strFeildValue, 30) && CheckTool::CheckStringByValid(strFeildValue, "")))
		{
			bRes = false;
		}	
	}
	else
	{
		bRes = false;
	}

	return bRes;
}

string UpdateUserInfoSysProc::GetOldUserInfoFieldValue(char* pAccount, char* pPwd, char* pName, unsigned char uSex, unsigned char uIdent, char* pMajor, char* pGrade, int iFieldId)
{
	if (iFieldId == 1)
	{
		return NULL==pAccount ? "NULL" : pAccount;
	}
	else if (iFieldId == 2)
	{
		return NULL==pPwd ? "NULL" : pPwd;
	}
	else if (iFieldId == 3)
	{
		return NULL==pName ? "NULL" : pName;
	}
	else if (iFieldId == 4)
	{
		return 0==uSex ? "0[男]" : (1==uSex ? "1[女]" : "NULL");
	}
	else if (iFieldId == 5)
	{
		return 1==uIdent ? "1[学生]" : (2==uIdent ? "2[教师]" : (3==uIdent ? "3[管理员]" : "NULL"));
	}
	else if (iFieldId == 6)
	{
		return NULL==pMajor ? "NULL" : pMajor;
	}
	else if (iFieldId == 7)
	{
		return NULL==pGrade ? "NULL" : pGrade;
	}
	else
	{
		return "NULL";
	}
}
