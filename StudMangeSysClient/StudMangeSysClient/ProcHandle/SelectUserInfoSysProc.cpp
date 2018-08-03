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
	CS_MSG_SELECT_BATCH_USERINFO_REQ SendReq;

	cout<<"是否需要条件查询查询（1--是  2--否）："<<endl;
	string strConditonChoose;
	string strConditon;
	cin>>strConditonChoose;
	if (!(CheckTool::CheckStringLen(strConditonChoose, 1) && CheckTool::CheckStringByValid(strConditonChoose, "1|2")))
	{
		OperInputErrorHandle();
		return;
	}
	else
	{
		if (strConditonChoose == "1") 
		{
			printf("请选择查询条件（输入格式为 X|X|X）[选项：%d-%s  %d-%s  %d-%s  %d-%s]：\n", SELECT_USERINFO_RANGE, "用户ID范围", SELECT_USERINFO_GRADE, "班级", SELECT_USERINFO_PART_NAME, "姓名模糊查询", SELECT_USERINFO_SEX, "性别");
			cin>>strConditon;
			if (!(CheckTool::CheckStringLen(strConditon, SELECT_USERINFO_END*2-3) && CheckStringVaildRemoveSpl(strConditon, "|", "1|2|3|4"))) 
			{
				OperInputErrorHandle();
				return;
			}
		}
	}

	unsigned cConditionCount = 0;
	if (strConditon.empty())
	{
		memset(SendReq.cCondition, 0, sizeof(SendReq.cCondition));
	}
	else
	{
		vector<string> vecStrConditon = StringTool::Splite(strConditon, "|");
		if (vecStrConditon.size() >= SELECT_USERINFO_END)
		{
			printf("%s 查询条件错误\n", __FUNCTION__);
			OperInputErrorHandle();
			return;
		}
		for (; cConditionCount<vecStrConditon.size() && cConditionCount<SELECT_USERINFO_END-1; cConditionCount++)
		{
			SendReq.cCondition[cConditionCount] = (unsigned char)atoi(vecStrConditon.at(cConditionCount).c_str());
		}
	}

	SendReq.uUserIdRange[2] = 0;
	if (FindOneInArray(SendReq.cCondition, cConditionCount, SELECT_USERINFO_END-1, SELECT_USERINFO_RANGE))
	{
		printf("请输入需要查询的用户id范围（格式 XXXXXX-XXXXXX）：\n");
		string strScoreRange;
		cin>>strScoreRange;
		vector<string> vecScoreRange = StringTool::Splite(strScoreRange, "-");
		if (!(vecScoreRange.size()==2 &&  CheckTool::CheckStringLen(vecScoreRange.at(0), 6) &&  CheckTool::CheckStringLen(vecScoreRange.at(1), 6) && \
			CheckTool::CheckStringByValid(vecScoreRange.at(0), "0~9") && CheckTool::CheckStringByValid(vecScoreRange.at(1), "0~9") && \
			(unsigned)atoi(vecScoreRange.at(0).c_str())>=100000 && (unsigned)atoi(vecScoreRange.at(0).c_str())<=999999 && \
			(unsigned)atoi(vecScoreRange.at(1).c_str())>=100000 && (unsigned)atoi(vecScoreRange.at(1).c_str())<=999999 && \
			(unsigned)atoi(vecScoreRange.at(0).c_str())<=(unsigned)atoi(vecScoreRange.at(1).c_str())))
		{
			OperInputErrorHandle();
			return;
		}
		SendReq.uUserIdRange[0] = (unsigned char)atoi(vecScoreRange.at(0).c_str());
		SendReq.uUserIdRange[1] = (unsigned char)atoi(vecScoreRange.at(1).c_str());
		SendReq.uUserIdRange[2] = 1;
	}

	if (FindOneInArray(SendReq.cCondition, cConditionCount, SELECT_USERINFO_END-1, SELECT_USERINFO_GRADE))
	{
		printf("请您输入需要查询的班级名称（如 机电11-1）：\n");
		string strGrade;
		cin>>strGrade;
		if (!CheckTool::CheckStringLen(strGrade, 31))
		{
			OperInputErrorHandle();
			return;
		}
		strcpy_s(SendReq.cGrade, sizeof(SendReq.cGrade), strGrade.c_str());
	}

	if (FindOneInArray(SendReq.cCondition, cConditionCount, SELECT_USERINFO_END-1, SELECT_USERINFO_PART_NAME))
	{
		printf("请您输入需要模糊查姓名的字组（如 张三 可以只输入 张）：\n");
		string strName;
		cin>>strName;
		if (!CheckTool::CheckStringLen(strName, 31))
		{
			OperInputErrorHandle();
			return;
		}
		strcpy_s(SendReq.cName, sizeof(SendReq.cName), strName.c_str());
	}

	if (FindOneInArray(SendReq.cCondition, cConditionCount, SELECT_USERINFO_END-1, SELECT_USERINFO_SEX))
	{
		printf("请您选择查询的性别（0-男  1-女）：\n");
		string strSex;
		cin>>strSex;
		if (!(CheckTool::CheckStringLen(strSex, 1) && CheckTool::CheckStringByValid(strSex, "0|1")))
		{
			OperInputErrorHandle();
			return;
		}
		SendReq.cSex = (unsigned char)atoi(strSex.c_str());
	}


	TCPHandle::GetInstance()->Send(&SendReq, sizeof(SendReq), /*MAIN_ID_LOGINREGISTER,*/ ASSIST_ID_SELECT_BATCH_USERINFO_REQ);
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
	if (OPER_PER_SELECTBATCHUSERINFO != GetCurOper())
	{
		printf("%s 不是进行该操作[%d]，当前进行的操作是[%d] error\n", __FUNCTION__, OPER_PER_SELECTBATCHUSERINFO, GetCurOper());
		SetIEndFlag(-1);
		return false;
	}
	if (DataLen != sizeof(SC_MSG_SELECT_BATCH_USERINFO_ACK))
	{
		printf("DataLen[%u] error, It should be [%u]\n", DataLen, sizeof(SC_MSG_SELECT_BATCH_USERINFO_ACK));
		SetIEndFlag(-1);
		return false;
	}

	SC_MSG_SELECT_BATCH_USERINFO_ACK* RecvMSG = (SC_MSG_SELECT_BATCH_USERINFO_ACK*) vpData;
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

	unsigned char iDataRecordCount = RecvMSG->bDataRecord[0];
	if (iDataRecordCount > MAXBATCHREQACKCOUNT)
	{
		printf("%s 单次数据记录过多\n", __FUNCTION__);
		SetIEndFlag(-1);
		return false;
	}
	if (RecvMSG->bResCode == 0 && iDataRecordCount == 0 && RecvMSG->bDataRecord[2] == 1) //出现这种情况，一般是刚好达到MAXBATCHREQACKCOUNT的倍率条记录，这个是为了补发结束标志
	{
		SetIEndFlag(1);
		return true;
	}

	bool bShow = false;
	if (RecvMSG->bDataRecord[1] == 1)
	{
		bShow = true;
	}

	for (unsigned char i=0; i<iDataRecordCount; i++)
	{
		vector<string> vSelectFieldShow;
		vector<string> vSelectResultShow;

		vSelectFieldShow.push_back("学号");
		vSelectResultShow.push_back(StringTool::NumberToStr((int)RecvMSG->uUserID[i]));
		vSelectFieldShow.push_back("姓名");
		vSelectResultShow.push_back(RecvMSG->cName[i]);
		vSelectFieldShow.push_back("账号");
		vSelectResultShow.push_back(RecvMSG->cAccount[i]);
		vSelectFieldShow.push_back("班级");
		vSelectResultShow.push_back(RecvMSG->cGrade[i]);
		vSelectFieldShow.push_back("密码");
		vSelectResultShow.push_back(RecvMSG->cPWD[i]);//由服务端做屏蔽处理
		vSelectFieldShow.push_back("性别");
		vSelectResultShow.push_back(RecvMSG->sSex[i]==0?"男":(RecvMSG->sSex[i]==1?"女":"NULL"));
		vSelectFieldShow.push_back("身份标识");
		vSelectResultShow.push_back(RecvMSG->sIdent[i]==1?"学生":(RecvMSG->sSex[i]==2?"教师":(RecvMSG->sSex[i]==3?"管理员":"NULL")));
		vSelectFieldShow.push_back("专业");
		vSelectResultShow.push_back(RecvMSG->cMajor[i]);

		//显示查询成绩
		ShowSelectSingleScoreResult(vSelectFieldShow, vSelectResultShow, bShow);
		if (bShow)
		{
			bShow = false;
		}
	}

	//结束
	if (RecvMSG->bDataRecord[2] == 1)
	{
		printf(">>总查询到%u条数据\n", (RecvMSG->bDataRecord[1]-1)*MAXBATCHREQACKCOUNT + RecvMSG->bDataRecord[0]);
		SetIEndFlag(1);
	}

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

bool SelectUserInfoSysProc::CheckStringVaildRemoveSpl(string str, string strSpl, string strVaild)
{
	vector<string> vecStr = StringTool::Splite(str, strSpl);
	string strTmp;
	for (unsigned i=0; i<vecStr.size(); i++)
	{
		strTmp += vecStr.at(i);
	}

	return CheckTool::CheckStringByValid(strTmp, strVaild);
}

bool SelectUserInfoSysProc::FindOneInArray(unsigned char* pArray, unsigned char iCount, unsigned char iMaxCount, unsigned char Obj)
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