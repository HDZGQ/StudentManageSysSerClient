#include "ScoreProc.h"
#include "SubjectsMgr.h"
#include "CheckTool.h"
#include "MsgPackageMgr.h"
#include "MysqlMgr.h"

ScoreProc::ScoreProc()
{

}

ScoreProc::~ScoreProc()
{

}

void ScoreProc::CheckCanAlterSubjectRecvHandle(SOCKET SocketId, void* vpData, unsigned int DataLen)
{
	if ( NULL == vpData)
	{
		printf("%s  消息为空\n", __FUNCTION__);
		return;
	}
	if (DataLen != sizeof(CS_MSG_GET_CAN_ALTER_AFTER_ALTER_SUBJECTS_REQ))
	{
		printf("%s  长度DataLen[%u]不对，正确长度[%u]\n", __FUNCTION__, DataLen, sizeof(CS_MSG_GET_CAN_ALTER_AFTER_ALTER_SUBJECTS_REQ));
		return;
	}

	CS_MSG_GET_CAN_ALTER_AFTER_ALTER_SUBJECTS_REQ* RecvMsg = (CS_MSG_GET_CAN_ALTER_AFTER_ALTER_SUBJECTS_REQ*)vpData;

	CS_MSG_GET_CAN_ALTER_AFTER_ALTER_SUBJECTS_ACK SendMsg;
	SendMsg.bSucceed = true;

	vector<SubjectsType> vsSubjectsTmp;
	if (RecvMsg->sGetType == 1)//增加科目前获取 
	{
		SendMsg.sGetType = 1;
		vsSubjectsTmp = SubjectsMgr::GetInstance()->GetVNotExistSubjects();
	}
	else if (RecvMsg->sGetType == 2)// 删除科目前获取
	{
		SendMsg.sGetType = 2;
		vsSubjectsTmp = SubjectsMgr::GetInstance()->GetVExistSubjects();
	}
	else
	{
		SendMsg.bSucceed = false;
		printf("%s  RecvMsg->sGetType=[%d] error\n", __FUNCTION__, (int)RecvMsg->sGetType);
	}
	
	vector<int> viSubjectsTmp;
	for (vector<SubjectsType>::iterator iter = vsSubjectsTmp.begin(); iter != vsSubjectsTmp.end(); iter++)
	{
		viSubjectsTmp.push_back((int)(*iter));
	}

	string strSubjectsTmp = CheckTool::CombVecToStr(viSubjectsTmp);
	strcpy_s(SendMsg.cCanAlterSubjects, sizeof(SendMsg.cCanAlterSubjects), strSubjectsTmp.c_str());

	PackData packData = MsgPackageMgr::Pack(&SendMsg, sizeof(SendMsg), ASSIST_ID_GET_CAN_ALTER_AFTER_ALTER_SUBJECTS_ACK);
	MsgPackageMgr::Send(SocketId, packData);
}

void ScoreProc::AlterSubjectRecvHandle(SOCKET SocketId, void* vpData, unsigned int DataLen)
{
	if ( NULL == vpData)
	{
		printf("%s  消息为空\n", __FUNCTION__);
		return;
	}
	if (DataLen != sizeof(CS_MSG_ALTER_SUBJECTS_REQ))
	{
		printf("%s  长度DataLen[%u]不对，正确长度[%u]\n", __FUNCTION__, DataLen, sizeof(CS_MSG_ALTER_SUBJECTS_REQ));
		return;
	}

	CS_MSG_ALTER_SUBJECTS_REQ* RecvMsg = (CS_MSG_ALTER_SUBJECTS_REQ*)vpData;
	
	string sEnglishName = SubjectsMgr::GetInstance()->GetEnglishNameByType((SubjectsType)RecvMsg->sSubjectId);

	string strSqlTmp = "";
	if (RecvMsg->sGetType == 1)
	{
		strSqlTmp += " add " + sEnglishName + " tinyint"; 
	}
	else if (RecvMsg->sGetType == 2)
	{
		strSqlTmp += " drop " + sEnglishName;
	}
	else
	{
		CS_MSG_ALTER_SUBJECTS_ACK SendMsg;
		SendMsg.bSucceed = false;
		PackData packData = MsgPackageMgr::Pack(&SendMsg, sizeof(SendMsg), ASSIST_ID_ALTER_SUBJECTS_ACK);
		MsgPackageMgr::Send(SocketId, packData);
		return;
	}

	char strMysql[512];
	memset(strMysql, 0, sizeof(strMysql));
	sprintf_s(strMysql, sizeof(strMysql), "alter table studScore %s", strSqlTmp.c_str());

	string  strRecord = CheckTool::NumberToStr((int)RecvMsg->sGetType) + "|" + CheckTool::NumberToStr((int)RecvMsg->sSubjectId);

	MysqlMgr::GetInstance()->InputMsgQueue(strMysql, MYSQL_OPER_ALTER, ASSIST_ID_ALTER_SUBJECTS_ACK, SocketId, strRecord);
}


void ScoreProc::AlterSubjectReplyHandle(SOCKET SocketId, MYSQL_RES *MysqlRes, string strRecord)
{
	vector<string> vStrRecord= CheckTool::Splite(strRecord);

	CS_MSG_ALTER_SUBJECTS_ACK SendMsg;
	SendMsg.bSucceed = true;
	if (vStrRecord.size() > 2 && (int)atoi(vStrRecord.at(0).c_str()) == 0)
	{
		SendMsg.sGetType = (short)atoi(vStrRecord.at(1).c_str());
		SendMsg.sSubjectId = (short)atoi(vStrRecord.at(2).c_str());
		
		if (SendMsg.sGetType == 1)
		{
			SubjectsMgr::GetInstance()->AddOneExistSubject((SubjectsType)SendMsg.sSubjectId);
		}
		else if (SendMsg.sGetType == 2)
		{
			SubjectsMgr::GetInstance()->DeleteOneExistSubject((SubjectsType)SendMsg.sSubjectId);
		}
		else
		{
			printf("%s  SendMsg.sGetType[%d] error\n", __FUNCTION__, SendMsg.sGetType);
			SendMsg.bSucceed = false;
		}
	}
	else 
	{
		printf("%s  数据库语句执行失败\n", __FUNCTION__);
		SendMsg.bSucceed = false;
	}

	PackData packData = MsgPackageMgr::Pack(&SendMsg, sizeof(SendMsg), ASSIST_ID_ALTER_SUBJECTS_ACK);
	MsgPackageMgr::Send(SocketId, packData);
}
