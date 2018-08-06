#include "UpdateScoreSysProc.h"
#include "TCPHandle.h"
#include "StringTool.h"
#include "CheckTool.h"
#include "ProcMgr.h"
#include "UserInfoMgr.h"

UpdateScoreSysProc::UpdateScoreSysProc(ProcDef nProcDef) : BaseProc(nProcDef)
{
	initMapChoose();
}

UpdateScoreSysProc::~UpdateScoreSysProc()
{

}

bool UpdateScoreSysProc::initMapChoose()
{
	//m_mChoose.insert(pair<int, ChooseData>(GetMaxRealChoose(), ChooseData("单科批量更改成绩", OPER_PER_UPDATEBATCHSCOREBYONESUBJECT, PROC_DEF_UPDATESCORESYSPROC)));
	//m_mChoose.insert(pair<int, ChooseData>(++GetMaxRealChoose(), ChooseData("现有所有科目批量更改成绩", OPER_PER_UPDATEBATCHSCOREBYSUBJECTS, PROC_DEF_UPDATESCORESYSPROC)));
	m_mChoose.insert(pair<int, ChooseData>(GetMaxRealChoose(), ChooseData("单科单条更改成绩", OPER_PER_UPDATESINGLESCOREBYONESUBJECT, PROC_DEF_UPDATESCORESYSPROC)));
	m_mChoose.insert(pair<int, ChooseData>(++GetMaxRealChoose(), ChooseData("现有所有科目单条更改成绩", OPER_PER_UPDATESINGLESCOREBYSUBJECTS, PROC_DEF_UPDATESCORESYSPROC)));
	m_mChoose.insert(pair<int, ChooseData>(++GetMaxRealChoose(), ChooseData("返回", OPER_PER_INVALID, PROC_DEF_SCORESYSPROC)));

	return true;
}

void UpdateScoreSysProc::EndProc()
{
	__super::EndProc();
}

void UpdateScoreSysProc::StartRecv(void* vpData, unsigned int DataLen, /*int iMianId,*/ int iAssistId)
{
	__super::StartRecv(vpData, DataLen, iAssistId);

	bool bRes = false;
	switch(iAssistId)
	{
	case ASSIST_ID_GET_SUBJECTS_ACK:
		bRes = GetSubjectsAfterUpdateSingleScoreRecvHandle(vpData, DataLen);
		break;
	case ASSIST_ID_SELECT_SINGLE_SCORE_ACK:
		bRes = GetSubjectsScoreAfterUpdateSingleScoreRecvHandle(vpData, DataLen);
		break;
	case ASSIST_ID_UPDATE_SINGLE_SCORE_ACK:
		bRes = UpdateSingleScoreRecvHandle(vpData, DataLen);
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

void UpdateScoreSysProc::EndRecv()
{
	__super::EndRecv();
}

void UpdateScoreSysProc::SwitchToOper(OperPermission CurOper)
{
	__super::SwitchToOper(CurOper);


	switch(CurOper)
	{
	case OPER_PER_UPDATESINGLESCOREBYONESUBJECT: //单科单条更改成绩  操作前请求处理
	case OPER_PER_UPDATESINGLESCOREBYSUBJECTS: //现有所有科目单条更改成绩  操作前请求处理
		GetSubjectsAfterUpdateSingleScoreChooseHandle();
		break;
	default:
		printf("%s 没有该操作！\n", __FUNCTION__);
		ProcMgr::GetInstance()->ProcSwitch(GetMyProcDef(), true);  
		break;
	}
}

void UpdateScoreSysProc::GetSubjectsAfterUpdateSingleScoreChooseHandle()
{
	//获取现有科目
	CS_MSG_GET_SUBJECTS_REQ SendReq;
	SendReq.sGetType = 2;

	TCPHandle::GetInstance()->Send(&SendReq, sizeof(SendReq), /*MAIN_ID_LOGINREGISTER,*/ ASSIST_ID_GET_SUBJECTS_REQ);
}

void UpdateScoreSysProc::GetSubjectsScoreAfterUpdateSingleScoreChooseHandle(char* pStrExistSubjects)
{
	if (NULL == pStrExistSubjects)
	{
		printf("%s 科目字段为NULL\n", __FUNCTION__);
		SetIEndFlag(-1);
		return;
	}

	CS_MSG_SELECT_SINGLE_SCORE_REQ SendReq;
	SendReq.sType = 2;

	cout<<"请输入更改分数对象的用户名："<<endl;
	string strAccount;
	cin>>strAccount;
	if (!(CheckTool::CheckStringLen(strAccount, 30) && CheckTool::CheckStringByValid(strAccount, "A~Z|a~z|0~9")))
	{
		OperInputErrorHandle(false);
		return;
	}
	strcpy_s(SendReq.cAccount, sizeof(SendReq.cAccount), strAccount.c_str());

	vector<string> vecStrSubjectId = StringTool::Splite(pStrExistSubjects, "|");
	for (unsigned i=0, j=0; i<vecStrSubjectId.size()&&j<MAXSUBJECTSCOUNT; i++)
	{
		unsigned char sId = (unsigned char)atoi(vecStrSubjectId.at(i).c_str());
		if (UserInfoMgr::GetInstance()->CanFindSubjectsType((SubjectsType)sId))
		{	
			SendReq.sSubjectId[j] = sId;
			SendReq.bSubjectCount = ++j;
		}
		else
		{
			printf("%s 科目ID[%d]不合法\n", __FUNCTION__,(int)sId);
		}
	}
	
	TCPHandle::GetInstance()->Send(&SendReq, sizeof(SendReq), /*MAIN_ID_LOGINREGISTER,*/ ASSIST_ID_SELECT_SINGLE_SCORE_REQ);
}

void UpdateScoreSysProc::UpdateSingleScoreByOneSubjectChooseHandle(char* pName, char* pAccount, char* pGrade, unsigned char* pSubjectsId, unsigned char* pScore, unsigned char bSubjectCount)
{
	if (NULL == pAccount)
	{
		printf("%s 账号为NULL\n", __FUNCTION__);
		SetIEndFlag(-1);
		return;
	}
	if (NULL == pSubjectsId || 0 == bSubjectCount || NULL == pScore)
	{
		printf("%s 科目id或分数为NULL再者被操作用户没有科目字段\n", __FUNCTION__);
		SetIEndFlag(-1);
		return;
	}
	string strName;
	string strGrade;
	if (NULL == pName)
	{
		strName = "NULL";
	}
	else
	{
		strName = pName;
	}
	if (NULL == pGrade)
	{
		strGrade = "NULL";
	}
	else
	{
		strGrade = pGrade;
	}


	//先显示被操作用户现在的分数情况
	printf("班级[%s] 账号[%s] 用户姓名[%s]现有成绩分数情况：\n", strGrade.c_str(), pAccount, strName.c_str());
	ShowSubjects(pSubjectsId, pScore, bSubjectCount, 3);
	printf("请选择您要更改的科目ID：\n");
	string iChooseSubjectsId;
	cin>>iChooseSubjectsId;
	unsigned char bSubjectsId = (unsigned char)atoi(iChooseSubjectsId.c_str());
	if (!CheckTool::CheckStringByValid(iChooseSubjectsId, "0~9") || !FindSubjectsId(pSubjectsId, bSubjectCount, bSubjectsId))
	{
		OperInputErrorHandle(false);
		return;
	}

	printf("请输入该科目新的分数：\n");
	string strScore;
	cin>>strScore;
	if (!(CheckTool::CheckStringLen(strScore, 3) && CheckTool::CheckStringByValid(strScore, "0~9") && (int)atoi(strScore.c_str()) >= 0 && (int)atoi(strScore.c_str()) <= 100))
	{
		OperInputErrorHandle(false);
		return;
	}

	CS_MSG_UPDATE_SINGLE_SCORE_REQ SendReq;
	SendReq.sType = 1;
	SendReq.sSubjectId[0] = bSubjectsId;
	SendReq.bScore[0] = (unsigned char)atoi(strScore.c_str());
	SendReq.bSubjectCount = 1;
	strcpy_s(SendReq.cAccount, sizeof(SendReq.cAccount), pAccount);

	TCPHandle::GetInstance()->Send(&SendReq, sizeof(SendReq), /*MAIN_ID_LOGINREGISTER,*/ ASSIST_ID_UPDATE_SINGLE_SCORE_REQ);
}

void UpdateScoreSysProc::UpdateSingleScoreBySubjectsChooseHandle(char* pName, char* pAccount, char* pGrade, unsigned char* pSubjectsId, unsigned char* pScore, unsigned char bSubjectCount)
{
	if (NULL == pAccount)
	{
		printf("%s 账号为NULL\n", __FUNCTION__);
		SetIEndFlag(-1);
		return;
	}
	if (NULL == pSubjectsId || 0 == bSubjectCount || NULL == pScore)
	{
		printf("%s 科目id或分数为NULL再者被操作用户没有科目字段\n", __FUNCTION__);
		SetIEndFlag(-1);
		return;
	}
	string strName;
	string strGrade;
	if (NULL == pName)
	{
		strName = "NULL";
	}
	else
	{
		strName = pName;
	}
	if (NULL == pGrade)
	{
		strGrade = "NULL";
	}
	else
	{
		strGrade = pGrade;
	}


	//先显示被操作用户现在的分数情况
	printf("班级[%s] 账号[%s] 用户姓名[%s]现有成绩分数情况：\n", strGrade.c_str(), pAccount, strName.c_str());
	ShowSubjects(pSubjectsId, pScore, bSubjectCount, 3);

	printf("按照从左到右和从上到下的顺序输入对应科目的新分数（格式为 XX|XX|XX）：\n");
	string strScore;
	cin>>strScore;
	if (!(CheckTool::CheckStringLen(strScore, 90) && CheckScore(strScore)))
	{
		OperInputErrorHandle(false);
		return;
	}

	unsigned char bScore[MAXSUBJECTSCOUNT];
	StringTool::StrSpliteToUcArray(bScore, MAXSUBJECTSCOUNT, strScore, "|");

	CS_MSG_UPDATE_SINGLE_SCORE_REQ SendReq;
	SendReq.sType = 2;
	SendReq.bSubjectCount = bSubjectCount;
	memcpy(SendReq.sSubjectId, pSubjectsId, sizeof(unsigned char)*MAXSUBJECTSCOUNT);
	memcpy(SendReq.bScore, bScore, sizeof(unsigned char)*MAXSUBJECTSCOUNT);
	strcpy_s(SendReq.cAccount, sizeof(SendReq.cAccount), pAccount);

	TCPHandle::GetInstance()->Send(&SendReq, sizeof(SendReq), /*MAIN_ID_LOGINREGISTER,*/ ASSIST_ID_UPDATE_SINGLE_SCORE_REQ);
}

bool UpdateScoreSysProc::GetSubjectsAfterUpdateSingleScoreRecvHandle(void* vpData, unsigned int DataLen)
{
	if (OPER_PER_UPDATESINGLESCOREBYONESUBJECT != GetCurOper() && OPER_PER_UPDATESINGLESCOREBYSUBJECTS != GetCurOper())
	{
		printf("%s 不是进行该操作[%d | %d]，当前进行的操作是[%d] error\n", __FUNCTION__, OPER_PER_UPDATESINGLESCOREBYONESUBJECT, OPER_PER_UPDATESINGLESCOREBYSUBJECTS, GetCurOper());
		SetIEndFlag(-1);
		return false;
	}
	if (DataLen != sizeof(SC_MSG_GET_SUBJECTS_ACK))
	{
		printf("DataLen[%u] error, It should be [%u]\n", DataLen, sizeof(SC_MSG_GET_SUBJECTS_ACK));
		SetIEndFlag(-1);
		return false;
	}

	SC_MSG_GET_SUBJECTS_ACK* RecvMSG = (SC_MSG_GET_SUBJECTS_ACK*) vpData;
	if (RecvMSG->bSucceed)
	{
		GetSubjectsScoreAfterUpdateSingleScoreChooseHandle(RecvMSG->cCanAlterSubjects);
	}
	else
	{
		printf("%s 可更改科目返回不成功\n", __FUNCTION__);
		SetIEndFlag(-1);
		return false;
	}
	return true;
}

bool UpdateScoreSysProc::GetSubjectsScoreAfterUpdateSingleScoreRecvHandle(void* vpData, unsigned int DataLen)
{
	if (OPER_PER_UPDATESINGLESCOREBYONESUBJECT != GetCurOper() && OPER_PER_UPDATESINGLESCOREBYSUBJECTS != GetCurOper())
	{
		printf("%s 不是进行该操作[%d | %d]，当前进行的操作是[%d] error\n", __FUNCTION__, OPER_PER_UPDATESINGLESCOREBYONESUBJECT, OPER_PER_UPDATESINGLESCOREBYSUBJECTS, GetCurOper());
		SetIEndFlag(-1);
		return false;
	}
	if (DataLen != sizeof(SC_MSG_SELECT_SINGLE_SCORE_ACK))
	{
		printf("%s DataLen[%u] error, It should be [%u]\n", DataLen, sizeof(SC_MSG_SELECT_SINGLE_SCORE_ACK));
		SetIEndFlag(-1);
		return false;
	}
	SC_MSG_SELECT_SINGLE_SCORE_ACK* RecvMSG = (SC_MSG_SELECT_SINGLE_SCORE_ACK*) vpData;

	if (RecvMSG->bResCode == 1)
	{
		printf("%s 单条查询失败\n", __FUNCTION__);
		SetIEndFlag(-1);
		return false;
	}
	else if (RecvMSG->bResCode == 2)
	{
		printf("%s 没有用户信息或者用户分数信息\n", __FUNCTION__);
		SetIEndFlag(-1);
		return false;
	}
	else if (RecvMSG->bResCode == 3)
	{
		printf("%s 其他异常\n", __FUNCTION__);
		SetIEndFlag(-1);
		return false;
	}

	if (OPER_PER_UPDATESINGLESCOREBYONESUBJECT == GetCurOper())
	{
		UpdateSingleScoreByOneSubjectChooseHandle(RecvMSG->cName, RecvMSG->cAccount, RecvMSG->cGrade,RecvMSG->bSubjectId, RecvMSG->bScore, RecvMSG->bSubjectCount);
	}
	else if (OPER_PER_UPDATESINGLESCOREBYSUBJECTS == GetCurOper())
	{
		UpdateSingleScoreBySubjectsChooseHandle(RecvMSG->cName, RecvMSG->cAccount, RecvMSG->cGrade,RecvMSG->bSubjectId, RecvMSG->bScore, RecvMSG->bSubjectCount);
	}
	else
	{
		printf("%s 不是进行该操作[%d | %d]，当前进行的操作是[%d] error\n",__FUNCTION__ ,OPER_PER_UPDATESINGLESCOREBYONESUBJECT, OPER_PER_UPDATESINGLESCOREBYSUBJECTS, GetCurOper());
		SetIEndFlag(-1);
		return false;
	}

	return true;
}

bool UpdateScoreSysProc::UpdateSingleScoreRecvHandle(void* vpData, unsigned int DataLen)
{
	if (OPER_PER_UPDATESINGLESCOREBYONESUBJECT != GetCurOper() && OPER_PER_UPDATESINGLESCOREBYSUBJECTS != GetCurOper())
	{
		printf("%s 不是进行该操作[%d | %d]，当前进行的操作是[%d] error\n", __FUNCTION__, OPER_PER_UPDATESINGLESCOREBYONESUBJECT, OPER_PER_UPDATESINGLESCOREBYSUBJECTS, GetCurOper());
		SetIEndFlag(-1);
		return false;
	}
	if (DataLen != sizeof(SC_MSG_UPDATE_SINGLE_SCORE_ACK))
	{
		printf("%s DataLen[%u] error, It should be [%u]\n", DataLen, sizeof(SC_MSG_UPDATE_SINGLE_SCORE_ACK));
		SetIEndFlag(-1);
		return false;
	}
	SC_MSG_UPDATE_SINGLE_SCORE_ACK* RecvMSG = (SC_MSG_UPDATE_SINGLE_SCORE_ACK*) vpData;

	if (RecvMSG->bSucceed)
	{
		if (RecvMSG->sType == 1)
		{
			printf("给账号[%s]更改单科分数成功\n", RecvMSG->cAccount);
			SetIEndFlag(1);
		}
		else if (RecvMSG->sType == 2)
		{
			printf("给账号[%s]更改现有所有科目分数成功\n", RecvMSG->cAccount);
			SetIEndFlag(1);
		}
		else
		{
			printf("%s  RecvMSG->sType=[%d] error\n", __FUNCTION__, (int)RecvMSG->sType);
			SetIEndFlag(-1);
			return false;
		}
	}
	else
	{
		SetIEndFlag(-1);
		printf("更改分数失败");
		return false;
	}

	return true;
}


bool UpdateScoreSysProc::ShowSubjects(unsigned char* pSubjectsId, unsigned char* pScore, unsigned char bSubjectCount, int iField)
{
	if (!pSubjectsId || !pScore || 0 == bSubjectCount || bSubjectCount>MAXSUBJECTSCOUNT)
	{
		printf("%s 入参有误，bSubjectCount=%u\n", __FUNCTION__, (unsigned)bSubjectCount);
		return false;
	}

	if (iField == 0)
		iField = 0xEFFFFFFF;
	else if (iField < 0)
		iField *= -1;

	m_vecStrChineseName.clear();
	for (unsigned char i=0; i<bSubjectCount; i++)
	{
		if (UserInfoMgr::GetInstance()->CanFindSubjectsType((SubjectsType)pSubjectsId[i]))
		{
			m_vecStrChineseName.push_back(UserInfoMgr::GetInstance()->GetChineseNameByType((SubjectsType)pSubjectsId[i]));
		}
	}

	if (m_vecStrChineseName.size() != bSubjectCount)
	{
		printf("%s 处理后的科目数量和传入的数量不等，传入的科目数量bSubjectCount=%u，处理后的[%u]\n", __FUNCTION__, (unsigned)bSubjectCount, m_vecStrChineseName.size());
		return false;
	}

	unsigned char iCount = 0;
	for (; iCount<bSubjectCount; iCount++)
	{
		if (iCount != 0 && iCount % iField == 0)
		{
			cout<<endl;
		}
		if (iCount != 0 && iCount % iField != 0)
		{
			cout<<"\t\t";
		}
		printf("%d--%s[%u]", pSubjectsId[iCount], m_vecStrChineseName.at(iCount).c_str(), (unsigned)pScore[iCount]);
	}
	if (iCount > 0)
	{
		cout<<endl;
	}

	return true;
}

bool UpdateScoreSysProc::FindSubjectsId(unsigned char* pSubjectsId, unsigned char bSubjectCount, unsigned char SubjectsId)
{
	bool bRes = false;
	if (!pSubjectsId || bSubjectCount==0 || bSubjectCount>MAXSUBJECTSCOUNT)
	{
		return bRes;
	}

	for (unsigned char i=0; i<bSubjectCount; i++)
	{
		if (SubjectsId == pSubjectsId[i])
		{
			bRes = true;
			break;
		}
	}

	return bRes;
}

bool UpdateScoreSysProc::CheckScore(string sScore)
{
	if (sScore.empty())
	{
		return true;
	}

	vector<string> vScore = StringTool::Splite(sScore, "|");

	for (unsigned i=0; i < vScore.size(); i++)
	{	
		if (!CheckTool::CheckStringByValid(vScore.at(i), "0~9"))
		{
			return false;
		}
		int iScore = (int)atoi(vScore.at(i).c_str());
		if (iScore < 0 || iScore > 100)
		{
			return false;
		}
	}

	return true;
}