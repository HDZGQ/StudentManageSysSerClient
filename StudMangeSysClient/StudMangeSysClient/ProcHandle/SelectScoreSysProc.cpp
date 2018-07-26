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
		printf("没有科目，请先添加科目\n");
		SetIEndFlag(1);
		return;
	}

	cout<<"请输入被查询成绩对象的用户名："<<endl;
	string strAccount;
	cin>>strAccount;
	if (!(CheckTool::CheckStringLen(strAccount, 30) && CheckTool::CheckStringByValid(strAccount, "A~Z|a~z|0~9")))
	{
		OperInputErrorHandle(false);
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
		printf("没有科目，请先添加科目\n");
		SetIEndFlag(1);
		return;
	}

	cout<<"请输入被查询成绩对象的用户名："<<endl;
	string strAccount;
	cin>>strAccount;
	if (!(CheckTool::CheckStringLen(strAccount, 30) && CheckTool::CheckStringByValid(strAccount, "A~Z|a~z|0~9")))
	{
		OperInputErrorHandle(false);
		return;
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

}

void SelectScoreSysProc::SelectBatchScoreBySubjectsChooseHandle(char* pStrExistSubjects)
{

}

bool SelectScoreSysProc::GetSubjectsAfterSelectScoreRecvHandle(void* vpData, unsigned int DataLen)
{

	if (OPER_PER_SELECTSINGLESCOREBYONESUBJECT != GetCurOper() && OPER_PER_SELECTSINGLESCOREBYSUBJECTS != GetCurOper() && OPER_PER_SELECTBATCHSCOREBYONESUBJECT != GetCurOper() && OPER_PER_SELECTBATCHSCOREBYSUBJECTS != GetCurOper())
	{
		printf("不是进行该操作[%d | %d | %d | %d]，当前进行的操作是[%d] error\n", OPER_PER_SELECTSINGLESCOREBYONESUBJECT, OPER_PER_SELECTSINGLESCOREBYSUBJECTS, OPER_PER_SELECTBATCHSCOREBYONESUBJECT, OPER_PER_SELECTBATCHSCOREBYSUBJECTS,GetCurOper());
		SetIEndFlag(-1);
		return false;
	}
	if (DataLen != sizeof(CS_MSG_GET_SUBJECTS_ACK))
	{
		printf("DataLen[%u] error, It should be [%u]\n", DataLen, sizeof(CS_MSG_GET_SUBJECTS_ACK));
		SetIEndFlag(-1);
		return false;
	}

	CS_MSG_GET_SUBJECTS_ACK* RecvMSG = (CS_MSG_GET_SUBJECTS_ACK*) vpData;
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
	if (DataLen != sizeof(CS_MSG_SELECT_SINGLE_SCORE_ACK))
	{
		printf("DataLen[%u] error, It should be [%u]\n", DataLen, sizeof(CS_MSG_SELECT_SINGLE_SCORE_ACK));
		SetIEndFlag(-1);
		return false;
	}

	CS_MSG_SELECT_SINGLE_SCORE_ACK* RecvMSG = (CS_MSG_SELECT_SINGLE_SCORE_ACK*) vpData;

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
	ShowSelectSingleScoreResult(vSelectFieldShow, vSelectResultShow, true, true);
	SetIEndFlag(1);

	return true;
}

bool SelectScoreSysProc::SelectBatchScoreRecvHandle(void* vpData, unsigned int DataLen)
{

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

void SelectScoreSysProc::ShowSelectSingleScoreResult(vector<string> vSelectFieldShow, vector<string> vSelectResultShow, bool bShowDescribe, bool bShowLine)
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

	if (bShowLine)
		printf("-------------------------------\n");
	if (bShowDescribe)
		printf("查询结果显示：\n");

	cout<<strFieldShow<<endl;
	cout<<strResult<<endl;
}