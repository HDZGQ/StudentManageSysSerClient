#include "SelectScoreSysProc.h"
#include "TCPHandle.h"
#include "ProcMgr.h"
#include "StringTool.h"
#include "UserInfoMgr.h"
#include "CheckTool.h"

SelectScoreSysProc::SelectScoreSysProc(ProcDef nProcDef) : BaseProc(nProcDef)
{
	initMapChoose();
}

SelectScoreSysProc::~SelectScoreSysProc()
{

}

bool SelectScoreSysProc::initMapChoose()
{
	m_mChoose.insert(pair<int, ChooseData>(GetMaxRealChoose(), ChooseData("单科批量查询成绩", OPER_PER_SELECTBATCHSCOREBYONESUBJECT, PROC_DEF_SELECTSCORESYSPROC)));
	m_mChoose.insert(pair<int, ChooseData>(++GetMaxRealChoose(), ChooseData("现有所有科目批量查询成绩", OPER_PER_SELECTBATCHSCOREBYSUBJECTS, PROC_DEF_SELECTSCORESYSPROC)));
	m_mChoose.insert(pair<int, ChooseData>(++GetMaxRealChoose(), ChooseData("单科单条查询成绩", OPER_PER_SELECTSINGLESCOREBYONESUBJECT, PROC_DEF_SELECTSCORESYSPROC)));
	m_mChoose.insert(pair<int, ChooseData>(++GetMaxRealChoose(), ChooseData("现有所有科目单条查询成绩", OPER_PER_SELECTSINGLESCOREBYSUBJECTS, PROC_DEF_SELECTSCORESYSPROC)));
	m_mChoose.insert(pair<int, ChooseData>(++GetMaxRealChoose(), ChooseData("返回", OPER_PER_INVALID, PROC_DEF_SCORESYSPROC)));

	return true;
}

void SelectScoreSysProc::EndProc()
{
	__super::EndProc();
}

void SelectScoreSysProc::StartRecv(void* vpData, unsigned int DataLen, /*int iMianId,*/ int iAssistId)
{
	bool bRes = false;
	switch(iAssistId)
	{
	case ASSIST_ID_GET_SUBJECTS_ACK:
		bRes = GetSubjectsAfterSelectScoreRecvHandle(vpData, DataLen);
		break;
	case ASSIST_ID_SELECT_SINGLE_SCORE_ACK:
		bRes = SelectSingleScoreRecvHandle(vpData, DataLen);
		break;
	case ASSIST_ID_SELECT_BATCH_SCORE_ACK:
		bRes = SelectBatchScoreRecvHandle(vpData, DataLen);
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

void SelectScoreSysProc::EndRecv()
{
	__super::EndRecv();
}

void SelectScoreSysProc::SwitchToOper(OperPermission CurOper)
{
	__super::SwitchToOper(CurOper);


	switch(CurOper)
	{
	case OPER_PER_SELECTSINGLESCOREBYONESUBJECT: //单科单条查询  操作前请求处理
	case OPER_PER_SELECTSINGLESCOREBYSUBJECTS: //现有所有科目单条更改成绩  操作前请求处理
	case OPER_PER_SELECTBATCHSCOREBYONESUBJECT: //单科批量查询  操作前请求处理
	case OPER_PER_SELECTBATCHSCOREBYSUBJECTS: //现有所有科目批量更改成绩  操作前请求处理
		GetSubjectsAfterSelectScoreChooseHandle();
		break;
	default:
		printf("%s 没有该操作！\n", __FUNCTION__);
		ProcMgr::GetInstance()->ProcSwitch(GetMyProcDef(), true);  
		break;
	}
}


void SelectScoreSysProc::GetSubjectsAfterSelectScoreChooseHandle()
{
	//获取现有科目
	CS_MSG_GET_SUBJECTS_REQ SendReq;
	SendReq.sGetType = 2;

	TCPHandle::GetInstance()->Send(&SendReq, sizeof(SendReq), /*MAIN_ID_LOGINREGISTER,*/ ASSIST_ID_GET_SUBJECTS_REQ);
}

void SelectScoreSysProc::SelectSingleScoreByOneSubjectChooseHandle(char* pStrExistSubjects)
{
	if (NULL == pStrExistSubjects)
	{
		printf("%s 科目字段为NULL\n", __FUNCTION__);
		SetIEndFlag(-1);
		return;
	}

	vector<string> vecStrSubjectId = StringTool::Splite(pStrExistSubjects, "|");
	map<int, string> mStrChineseNameShow; //显示的科目
	for (unsigned i=0; i<vecStrSubjectId.size(); i++)
	{
		int sId = (int)atoi(vecStrSubjectId.at(i).c_str());
		if (UserInfoMgr::GetInstance()->CanFindSubjectsType((SubjectsType)sId))
		{
			mStrChineseNameShow.insert(pair<int, string>(sId, UserInfoMgr::GetInstance()->GetChineseNameByType((SubjectsType)sId)));
		}
	}

	if (mStrChineseNameShow.empty())
	{
		printf("%s 没有科目，请先添加科目\n", __FUNCTION__);
		SetIEndFlag(1);
		return;
	}
	if (mStrChineseNameShow.size() != vecStrSubjectId.size())
	{
		printf("%s 处理过后的科目数量与原科目数量不同\n", __FUNCTION__);
		SetIEndFlag(-1);
		return;
	}

	string strAccount;
	if (UserInfoMgr::GetInstance()->GetUserIdent() == 1)
	{
		strAccount = UserInfoMgr::GetInstance()->GetUserAccount();
	}
	else
	{
		cout<<"请输入被查询成绩对象的用户名："<<endl;
		cin>>strAccount;
		if (!(CheckTool::CheckStringLen(strAccount, 30) && CheckTool::CheckStringByValid(strAccount, "A~Z|a~z|0~9")))
		{
			OperInputErrorHandle(false);
			return;
		}
	}

	printf("请选择要需要查询分数的科目ID：\n");
	ShowSubjects(mStrChineseNameShow);
	string iChooseTmp;
	cin>>iChooseTmp;
	if (!CheckTool::CheckStringByValid(iChooseTmp, "0~9") || mStrChineseNameShow.find((int)atoi(iChooseTmp.c_str())) == mStrChineseNameShow.end())
	{
		OperInputErrorHandle(false);
		return;
	}

	CS_MSG_SELECT_SINGLE_SCORE_REQ SendReq;
	SendReq.sType = 1;
	SendReq.bSubjectCount = 1;
	SendReq.sSubjectId[0] = (unsigned char)atoi(iChooseTmp.c_str());
	strcpy_s(SendReq.cAccount, sizeof(SendReq.cAccount), strAccount.c_str());

	TCPHandle::GetInstance()->Send(&SendReq, sizeof(SendReq), /*MAIN_ID_LOGINREGISTER,*/ ASSIST_ID_SELECT_SINGLE_SCORE_REQ);
}

void SelectScoreSysProc::SelectSingleScoreBySubjectsChooseHandle(char* pStrExistSubjects)
{
	if (NULL == pStrExistSubjects)
	{
		printf("%s 科目字段为NULL\n", __FUNCTION__);
		SetIEndFlag(-1);
		return;
	}

	CS_MSG_SELECT_SINGLE_SCORE_REQ SendReq;
	SendReq.sType = 2;

	vector<string> vecStrSubjectId = StringTool::Splite(pStrExistSubjects, "|");
	vector<unsigned char> vecUcSubjectId; //有效的科目id
	for (unsigned i=0; i<vecStrSubjectId.size(); i++)
	{
		unsigned char sId = (unsigned char)atoi(vecStrSubjectId.at(i).c_str());
		if (UserInfoMgr::GetInstance()->CanFindSubjectsType((SubjectsType)sId))
		{
			vecUcSubjectId.push_back(sId);
		}
	}

	if (vecUcSubjectId.empty())
	{
		printf("%s 没有科目，请先添加科目\n", __FUNCTION__);
		SetIEndFlag(1);
		return;
	}
	if (vecUcSubjectId.size() != vecStrSubjectId.size())
	{
		printf("%s 处理过后的科目数量与原科目数量不同\n", __FUNCTION__);
		SetIEndFlag(-1);
		return;
	}

	string strAccount;
	if (UserInfoMgr::GetInstance()->GetUserIdent() == 1)
	{
		strAccount = UserInfoMgr::GetInstance()->GetUserAccount();
	}
	else
	{
		cout<<"请输入被查询成绩对象的用户名："<<endl;
		cin>>strAccount;
		if (!(CheckTool::CheckStringLen(strAccount, 30) && CheckTool::CheckStringByValid(strAccount, "A~Z|a~z|0~9")))
		{
			OperInputErrorHandle(false);
			return;
		}
	}
	strcpy_s(SendReq.cAccount, sizeof(SendReq.cAccount), strAccount.c_str());

	SendReq.bSubjectCount = 0;
	for (;SendReq.bSubjectCount<vecUcSubjectId.size(); SendReq.bSubjectCount++)
	{
		SendReq.sSubjectId[SendReq.bSubjectCount] = vecUcSubjectId.at(SendReq.bSubjectCount);
	}

	TCPHandle::GetInstance()->Send(&SendReq, sizeof(SendReq), /*MAIN_ID_LOGINREGISTER,*/ ASSIST_ID_SELECT_SINGLE_SCORE_REQ);
}

void SelectScoreSysProc::SelectBatchScoreByOneSubjectChooseHandle(char* pStrExistSubjects)
{
	if (NULL == pStrExistSubjects)
	{
		printf("%s 科目字段为NULL\n", __FUNCTION__);
		SetIEndFlag(-1);
		return;
	}

	CS_MSG_SELECT_BATCH_SCORE_REQ SendReq;
	SendReq.sType = 1;

	vector<string> vecStrSubjectId = StringTool::Splite(pStrExistSubjects, "|");
	map<int, string> mStrChineseNameShow; //显示的科目
	for (unsigned i=0; i<vecStrSubjectId.size(); i++)
	{
		int sId = (int)atoi(vecStrSubjectId.at(i).c_str());
		if (UserInfoMgr::GetInstance()->CanFindSubjectsType((SubjectsType)sId))
		{
			mStrChineseNameShow.insert(pair<int, string>(sId, UserInfoMgr::GetInstance()->GetChineseNameByType((SubjectsType)sId)));
		}
	}

	if (mStrChineseNameShow.empty())
	{
		printf("%s 没有科目，请先添加科目\n", __FUNCTION__);
		SetIEndFlag(1);
		return;
	}
	if (mStrChineseNameShow.size() != vecStrSubjectId.size())
	{
		printf("%s 处理过后的科目数量与原科目数量不同\n", __FUNCTION__);
		SetIEndFlag(-1);
		return;
	}

	printf("请选择要需要查询分数的科目ID：\n");
	ShowSubjects(mStrChineseNameShow);
	string iChooseTmp;
	cin>>iChooseTmp;
	if (!CheckTool::CheckStringByValid(iChooseTmp, "0~9") || mStrChineseNameShow.find((int)atoi(iChooseTmp.c_str())) == mStrChineseNameShow.end())
	{
		OperInputErrorHandle(false);
		return;
	}
	SendReq.bSubjectCount = 1;
	SendReq.sSubjectId[0] = (unsigned char)atoi(iChooseTmp.c_str());

	cout<<"是否根据班级查询（1--是  2--否）："<<endl;
	string strGradeChoose;
	string strGrade;
	cin>>strGradeChoose;
	if (!(CheckTool::CheckStringLen(strGradeChoose, 1) && CheckTool::CheckStringByValid(strGradeChoose, "1|2")))
	{
		OperInputErrorHandle(false);
		return;
	}
	else
	{
		if (strGradeChoose == "1") //以后优化成选项，也就是需要从服务端先获取现有班级
		{
			cout<<"请您输入需要查询的班级名称（如 机电11-1）："<<endl;
			cin>>strGrade;
		}
	}
	if (strGrade.empty())
	{
		memset(SendReq.cGrade, 0, sizeof(SendReq.cGrade));
	}
	else
	{
		strcpy_s(SendReq.cGrade, sizeof(SendReq.cGrade), strGrade.c_str());
	}

	cout<<"是否需要条件查询查询（1--是  2--否）："<<endl;
	string strConditonChoose;
	string strConditon;
	cin>>strConditonChoose;
	if (!(CheckTool::CheckStringLen(strConditonChoose, 1) && CheckTool::CheckStringByValid(strConditonChoose, "1|2")))
	{
		OperInputErrorHandle(false);
		return;
	}
	else
	{
		if (strConditonChoose == "1") 
		{
			printf("请选择查询条件（输入格式为 X|X|X）[选项：%d-%s  %d-%s  %d-%s  %d-%s]：\n", SELECT_SCORE_CONDITION_RANK, "排序", SELECT_SCORE_CONDITION_RANGE, "分数范围", SELECT_SCORE_CONDITION_TOTAL, "总分", SELECT_SCORE_CONDITION_AVERAGE, "平均分");
			cin>>strConditon;
			if (!(CheckTool::CheckStringLen(strConditon, SELECT_SCORE_CONDITION_END*2-3) && CheckStringVaildRemoveSpl(strConditon, "|", "1|2|3|4"))) 
			{
				OperInputErrorHandle(false);
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
		if (vecStrConditon.size() >= SELECT_SCORE_CONDITION_END)
		{
			printf("%s 查询条件错误\n", __FUNCTION__);
			OperInputErrorHandle(false);
			return;
		}
		for (; cConditionCount<vecStrConditon.size() && cConditionCount<SELECT_SCORE_CONDITION_END-1; cConditionCount++)
		{
			SendReq.cCondition[cConditionCount] = (unsigned char)atoi(vecStrConditon.at(cConditionCount).c_str());
		}
	}

	SendReq.bRankFlag = 0;
	if (FindOneInArray(SendReq.cCondition, cConditionCount, SELECT_SCORE_CONDITION_END-1, SELECT_SCORE_CONDITION_RANK))
	{
		printf("请输入查询排序方式（%d-%s  %d-%s）：\n",SELECT_SCORE_RANK_ASC, "升序", SELECT_SCORE_RANK_DESC, "降序");
		string strRankFlag;
		cin>>strRankFlag;
		if (!(CheckTool::CheckStringLen(strRankFlag, 1) && CheckTool::CheckStringByValid(strRankFlag, "1|2")))
		{
			OperInputErrorHandle(false);
			return;
		}
		SendReq.bRankFlag = (unsigned char)atoi(strRankFlag.c_str());
	}

	SendReq.bScoreRange[2] = 0;
	if (FindOneInArray(SendReq.cCondition, cConditionCount, SELECT_SCORE_CONDITION_END-1, SELECT_SCORE_CONDITION_RANGE))
	{
		printf("请输入需要查询的分数范围（格式 XX-XX）：\n");
		string strScoreRange;
		cin>>strScoreRange;
		vector<string> vecScoreRange = StringTool::Splite(strScoreRange, "-");
		if (!(vecScoreRange.size()==2 &&  CheckTool::CheckStringLen(vecScoreRange.at(0), 3) &&  CheckTool::CheckStringLen(vecScoreRange.at(1), 3) && \
			CheckTool::CheckStringByValid(vecScoreRange.at(0), "0~9") && CheckTool::CheckStringByValid(vecScoreRange.at(1), "0~9") && \
			(unsigned char)atoi(vecScoreRange.at(0).c_str())>=0 && (unsigned char)atoi(vecScoreRange.at(0).c_str())<=100 && \
			(unsigned char)atoi(vecScoreRange.at(1).c_str())>=0 && (unsigned char)atoi(vecScoreRange.at(1).c_str())<=100 && \
			(unsigned char)atoi(vecScoreRange.at(0).c_str())<=(unsigned char)atoi(vecScoreRange.at(1).c_str())))
		{
			OperInputErrorHandle(false);
			return;
		}
		SendReq.bScoreRange[0] = (unsigned char)atoi(vecScoreRange.at(0).c_str());
		SendReq.bScoreRange[1] = (unsigned char)atoi(vecScoreRange.at(1).c_str());
		SendReq.bScoreRange[2] = 1;
	}

	TCPHandle::GetInstance()->Send(&SendReq, sizeof(SendReq), /*MAIN_ID_LOGINREGISTER,*/ ASSIST_ID_SELECT_BATCH_SCORE_REQ);
}

void SelectScoreSysProc::SelectBatchScoreBySubjectsChooseHandle(char* pStrExistSubjects)
{
	if (NULL == pStrExistSubjects)
	{
		printf("%s 科目字段为NULL\n", __FUNCTION__);
		SetIEndFlag(-1);
		return;
	}
	
	CS_MSG_SELECT_BATCH_SCORE_REQ SendReq;
	SendReq.sType = 2;
	
	vector<string> vecStrSubjectId = StringTool::Splite(pStrExistSubjects, "|");
	vector<unsigned char> vecUcSubjectId; //有效的科目id
	for (unsigned i=0; i<vecStrSubjectId.size(); i++)
	{
		unsigned char sId = (unsigned char)atoi(vecStrSubjectId.at(i).c_str());
		if (UserInfoMgr::GetInstance()->CanFindSubjectsType((SubjectsType)sId))
		{
			vecUcSubjectId.push_back(sId);
		}
	}

	if (vecUcSubjectId.empty())
	{
		printf("%s 没有科目，请先添加科目\n", __FUNCTION__);
		SetIEndFlag(1);
		return;
	}
	if (vecUcSubjectId.size() != vecStrSubjectId.size())
	{
		printf("%s 处理过后的科目数量与原科目数量不同\n", __FUNCTION__);
		SetIEndFlag(-1);
		return;
	}

	SendReq.bSubjectCount = 0;
	for (;SendReq.bSubjectCount<vecUcSubjectId.size(); SendReq.bSubjectCount++)
	{
		SendReq.sSubjectId[SendReq.bSubjectCount] = vecUcSubjectId.at(SendReq.bSubjectCount);
	}

	cout<<"是否根据班级查询（1--是  2--否）："<<endl;
	string strGradeChoose;
	string strGrade;
	cin>>strGradeChoose;
	if (!(CheckTool::CheckStringLen(strGradeChoose, 1) && CheckTool::CheckStringByValid(strGradeChoose, "1|2")))
	{
		OperInputErrorHandle(false);
		return;
	}
	else
	{
		if (strGradeChoose == "1") //以后优化成选项，也就是需要从服务端先获取现有班级
		{
			cout<<"请您输入需要查询的班级名称（如 机电11-1）："<<endl;
			cin>>strGrade;
		}
	}
	if (strGrade.empty())
	{
		memset(SendReq.cGrade, 0, sizeof(SendReq.cGrade));
	}
	else
	{
		strcpy_s(SendReq.cGrade, sizeof(SendReq.cGrade), strGrade.c_str());
	}

	cout<<"是否需要条件查询查询（1--是  2--否）："<<endl;
	string strConditonChoose;
	string strConditon;
	cin>>strConditonChoose;
	if (!(CheckTool::CheckStringLen(strConditonChoose, 1) && CheckTool::CheckStringByValid(strConditonChoose, "1|2")))
	{
		OperInputErrorHandle(false);
		return;
	}
	else
	{
		if (strConditonChoose == "1") 
		{
			printf("请选择查询条件（输入格式为 X|X|X）[选项：%d-%s  %d-%s  %d-%s  %d-%s]：\n", SELECT_SCORE_CONDITION_RANK, "排序", SELECT_SCORE_CONDITION_RANGE, "分数范围", SELECT_SCORE_CONDITION_TOTAL, "总分", SELECT_SCORE_CONDITION_AVERAGE, "平均分");
			cin>>strConditon;
			if (!(CheckTool::CheckStringLen(strConditon, SELECT_SCORE_CONDITION_END*2-3) && CheckStringVaildRemoveSpl(strConditon, "|", "1|2|3|4"))) 
			{
				OperInputErrorHandle(false);
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
		if (vecStrConditon.size() >= SELECT_SCORE_CONDITION_END)
		{
			printf("%s 查询条件错误\n", __FUNCTION__);
			OperInputErrorHandle(false);
			return;
		}
		for (; cConditionCount<vecStrConditon.size() && cConditionCount<SELECT_SCORE_CONDITION_END-1; cConditionCount++)
		{
			SendReq.cCondition[cConditionCount] = (unsigned char)atoi(vecStrConditon.at(cConditionCount).c_str());
		}
	}

	SendReq.bRankFlag = 0;
	if (FindOneInArray(SendReq.cCondition, cConditionCount, SELECT_SCORE_CONDITION_END-1, SELECT_SCORE_CONDITION_RANK))
	{
		printf("请输入查询排序方式（%d-%s  %d-%s）：\n",SELECT_SCORE_RANK_ASC, "升序", SELECT_SCORE_RANK_DESC, "降序");
		string strRankFlag;
		cin>>strRankFlag;
		if (!(CheckTool::CheckStringLen(strRankFlag, 1) && CheckTool::CheckStringByValid(strRankFlag, "1|2")))
		{
			OperInputErrorHandle(false);
			return;
		}
		SendReq.bRankFlag = (unsigned char)atoi(strRankFlag.c_str());
	}

	SendReq.bScoreRange[2] = 0;
	if (FindOneInArray(SendReq.cCondition, cConditionCount, SELECT_SCORE_CONDITION_END-1, SELECT_SCORE_CONDITION_RANGE))
	{
		printf("请输入需要查询的分数范围（格式 XX-XX）：\n");
		string strScoreRange;
		cin>>strScoreRange;
		vector<string> vecScoreRange = StringTool::Splite(strScoreRange, "-");
		if (!(vecScoreRange.size()==2 &&  CheckTool::CheckStringLen(vecScoreRange.at(0), 3) &&  CheckTool::CheckStringLen(vecScoreRange.at(1), 3) && \
			CheckTool::CheckStringByValid(vecScoreRange.at(0), "0~9") && CheckTool::CheckStringByValid(vecScoreRange.at(1), "0~9") && \
			(unsigned char)atoi(vecScoreRange.at(0).c_str())>=0 && (unsigned char)atoi(vecScoreRange.at(0).c_str())<=100 && \
			(unsigned char)atoi(vecScoreRange.at(1).c_str())>=0 && (unsigned char)atoi(vecScoreRange.at(1).c_str())<=100 && \
			(unsigned char)atoi(vecScoreRange.at(0).c_str())<=(unsigned char)atoi(vecScoreRange.at(1).c_str())))
		{
			OperInputErrorHandle(false);
			return;
		}
		SendReq.bScoreRange[0] = (unsigned char)atoi(vecScoreRange.at(0).c_str());
		SendReq.bScoreRange[1] = (unsigned char)atoi(vecScoreRange.at(1).c_str());
		SendReq.bScoreRange[2] = 1;
	}

	TCPHandle::GetInstance()->Send(&SendReq, sizeof(SendReq), /*MAIN_ID_LOGINREGISTER,*/ ASSIST_ID_SELECT_BATCH_SCORE_REQ);
}

bool SelectScoreSysProc::GetSubjectsAfterSelectScoreRecvHandle(void* vpData, unsigned int DataLen)
{

	if (OPER_PER_SELECTSINGLESCOREBYONESUBJECT != GetCurOper() && OPER_PER_SELECTSINGLESCOREBYSUBJECTS != GetCurOper() && OPER_PER_SELECTBATCHSCOREBYONESUBJECT != GetCurOper() && OPER_PER_SELECTBATCHSCOREBYSUBJECTS != GetCurOper())
	{
		printf("不是进行该操作[%d | %d | %d | %d]，当前进行的操作是[%d] error\n", OPER_PER_SELECTSINGLESCOREBYONESUBJECT, OPER_PER_SELECTSINGLESCOREBYSUBJECTS, OPER_PER_SELECTBATCHSCOREBYONESUBJECT, OPER_PER_SELECTBATCHSCOREBYSUBJECTS,GetCurOper());
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
		if (OPER_PER_SELECTSINGLESCOREBYONESUBJECT == GetCurOper())
		{
			SelectSingleScoreByOneSubjectChooseHandle(RecvMSG->cCanAlterSubjects);
		}
		else if(OPER_PER_SELECTSINGLESCOREBYSUBJECTS == GetCurOper())
		{
			SelectSingleScoreBySubjectsChooseHandle(RecvMSG->cCanAlterSubjects);
		}
		else if(OPER_PER_SELECTBATCHSCOREBYONESUBJECT == GetCurOper())
		{
			SelectBatchScoreByOneSubjectChooseHandle(RecvMSG->cCanAlterSubjects);
		}
		else if(OPER_PER_SELECTBATCHSCOREBYSUBJECTS == GetCurOper())
		{
			SelectBatchScoreBySubjectsChooseHandle(RecvMSG->cCanAlterSubjects);
		}
		else
		{
			//printf("%s  RecvMSG->sGetType=[%d] error\n", __FUNCTION__, (int)RecvMSG->sGetType);
			SetIEndFlag(-1);
			return false;
		}

	}
	else
	{
		printf("可查询科目返回不成功");
		SetIEndFlag(-1);
		return false;
	}
	return true;
}

bool SelectScoreSysProc::SelectSingleScoreRecvHandle(void* vpData, unsigned int DataLen)
{
	if (OPER_PER_SELECTSINGLESCOREBYONESUBJECT != GetCurOper() && OPER_PER_SELECTSINGLESCOREBYSUBJECTS != GetCurOper() )
	{
		printf("不是进行该操作[%d | %d]，当前进行的操作是[%d] error\n", OPER_PER_SELECTSINGLESCOREBYONESUBJECT, OPER_PER_SELECTSINGLESCOREBYSUBJECTS,GetCurOper());
		SetIEndFlag(-1);
		return false;
	}
	if (DataLen != sizeof(SC_MSG_SELECT_SINGLE_SCORE_ACK))
	{
		printf("DataLen[%u] error, It should be [%u]\n", DataLen, sizeof(SC_MSG_SELECT_SINGLE_SCORE_ACK));
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

	vector<string> vSelectFieldShow;
	vector<string> vSelectResultShow;
	vSelectFieldShow.push_back("学号");
	vSelectResultShow.push_back(StringTool::NumberToStr((int)RecvMSG->uUserid));
	vSelectFieldShow.push_back("姓名");
	vSelectResultShow.push_back(RecvMSG->cName);
	vSelectFieldShow.push_back("账号");
	vSelectResultShow.push_back(RecvMSG->cAccount);
	vSelectFieldShow.push_back("班级");
	vSelectResultShow.push_back(RecvMSG->cGrade);

	unsigned char cInvalidCount = 0;
	for (unsigned char i=0; i<RecvMSG->bSubjectCount; i++)
	{
		SubjectsType sSubjectId = (SubjectsType)RecvMSG->bSubjectId[i];
		string strChineseName;
		string strScore = StringTool::NumberToStr((int)RecvMSG->bScore[i]);
		if (UserInfoMgr::GetInstance()->CanFindSubjectsType(sSubjectId))
		{
			strChineseName = UserInfoMgr::GetInstance()->GetChineseNameByType(sSubjectId);

			vSelectFieldShow.push_back(strChineseName);
			vSelectResultShow.push_back(strScore);
			cInvalidCount++;
		}
	}

	if (cInvalidCount != RecvMSG->bSubjectCount)
	{
		printf("%s 科目数量和处理后科目数量不一致\n", __FUNCTION__);
		SetIEndFlag(-1);
		return false;
	}
	//显示查询成绩
	ShowSelectSingleScoreResult(vSelectFieldShow, vSelectResultShow, true);
	SetIEndFlag(1);

	return true;
}

bool SelectScoreSysProc::SelectBatchScoreRecvHandle(void* vpData, unsigned int DataLen)
{
	if (OPER_PER_SELECTBATCHSCOREBYONESUBJECT != GetCurOper() && OPER_PER_SELECTBATCHSCOREBYSUBJECTS != GetCurOper() )
	{
		printf("不是进行该操作[%d | %d]，当前进行的操作是[%d] error\n", OPER_PER_SELECTBATCHSCOREBYONESUBJECT, OPER_PER_SELECTBATCHSCOREBYSUBJECTS,GetCurOper());
		SetIEndFlag(-1);
		return false;
	}
	if (DataLen != sizeof(SC_MSG_SELECT_BATCH_SCORE_ACK))
	{
		printf("DataLen[%u] error, It should be [%u]\n", DataLen, sizeof(SC_MSG_SELECT_BATCH_SCORE_ACK));
		SetIEndFlag(-1);
		return false;
	}

	SC_MSG_SELECT_BATCH_SCORE_ACK* RecvMSG = (SC_MSG_SELECT_BATCH_SCORE_ACK*) vpData;
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

		if (RecvMSG->bRankFlag == 2) //降序
		{
			vSelectFieldShow.push_back("排名");
			vSelectResultShow.push_back(StringTool::NumberToStr((int)RecvMSG->sRank[i]));
		}

		vSelectFieldShow.push_back("学号");
		vSelectResultShow.push_back(StringTool::NumberToStr((int)RecvMSG->uUserid[i]));
		
		vSelectFieldShow.push_back("姓名");
		vSelectResultShow.push_back(RecvMSG->cName[i]);

		vSelectFieldShow.push_back("账号");
		vSelectResultShow.push_back(RecvMSG->cAccount[i]);

		vSelectFieldShow.push_back("班级");
		vSelectResultShow.push_back(RecvMSG->cGrade[i]);

		unsigned char cInvalidCount = 0;
		for (unsigned char j=0; j<RecvMSG->bSubjectCount; j++)
		{
			SubjectsType sSubjectId = (SubjectsType)RecvMSG->bSubjectId[i][j];
			string strChineseName;
			string strScore = StringTool::NumberToStr((int)RecvMSG->bScore[i][j]);
			if (UserInfoMgr::GetInstance()->CanFindSubjectsType(sSubjectId))
			{
				strChineseName = UserInfoMgr::GetInstance()->GetChineseNameByType(sSubjectId);

				vSelectFieldShow.push_back(strChineseName);
				vSelectResultShow.push_back(strScore);
				cInvalidCount++;
			}
		}
		if (cInvalidCount != RecvMSG->bSubjectCount)
		{
			printf("%s 科目数量和处理后科目数量不一致\n", __FUNCTION__);
			SetIEndFlag(-1);
			return false;
		}
		
		if (FindOneInArray(RecvMSG->cCondition, SELECT_SCORE_CONDITION_END-1, SELECT_SCORE_CONDITION_END-1, SELECT_SCORE_CONDITION_TOTAL))
		{
			vSelectFieldShow.push_back("总分");
			vSelectResultShow.push_back(StringTool::NumberToStr((int)RecvMSG->bSum[i]));
		}
		
		if (FindOneInArray(RecvMSG->cCondition, SELECT_SCORE_CONDITION_END-1, SELECT_SCORE_CONDITION_END-1, SELECT_SCORE_CONDITION_AVERAGE))
		{
			vSelectFieldShow.push_back("平均分");
			vSelectResultShow.push_back(StringTool::NumberToStr((int)RecvMSG->bAverage[i]));
		}

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

int SelectScoreSysProc::ShowSubjects(map<int, string> mIStr, int iField)
{
	if (iField == 0)
		iField = 0xEFFFFFFF;
	else if (iField < 0)
		iField *= -1;

	int iCount = 0;
	for (map<int, string>::iterator iter = mIStr.begin(); iter != mIStr.end(); iter++)
	{
		if (iCount != 0 && iCount % iField == 0)
		{
			cout<<endl;
		}
		if (iCount != 0 && iCount % iField != 0)
		{
			cout<<"\t\t";
		}
		iCount++;
		printf("%d--%s", iter->first, iter->second.c_str());
	}
	if (iCount > 0)
	{
		cout<<endl;
	}

	return iCount;
}

void SelectScoreSysProc::ShowSelectSingleScoreResult(vector<string> vSelectFieldShow, vector<string> vSelectResultShow, bool bShow)
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

bool SelectScoreSysProc::FindOneInArray(unsigned char* pArray, unsigned char iCount, unsigned char iMaxCount, unsigned char Obj)
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

bool SelectScoreSysProc::CheckStringVaildRemoveSpl(string str, string strSpl, string strVaild)
{
	vector<string> vecStr = StringTool::Splite(str, strSpl);
	string strTmp;
	for (unsigned i=0; i<vecStr.size(); i++)
	{
		strTmp += vecStr.at(i);
	}
	
	return CheckTool::CheckStringByValid(strTmp, strVaild);
}