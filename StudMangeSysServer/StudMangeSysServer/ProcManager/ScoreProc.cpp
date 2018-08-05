#include "ScoreProc.h"
#include "SubjectsMgr.h"
#include "MsgPackageMgr.h"
#include "MysqlMgr.h"
#include "UserInfoMgr.h"
#include "StringTool.h"

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
	if (DataLen != sizeof(CS_MSG_GET_SUBJECTS_REQ))
	{
		printf("%s  长度DataLen[%u]不对，正确长度[%u]\n", __FUNCTION__, DataLen, sizeof(CS_MSG_GET_SUBJECTS_REQ));
		return;
	}

	CS_MSG_GET_SUBJECTS_REQ* RecvMsg = (CS_MSG_GET_SUBJECTS_REQ*)vpData;

	SC_MSG_GET_SUBJECTS_ACK SendMsg;
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

	string strSubjectsTmp = StringTool::CombVecToStr(viSubjectsTmp);
	strcpy_s(SendMsg.cCanAlterSubjects, sizeof(SendMsg.cCanAlterSubjects), strSubjectsTmp.c_str());

	PackData packData = MsgPackageMgr::Pack(&SendMsg, sizeof(SendMsg), ASSIST_ID_GET_SUBJECTS_ACK);
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
		strSqlTmp += " add " + sEnglishName + " tinyint default 0"; 
	}
	else if (RecvMsg->sGetType == 2)
	{
		strSqlTmp += " drop " + sEnglishName;
	}
	else
	{
		SC_MSG_ALTER_SUBJECTS_ACK SendMsg;
		SendMsg.bSucceed = false;
		PackData packData = MsgPackageMgr::Pack(&SendMsg, sizeof(SendMsg), ASSIST_ID_ALTER_SUBJECTS_ACK);
		MsgPackageMgr::Send(SocketId, packData);
		return;
	}

	char strMysql[512];
	memset(strMysql, 0, sizeof(strMysql));
	sprintf_s(strMysql, sizeof(strMysql), "alter table studScore %s", strSqlTmp.c_str());

	string  strRecord = StringTool::NumberToStr((int)RecvMsg->sGetType) + "|" + StringTool::NumberToStr((int)RecvMsg->sSubjectId);

	MysqlMgr::GetInstance()->InputMsgQueue(strMysql, MYSQL_OPER_ALTER, ASSIST_ID_ALTER_SUBJECTS_ACK, SocketId, strRecord);
}

void ScoreProc::AddSingleScoreRecvHandle(SOCKET SocketId, void* vpData, unsigned int DataLen)
{
	if ( NULL == vpData)
	{
		printf("%s  消息为空\n", __FUNCTION__);
		return;
	}
	if (DataLen != sizeof(CS_MSG_ADD_SINGLE_SCORE_REQ))
	{
		printf("%s  长度DataLen[%u]不对，正确长度[%u]\n", __FUNCTION__, DataLen, sizeof(CS_MSG_ADD_SINGLE_SCORE_REQ));
		return;
	}

	CS_MSG_ADD_SINGLE_SCORE_REQ* RecvMsg = (CS_MSG_ADD_SINGLE_SCORE_REQ*)vpData;
	
	vector<string> vStrScore = StringTool::Splite(RecvMsg->sScore, "|");

	string strEnglishName = SubjectsMgr::GetInstance()->GetStrEnglishNameByStrType(RecvMsg->sSubjectId, "|");
	vector<string> vStrSubjectEnglishName = StringTool::Splite(strEnglishName, "|");
	if (vStrScore.empty() || vStrSubjectEnglishName.empty() || vStrScore.size() != vStrSubjectEnglishName.size())
	{
		printf("%s  分数组数或科目数有误，或两者不相等\n", __FUNCTION__);

		SC_MSG_ADD_SINGLE_SCORE_ACK SendMsg;
		SendMsg.bSucceed = false;
		PackData packData = MsgPackageMgr::Pack(&SendMsg, sizeof(SendMsg), ASSIST_ID_ADD_SINGLE_SCORE_ACK);
		MsgPackageMgr::Send(SocketId, packData);
		return;
	}

	string strSubjectEnglishName = StringTool::CombVecToStr(vStrSubjectEnglishName, ",");
	string strScore = StringTool::CombVecToStr(vStrScore, ",");
	string strUpdateSet = StringTool::CombToSqlUpdateSetStr(strSubjectEnglishName, strScore, ",");

	string strAccount = RecvMsg->cAccount;
	string strName = strAccount; //名称后续自己修改
	string strPwd = "123456";
	string strSex = "0";
	string strIdent = "1";

	string strInsertInfo = "''" + strAccount + "'',''" + strName + "'',''" + strPwd + "'',''" + strSex + "'',''" + strIdent + "''";

	//根据身份使用默认权限
	vector<OperPermission> vecOper;
	UserInfoMgr::GetInstance()->GetDefaultAuthorityByIdent((IdentType)atoi(strIdent.c_str()), vecOper);
	string strAuthority = StringTool::CombVecToStr(vecOper);

	char strMysql[512];
	memset(strMysql, 0, sizeof(strMysql));
	sprintf_s(strMysql, sizeof(strMysql), "call AddSingleScore('%s', '%s', '%s', '%s', '%s', '%s')", strUpdateSet.c_str(), strInsertInfo.c_str(), strAccount.c_str(), strAuthority.c_str(), strSubjectEnglishName.c_str(), strScore.c_str());
	
	string strRecord = "~";
	strRecord += StringTool::NumberToStr(RecvMsg->sType) + "~" + RecvMsg->cAccount;

	MysqlMgr::GetInstance()->InputMsgQueue(strMysql, MYSQL_OPER_CALL_PROC, ASSIST_ID_ADD_SINGLE_SCORE_ACK, SocketId, strRecord);
}

void ScoreProc::AddBatchScoreRecvHandle(SOCKET SocketId, void* vpData, unsigned int DataLen)
{
	if ( NULL == vpData)
	{
		printf("%s  消息为空\n", __FUNCTION__);
		return;
	}
	if (DataLen != sizeof(CS_MSG_ADD_BATCH_SCORE_REQ))
	{
		printf("%s  长度DataLen[%u]不对，正确长度[%u]\n", __FUNCTION__, DataLen, sizeof(CS_MSG_ADD_BATCH_SCORE_REQ));
		return;
	}

	CS_MSG_ADD_BATCH_SCORE_REQ* RecvMsg = (CS_MSG_ADD_BATCH_SCORE_REQ*)vpData;
	if (RecvMsg->bRecordCount < 1 || RecvMsg->bRecordCount > MAXBATCHREQACKCOUNT)
	{
		printf("%s  客户端传过来的成绩记录数不正确RecvMsg->bRecordCount[%u]\n", __FUNCTION__, (unsigned)RecvMsg->bRecordCount);
		return;
	}
	if (RecvMsg->bSubjectCount < 1 || RecvMsg->bSubjectCount > MAXSUBJECTSCOUNT)
	{
		printf("%s  客户端传过来的科目数量不正确RecvMsg->bSubjectCount[%u]\n", __FUNCTION__, (unsigned)RecvMsg->bSubjectCount);
		return;
	}

	for (unsigned char i=0; i < RecvMsg->bRecordCount; i++)
	{

		string strUpdateSet;
		vector<string> vStrSubjectEnglishName;
		vector<string> vStrScore;
		for (unsigned char j=0; j < RecvMsg->bSubjectCount; j++)
		{
			if (SubjectsMgr::GetInstance()->IsInAllSubjects((SubjectsType)RecvMsg->bSubjectId[i][j]))
			{
				vStrSubjectEnglishName.push_back(SubjectsMgr::GetInstance()->GetEnglishNameByType((SubjectsType)RecvMsg->bSubjectId[i][j]));
			}
			else
			{
				printf("%s  客户端传过来的不存在的科目ID RecvMsg->bSubjectId[i][j]=%u\n", __FUNCTION__, (unsigned)RecvMsg->bSubjectId[i][j]);
				return;
			}
			vStrScore.push_back(StringTool::NumberToStr((int)RecvMsg->bScore[i][j]));
		}
		strUpdateSet = StringTool::CombToSqlUpdateSetStr(vStrSubjectEnglishName, vStrScore);


		string strAccount = RecvMsg->cAccount[i];
		string strName = strAccount; //名称后续自己修改
		string strPwd = "123456";
		string strSex = "0";
		string strIdent = "1";

		string strInsertInfo = "''" + strAccount + "'',''" + strName + "'',''" + strPwd + "'',''" + strSex + "'',''" + strIdent + "''";

		string strSubjectEnglishName = StringTool::CombVecToStr(vStrSubjectEnglishName, ",");
		string strScore = StringTool::CombVecToStr(vStrScore, ",");

		//根据身份使用默认权限
		vector<OperPermission> vecOper;
		UserInfoMgr::GetInstance()->GetDefaultAuthorityByIdent((IdentType)atoi(strIdent.c_str()), vecOper);
		string strAuthority = StringTool::CombVecToStr(vecOper);

		char strMysql[512];
		memset(strMysql, 0, sizeof(strMysql));
		sprintf_s(strMysql, sizeof(strMysql), "call AddSingleScore('%s', '%s', '%s', '%s', '%s', '%s')", strUpdateSet.c_str(), strInsertInfo.c_str(), strAccount.c_str(), strAuthority.c_str(), strSubjectEnglishName.c_str(), strScore.c_str());

		string strRecord = "~";
		strRecord += StringTool::NumberToStr(RecvMsg->sType) + "~" + StringTool::NumberToStr(RecvMsg->bRecordCount) + "~" + StringTool::NumberToStr((int)RecvMsg->bRecordNO) + "~" + StringTool::NumberToStr((int)RecvMsg->bEnd) + "~" + StringTool::NumberToStr((int)((i==RecvMsg->bRecordCount-1) ? 1 : 0));

		MysqlMgr::GetInstance()->InputMsgQueue(strMysql, MYSQL_OPER_CALL_PROC, ASSIST_ID_ADD_BATCH_SCORE_ACK, SocketId, strRecord);
	}
}

void ScoreProc::SelectSingleScoreRecvHandle(SOCKET SocketId, void* vpData, unsigned int DataLen)
{
	if ( NULL == vpData)
	{
		printf("%s  消息为空\n", __FUNCTION__);
		return;
	}
	if (DataLen != sizeof(CS_MSG_SELECT_SINGLE_SCORE_REQ))
	{
		printf("%s  长度DataLen[%u]不对，正确长度[%u]\n", __FUNCTION__, DataLen, sizeof(CS_MSG_SELECT_SINGLE_SCORE_REQ));
		return;
	}

	CS_MSG_SELECT_SINGLE_SCORE_REQ* RecvMsg = (CS_MSG_SELECT_SINGLE_SCORE_REQ*)vpData;
	if (RecvMsg->bSubjectCount == 0)
	{
		SC_MSG_SELECT_SINGLE_SCORE_ACK SendMsg;
		SendMsg.bResCode = 3;
		PackData packData = MsgPackageMgr::Pack(&SendMsg, sizeof(SendMsg), ASSIST_ID_SELECT_SINGLE_SCORE_ACK);
		MsgPackageMgr::Send(SocketId, packData);
		return;
	}

	string strSubjectField;
	for (unsigned char i=0; i<RecvMsg->bSubjectCount; i++)
	{
		if (SubjectsMgr::GetInstance()->IsInAllSubjects((SubjectsType)RecvMsg->sSubjectId[i]))
		{
			strSubjectField += ",";
			strSubjectField += "s." + SubjectsMgr::GetInstance()->GetEnglishNameByType((SubjectsType)RecvMsg->sSubjectId[i]);
		}
	}

	char strMysql[512];
	memset(strMysql, 0, sizeof(strMysql));
	sprintf_s(strMysql, sizeof(strMysql), "select u.userID,u.name,u.grade %s from userinfo u,studscore s where u.userID=s.userID and u.account='%s'", strSubjectField.c_str(), RecvMsg->cAccount);

	string strRecord = "~";
	strRecord += StringTool::NumberToStr(RecvMsg->sType) + "~" + RecvMsg->cAccount + "~" + StringTool::CombArrayToStr(RecvMsg->sSubjectId, (unsigned)RecvMsg->bSubjectCount)  + "~" + StringTool::NumberToStr((int)RecvMsg->bSubjectCount);

	MysqlMgr::GetInstance()->InputMsgQueue(strMysql, MYSQL_OPER_SELECT, ASSIST_ID_SELECT_SINGLE_SCORE_ACK, SocketId, strRecord);
}

void ScoreProc::SelectBatchScoreRecvHandle(SOCKET SocketId, void* vpData, unsigned int DataLen)
{
	if ( NULL == vpData)
	{
		printf("%s  消息为空\n", __FUNCTION__);
		return;
	}
	if (DataLen != sizeof(CS_MSG_SELECT_BATCH_SCORE_REQ))
	{
		printf("%s  长度DataLen[%u]不对，正确长度[%u]\n", __FUNCTION__, DataLen, sizeof(CS_MSG_SELECT_BATCH_SCORE_REQ));
		return;
	}

	CS_MSG_SELECT_BATCH_SCORE_REQ* RecvMsg = (CS_MSG_SELECT_BATCH_SCORE_REQ*)vpData;
	if (RecvMsg->bSubjectCount == 0 || (RecvMsg->bScoreRange[2] == 1 && !(RecvMsg->bScoreRange[0]<=RecvMsg->bScoreRange[1] && \
		RecvMsg->bScoreRange[0]>=0 && RecvMsg->bScoreRange[0]<=100 && RecvMsg->bScoreRange[1]>=0 && RecvMsg->bScoreRange[1]<=100)))
	{
		SC_MSG_SELECT_BATCH_SCORE_ACK SendMsg;
		SendMsg.bResCode = 3;
		PackData packData = MsgPackageMgr::Pack(&SendMsg, sizeof(SendMsg), ASSIST_ID_SELECT_BATCH_SCORE_ACK);
		MsgPackageMgr::Send(SocketId, packData);
		return;
	}

	string strSubjectField;
	string strSumOrAverage;
	string strWhere;

	for (unsigned char i=0; i<RecvMsg->bSubjectCount; i++)
	{
		if (SubjectsMgr::GetInstance()->IsInAllSubjects((SubjectsType)RecvMsg->sSubjectId[i]))
		{
			string strOneSubjectField = "s." + SubjectsMgr::GetInstance()->GetEnglishNameByType((SubjectsType)RecvMsg->sSubjectId[i]);
			strSubjectField += ",";
			strSubjectField += strOneSubjectField;

			if (!strSumOrAverage.empty())
			{
				strSumOrAverage += "+";
			}
			strSumOrAverage += strOneSubjectField;

			if (RecvMsg->bScoreRange[2] == 1)
			{
				strWhere += " and " + strOneSubjectField;
				strWhere += ">=" + StringTool::NumberToStr((int)RecvMsg->bScoreRange[0]);
				strWhere += " and " + strOneSubjectField;
				strWhere += "<=" + StringTool::NumberToStr((int)RecvMsg->bScoreRange[1]);
			}
		}
	}

	char ch[128];
	if (!string(RecvMsg->cGrade).empty())
	{
		memset(ch, 0, sizeof(ch));
		sprintf_s(ch, sizeof(ch), " and u.grade='%s'", RecvMsg->cGrade);
		strWhere += ch;
	}

	if (RecvMsg->bRankFlag == 1) //升序
	{
		strWhere += " order by SSum asc";
	}
	else if (RecvMsg->bRankFlag == 2) //降序
	{
		strWhere += " order by SSum desc";
	}
	else //默认根据学号排序
	{
		strWhere += " order by u.userid asc";
	}

	char strMysql[1024];
	memset(strMysql, 0, sizeof(strMysql));
	sprintf_s(strMysql, sizeof(strMysql), "select @curRank := @curRank + 1 AS rank,u.userid,u.account,u.name,u.grade %s,(%s) as SSum, floor((%s)/3) as average from userinfo u,studscore s,(SELECT @curRank := 0) q where u.userid=s.userid %s", strSubjectField.c_str(), strSumOrAverage.c_str(), strSumOrAverage.c_str(), strWhere.c_str());

	string strRecord = "~";
	strRecord += StringTool::NumberToStr(RecvMsg->sType) + "~" + StringTool::CombArrayToStr(RecvMsg->cCondition, 5)  + "~" + StringTool::NumberToStr((int)RecvMsg->bRankFlag);

	MysqlMgr::GetInstance()->InputMsgQueue(strMysql, MYSQL_OPER_SELECT, ASSIST_ID_SELECT_BATCH_SCORE_ACK, SocketId, strRecord);
}

void ScoreProc::UpdateSingleScoreRecvHandle(SOCKET SocketId, void* vpData, unsigned int DataLen)
{
	if ( NULL == vpData)
	{
		printf("%s  消息为空\n", __FUNCTION__);
		return;
	}
	if (DataLen != sizeof(CS_MSG_UPDATE_SINGLE_SCORE_REQ))
	{
		printf("%s  长度DataLen[%u]不对，正确长度[%u]\n", __FUNCTION__, DataLen, sizeof(CS_MSG_UPDATE_SINGLE_SCORE_REQ));
		return;
	}

	CS_MSG_UPDATE_SINGLE_SCORE_REQ* RecvMsg = (CS_MSG_UPDATE_SINGLE_SCORE_REQ*)vpData;
	if (RecvMsg->bSubjectCount == 0)
	{
		SC_MSG_UPDATE_SINGLE_SCORE_ACK SendMsg;
		SendMsg.bSucceed = false;
		PackData packData = MsgPackageMgr::Pack(&SendMsg, sizeof(SendMsg), ASSIST_ID_UPDATE_SINGLE_SCORE_ACK);
		MsgPackageMgr::Send(SocketId, packData);
		return;
	}

	string strUpdateSet;
	for (unsigned char i=0; i<RecvMsg->bSubjectCount; i++)
	{
		string strEnglishName = SubjectsMgr::GetInstance()->GetEnglishNameByType((SubjectsType)RecvMsg->sSubjectId[i]);
		if (!strEnglishName.empty())
		{
			if (!strUpdateSet.empty())
			{
				strUpdateSet += ",";
			}

			strUpdateSet += " s." + strEnglishName + "=" + StringTool::NumberToStr((int)RecvMsg->bScore[i]);
		}
	}

	char strMysql[512];
	memset(strMysql, 0, sizeof(strMysql));
	sprintf_s(strMysql, sizeof(strMysql), "update studscore s inner join (select userID,account from userinfo where account='%s') u on u.userID=s.userID set %s", RecvMsg->cAccount, strUpdateSet.c_str());

	string strRecord = "~";
	strRecord += StringTool::NumberToStr(RecvMsg->sType) + "~" + RecvMsg->cAccount ;

	MysqlMgr::GetInstance()->InputMsgQueue(strMysql, MYSQL_OPER_UPDATE, ASSIST_ID_UPDATE_SINGLE_SCORE_ACK, SocketId, strRecord);
}

void ScoreProc::DeleteScoreRecvHandle(SOCKET SocketId, void* vpData, unsigned int DataLen)
{
	if ( NULL == vpData)
	{
		printf("%s  消息为空\n", __FUNCTION__);
		return;
	}
	if (DataLen != sizeof(CS_MSG_DELETE_SCORE_REQ))
	{
		printf("%s  长度DataLen[%u]不对，正确长度[%u]\n", __FUNCTION__, DataLen, sizeof(CS_MSG_DELETE_SCORE_REQ));
		return;
	}

	CS_MSG_DELETE_SCORE_REQ* RecvMsg = (CS_MSG_DELETE_SCORE_REQ*)vpData;

	string strDeleteSql = "delete";
	char ch[258];
	if (RecvMsg->sType == 1 && RecvMsg->uUserid[2] == 0)
	{
		memset(ch,0,sizeof(ch));
		sprintf_s(ch, sizeof(ch), " s from studscore s inner join (select userid from userinfo where account='%s') u on s.userid=u.userid", RecvMsg->cAccount);
		strDeleteSql+= ch;
	}
	else if (RecvMsg->sType == 2 && RecvMsg->uUserid[2] == 1 && RecvMsg->uUserid[0] <= RecvMsg->uUserid[1])
	{
		memset(ch,0,sizeof(ch));
		strDeleteSql += " from studscore where ";
		sprintf_s(ch, sizeof(ch), "userID>=%u and userID<=%u", RecvMsg->uUserid[0], RecvMsg->uUserid[1]);
		strDeleteSql += ch;
	}
	else
	{
		SC_MSG_DELETE_SCORE_ACK SendMsg;
		SendMsg.bSucceed = false;
		PackData packData = MsgPackageMgr::Pack(&SendMsg, sizeof(SendMsg), ASSIST_ID_DELETE_SCORE_ACK);
		MsgPackageMgr::Send(SocketId, packData);
		return;
	}

	char strMysql[512];
	memset(strMysql, 0, sizeof(strMysql));
	sprintf_s(strMysql, sizeof(strMysql), "%s", strDeleteSql.c_str());

	string strRecord = "~";
	strRecord += StringTool::NumberToStr(RecvMsg->sType);

	MysqlMgr::GetInstance()->InputMsgQueue(strMysql, MYSQL_OPER_DELETE, ASSIST_ID_DELETE_SCORE_ACK, SocketId, strRecord);
}

void ScoreProc::AlterSubjectReplyHandle(SOCKET SocketId, MYSQL_RES *MysqlRes, string strRecord)
{
	vector<string> vStrRecord= StringTool::Splite(strRecord);

	SC_MSG_ALTER_SUBJECTS_ACK SendMsg;
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

void ScoreProc::AddSingleScoreReplyHandle(SOCKET SocketId, MYSQL_RES *MysqlRes, string strRecord)
{
	SC_MSG_ADD_SINGLE_SCORE_ACK SendMsg;
	SendMsg.bSucceed = true;
	vector<string> vStrRecord= StringTool::Splite(strRecord, "~");
	do 
	{
		if (vStrRecord.size() != 3)
		{
			printf("%s  数据项数量[%u] 记录内数据项数量有错strRecord[%s]\n", __FUNCTION__, vStrRecord.size(), strRecord.c_str());
			SendMsg.bSucceed = false;
			break;
		}
		if ((int)atoi(vStrRecord.at(0).c_str()) != 0)
		{
			printf("%s  数据库操作错误\n", __FUNCTION__);
			SendMsg.bSucceed = false;
			break;
		}
		
		SendMsg.sType = (short)atoi(vStrRecord.at(1).c_str());
		strcpy_s(SendMsg.cAccount, sizeof(SendMsg.cAccount), vStrRecord.at(2).c_str());
	} while (false);

	PackData packData = MsgPackageMgr::Pack(&SendMsg, sizeof(SendMsg), ASSIST_ID_ADD_SINGLE_SCORE_ACK);
	MsgPackageMgr::Send(SocketId, packData);
}

void ScoreProc::AddBatchScoreReplyHandle(SOCKET SocketId, MYSQL_RES *MysqlRes, string strRecord)
{
	unsigned char bSendFlag = 0;
	vector<string> vStrRecord= StringTool::Splite(strRecord, "~");
	do 
	{
		if (vStrRecord.size() != 6)
		{
			printf("%s  数据项数量[%u] 记录内数据项数量有错strRecord[%s]\n", __FUNCTION__, vStrRecord.size(), strRecord.c_str());
			bSendFlag = 2;
			break;
		}
		if ((int)atoi(vStrRecord.at(0).c_str()) != 0)
		{
			printf("%s  数据库操作错误\n", __FUNCTION__);
			bSendFlag = 2;
			break;
		}

		if (vStrRecord.at(5) == "1") //批量分批增加分数完毕，返回结果给客户端
		{
			printf("%s  批量分批增加分数完毕，这次添加了%d分数记录\n", __FUNCTION__, atoi(vStrRecord.at(2).c_str()));
			bSendFlag = 0;
		}
		else
		{
			printf("%s  单次批量增加分数未完毕，继续增加\n", __FUNCTION__);
			bSendFlag = 1;
			break;
		}

		if (bSendFlag==0 && vStrRecord.at(4) == "1") //批量增添分数完毕
		{
			printf("%s  用户[%s]批量增加了%d条分数记录\n", __FUNCTION__, UserInfoMgr::GetInstance()->GetAccountBySocketId(SocketId).c_str(), (int)(atoi(vStrRecord.at(3).c_str())-1)*MAXBATCHREQACKCOUNT+atoi(vStrRecord.at(2).c_str()));
			break;
		}
	} while(false);

	if (bSendFlag==0 || bSendFlag==2)
	{
		SC_MSG_ADD_BATCH_SCORE_ACK SendMsg;
		SendMsg.sType = (short)atoi(vStrRecord.at(1).c_str());
		if (bSendFlag == 0)
		{
			SendMsg.bSucceed = true;
		}
		else
		{
			SendMsg.bSucceed = false;
		}
		PackData packData = MsgPackageMgr::Pack(&SendMsg, sizeof(SendMsg), ASSIST_ID_ADD_BATCH_SCORE_ACK);
		MsgPackageMgr::Send(SocketId, packData);
	}
}

void ScoreProc::SelectSingleScoreReplyHandle(SOCKET SocketId, MYSQL_RES *MysqlRes, string strRecord)
{
	SC_MSG_SELECT_SINGLE_SCORE_ACK SendMsg;
	SendMsg.bResCode = 0;
	do 
	{
		vector<string> vStrRecord= StringTool::Splite(strRecord, "~");
		if (vStrRecord.size() != 5)
		{
			printf("%s  数据项数量[%u] 记录内数据项数量有错strRecord[%s]\n", __FUNCTION__, vStrRecord.size(), strRecord.c_str());
			SendMsg.bResCode = 3;
			break;
		}
		if ((int)atoi(vStrRecord.at(0).c_str()) != 0)
		{
			printf("%s  数据库操作错误\n", __FUNCTION__);
			SendMsg.bResCode = 1;
			break;
		}

		unsigned rowsNum = (unsigned)mysql_num_rows(MysqlRes);
		if (1 == rowsNum) 
		{
			MYSQL_ROW sql_row;
			MYSQL_FIELD *fd = NULL;
			char chTmp[52];
			int j = mysql_num_fields(MysqlRes);
			sql_row=mysql_fetch_row(MysqlRes);
			if (j > 3)
			{
				vector<string> vecStrField;
				for(int i=0; fd=mysql_fetch_field(MysqlRes);i++)
				{
					memset(chTmp, 0, sizeof(chTmp));
					strcpy_s(chTmp, sizeof(chTmp), fd->name);
					vecStrField.push_back(chTmp);
				}
				if (vecStrField.size() != j)
				{
					printf("%s  返回结果字段处理后字段数量有误\n", __FUNCTION__);
					SendMsg.bResCode = 3;
					break;
				}

				SendMsg.bSubjectCount = 0;
				unsigned char bNullSubjectCount = 0;
				for (unsigned i=0; i<vecStrField.size(); i++)
				{
					if (StringTool::ToLowercase(vecStrField.at(i)) == StringTool::ToLowercase(string("userID")))
					{
						if (sql_row[i])
						{
							SendMsg.uUserid = (unsigned)atoi(sql_row[i]);
						}
					}
					else if (StringTool::ToLowercase(vecStrField.at(i)) == StringTool::ToLowercase(string("name")))
					{
						if (sql_row[i])
						{
							strcpy_s(SendMsg.cName, sizeof(SendMsg.cName), sql_row[i]);
						}
						else
						{
							strcpy_s(SendMsg.cName, sizeof(SendMsg.cName), "NULL");
						}
					}
					else if (StringTool::ToLowercase(vecStrField.at(i)) == StringTool::ToLowercase(string("grade")))
					{
						if (sql_row[i])
						{
							strcpy_s(SendMsg.cGrade, sizeof(SendMsg.cGrade), sql_row[i]);
						}
						else
						{
							strcpy_s(SendMsg.cGrade, sizeof(SendMsg.cGrade), "NULL");
						}
					}
					else if (SUBJECTS_TYPE_INVALID != SubjectsMgr::GetInstance()->GetTypeByEnglishName(StringTool::ToLowercase(vecStrField.at(i))))
					{
						if (sql_row[i])
						{
							SendMsg.bScore[SendMsg.bSubjectCount] = (unsigned char)atoi(sql_row[i]);
						}
						else
						{
							SendMsg.bScore[SendMsg.bSubjectCount] = 0;
							bNullSubjectCount++;
						}
						
						SendMsg.bSubjectId[SendMsg.bSubjectCount] = (unsigned char)SubjectsMgr::GetInstance()->GetTypeByEnglishName(StringTool::ToLowercase(vecStrField.at(i)));
						SendMsg.bSubjectCount++;
					}
					else
					{
						printf("%s  没有的字段项[%s]\n", __FUNCTION__, vecStrField.at(i).c_str());
						memset(&SendMsg, 0, sizeof(SendMsg));
						SendMsg.bResCode = 3;
						break;
					}
				}
				SendMsg.sType = (short)atoi(vStrRecord.at(1).c_str());
				strcpy_s(SendMsg.cAccount, sizeof(SendMsg.cAccount), vStrRecord.at(2).c_str());
				if (SendMsg.bSubjectCount == bNullSubjectCount)
				{
					printf("%s  没有成绩信息\n", __FUNCTION__);
					memset(&SendMsg, 0, sizeof(SendMsg));
					SendMsg.bResCode = 2;
					break;
				}
			}
			else
			{
				printf("%s  数据库返回结果错误，列数错误\n", __FUNCTION__);
				SendMsg.bResCode = 3;
				break;
			}
		}
		else
		{
			printf("%s  数据库返回数据记录不正确，现记录数为[%u]（0表示数据库没有对应数据记录，大于1表示数据库记录信息异常）\n", __FUNCTION__,rowsNum);
			SendMsg.bResCode = 2;
			break;
		}
	} while (false);

	PackData packData = MsgPackageMgr::Pack(&SendMsg, sizeof(SendMsg), ASSIST_ID_SELECT_SINGLE_SCORE_ACK);
	MsgPackageMgr::Send(SocketId, packData);
}

void ScoreProc::SelectBatchScoreReplyHandle(SOCKET SocketId, MYSQL_RES *MysqlRes, string strRecord)
{
	unsigned int iRecordCount = 0; //总记录数
	SC_MSG_SELECT_BATCH_SCORE_ACK SendMsg;
	SendMsg.bResCode = 0;
	do 
	{
		vector<string> vStrRecord= StringTool::Splite(strRecord, "~");
		if (vStrRecord.size() != 4)
		{
			printf("%s  数据项数量[%u] 记录内数据项数量有错strRecord[%s]\n", __FUNCTION__, vStrRecord.size(), strRecord.c_str());
			SendMsg.bResCode = 3;
			break;
		}
		if ((int)atoi(vStrRecord.at(0).c_str()) != 0)
		{
			printf("%s  数据库操作错误\n", __FUNCTION__);
			SendMsg.bResCode = 1;
			break;
		}

		unsigned rowsNum = (unsigned)mysql_num_rows(MysqlRes);
		if (rowsNum == 0)
		{
			printf("%s  数据库查询没有记录\n", __FUNCTION__);
			SendMsg.bResCode = 2;
			break;
		}

		int j = mysql_num_fields(MysqlRes);
		if (j < 8)
		{
			printf("%s  数据库返回结果错误，列数错误\n", __FUNCTION__);
			memset(&SendMsg, 0, sizeof(SendMsg));
			SendMsg.bResCode = 3;
			break;
		}

		char chTmp[52];
		MYSQL_FIELD *fd = NULL;
		vector<string> vecStrField;
		for(int i=0; fd=mysql_fetch_field(MysqlRes);i++)
		{
			memset(chTmp, 0, sizeof(chTmp));
			strcpy_s(chTmp, sizeof(chTmp), fd->name);
			vecStrField.push_back(chTmp);
		}
		if (vecStrField.size() != j)
		{
			printf("%s  返回结果字段处理后字段数量有误\n", __FUNCTION__);
			SendMsg.bResCode = 3;
			break;
		}

		MYSQL_ROW sql_row;
		while (sql_row=mysql_fetch_row(MysqlRes)) //获取具体的数据
		{
			SendMsg.bSubjectCount = 0;
			unsigned char bNullSubjectCount = 0;
			for (unsigned i=0; i<vecStrField.size(); i++)
			{
				if (StringTool::ToLowercase(vecStrField.at(i)) == StringTool::ToLowercase(string("rank")))
				{
					if (sql_row[i])
					{
						SendMsg.sRank[iRecordCount%MAXBATCHREQACKCOUNT] = (unsigned)atoi(sql_row[i]);
					}
				}
				else if (StringTool::ToLowercase(vecStrField.at(i)) == StringTool::ToLowercase(string("userID")))
				{
					if (sql_row[i])
					{
						SendMsg.uUserid[iRecordCount%MAXBATCHREQACKCOUNT] = (unsigned)atoi(sql_row[i]);
					}
				}
				else if (StringTool::ToLowercase(vecStrField.at(i)) == StringTool::ToLowercase(string("account")))
				{
					if (sql_row[i])
					{
						strcpy_s(SendMsg.cAccount[iRecordCount%MAXBATCHREQACKCOUNT], sizeof(SendMsg.cAccount[iRecordCount%MAXBATCHREQACKCOUNT]), sql_row[i]);
					}
					else
					{
						strcpy_s(SendMsg.cAccount[iRecordCount%MAXBATCHREQACKCOUNT], sizeof(SendMsg.cAccount[iRecordCount%MAXBATCHREQACKCOUNT]), "NULL");
					}
				}
				else if (StringTool::ToLowercase(vecStrField.at(i)) == StringTool::ToLowercase(string("name")))
				{
					if (sql_row[i])
					{
						strcpy_s(SendMsg.cName[iRecordCount%MAXBATCHREQACKCOUNT], sizeof(SendMsg.cName[iRecordCount%MAXBATCHREQACKCOUNT]), sql_row[i]);
					}
					else
					{
						strcpy_s(SendMsg.cName[iRecordCount%MAXBATCHREQACKCOUNT], sizeof(SendMsg.cName[iRecordCount%MAXBATCHREQACKCOUNT]), "NULL");
					}
				}
				else if (StringTool::ToLowercase(vecStrField.at(i)) == StringTool::ToLowercase(string("grade")))
				{
					if (sql_row[i])
					{
						strcpy_s(SendMsg.cGrade[iRecordCount%MAXBATCHREQACKCOUNT], sizeof(SendMsg.cGrade[iRecordCount%MAXBATCHREQACKCOUNT]), sql_row[i]);
					}
					else
					{
						strcpy_s(SendMsg.cGrade[iRecordCount%MAXBATCHREQACKCOUNT], sizeof(SendMsg.cGrade[iRecordCount%MAXBATCHREQACKCOUNT]), "NULL");
					}
				}
				else if (StringTool::ToLowercase(vecStrField.at(i)) == StringTool::ToLowercase(string("SSum")))
				{
					if (sql_row[i])
					{
						SendMsg.bSum[iRecordCount%MAXBATCHREQACKCOUNT] = (unsigned)atoi(sql_row[i]);
					}
				}
				else if (StringTool::ToLowercase(vecStrField.at(i)) == StringTool::ToLowercase(string("average")))
				{
					if (sql_row[i])
					{
						SendMsg.bAverage[iRecordCount%MAXBATCHREQACKCOUNT] = (unsigned)atoi(sql_row[i]);
					}
				}
				else if (SUBJECTS_TYPE_INVALID != SubjectsMgr::GetInstance()->GetTypeByEnglishName(StringTool::ToLowercase(vecStrField.at(i))))
				{
					if (sql_row[i])
					{
						SendMsg.bScore[iRecordCount%MAXBATCHREQACKCOUNT][SendMsg.bSubjectCount] = (unsigned char)atoi(sql_row[i]);
					}
					else
					{
						SendMsg.bScore[iRecordCount%MAXBATCHREQACKCOUNT][SendMsg.bSubjectCount] = 0;
						bNullSubjectCount++;
					}

					SendMsg.bSubjectId[iRecordCount%MAXBATCHREQACKCOUNT][SendMsg.bSubjectCount] = (unsigned char)SubjectsMgr::GetInstance()->GetTypeByEnglishName(StringTool::ToLowercase(vecStrField.at(i)));
					SendMsg.bSubjectCount++;
				}
				else
				{
					printf("%s  没有的字段项[%s]\n", __FUNCTION__, vecStrField.at(i).c_str());
					memset(&SendMsg, 0, sizeof(SendMsg));
					SendMsg.bResCode = 3;
					break;
				}
			}
			if (SendMsg.bResCode != 0)
			{
				break;
			}
// 			if (SendMsg.bSubjectCount == bNullSubjectCount)
// 			{
// 				printf("%s  没有成绩信息\n", __FUNCTION__);
// 				memset(&SendMsg, 0, sizeof(SendMsg));
// 				SendMsg.bResCode = 2;
// 				break;
// 			}

			iRecordCount++;
			if (iRecordCount % MAXBATCHREQACKCOUNT == 0) //每到达一定数量发送一次记录
			{
				SendMsg.bDataRecord[0] = MAXBATCHREQACKCOUNT;
				SendMsg.bDataRecord[1] = (iRecordCount>0 ? iRecordCount-1 : 0)/MAXBATCHREQACKCOUNT + 1;
				SendMsg.bDataRecord[2] = 0;

				PackData packData = MsgPackageMgr::Pack(&SendMsg, sizeof(SendMsg), ASSIST_ID_SELECT_BATCH_SCORE_ACK);
				MsgPackageMgr::Send(SocketId, packData);

				//清空
				memset(&SendMsg, 0, sizeof(SendMsg));
				SendMsg.bResCode = 0;
			}
			if (iRecordCount == 1 || iRecordCount % MAXBATCHREQACKCOUNT == 0)
			{
				SendMsg.sType = (short)atoi(vStrRecord.at(1).c_str());
				SendMsg.bRankFlag = (unsigned char)atoi(vStrRecord.at(3).c_str());
				StringTool::StrSpliteToUcArray(SendMsg.cCondition, 5, vStrRecord.at(2));
			}
		}
	} while (false);

	if (SendMsg.bResCode == 0)
	{
		SendMsg.bDataRecord[0] = iRecordCount%MAXBATCHREQACKCOUNT;
		SendMsg.bDataRecord[1] = (iRecordCount>0 ? iRecordCount-1 : 0)/MAXBATCHREQACKCOUNT + 1;
	}
	SendMsg.bDataRecord[2] = 1;

	PackData packData = MsgPackageMgr::Pack(&SendMsg, sizeof(SendMsg), ASSIST_ID_SELECT_BATCH_SCORE_ACK);
	MsgPackageMgr::Send(SocketId, packData);
}

void ScoreProc::UpdateSingleScoreReplyHandle(SOCKET SocketId, MYSQL_RES *MysqlRes, string strRecord)
{
	SC_MSG_UPDATE_SINGLE_SCORE_ACK SendMsg;
	SendMsg.bSucceed = true;
	do 
	{
		vector<string> vStrRecord= StringTool::Splite(strRecord, "~");
		if (vStrRecord.size() != 3)
		{
			printf("%s  数据项数量[%u] 记录内数据项数量有错strRecord[%s]\n", __FUNCTION__, vStrRecord.size(), strRecord.c_str());
			SendMsg.bSucceed = false;
			break;
		}
		if ((int)atoi(vStrRecord.at(0).c_str()) != 0)
		{
			printf("%s  数据库操作错误\n", __FUNCTION__);
			SendMsg.bSucceed = false;
			break;
		}

		SendMsg.sType = (short)atoi(vStrRecord.at(1).c_str());
		strcpy_s(SendMsg.cAccount, sizeof(SendMsg.cAccount), vStrRecord.at(2).c_str());

	} while(false);

	PackData packData = MsgPackageMgr::Pack(&SendMsg, sizeof(SendMsg), ASSIST_ID_UPDATE_SINGLE_SCORE_ACK);
	MsgPackageMgr::Send(SocketId, packData);
}

void ScoreProc::DeleteScoreReplyHandle(SOCKET SocketId, MYSQL_RES *MysqlRes, string strRecord)
{
	SC_MSG_DELETE_SCORE_ACK SendMsg;
	SendMsg.bSucceed = true;
	do 
	{
		vector<string> vStrRecord= StringTool::Splite(strRecord, "~");
		if (vStrRecord.size() != 2)
		{
			printf("%s  数据项数量[%u] 记录内数据项数量有错strRecord[%s]\n", __FUNCTION__, vStrRecord.size(), strRecord.c_str());
			SendMsg.bSucceed = false;
			break;
		}
		if ((int)atoi(vStrRecord.at(0).c_str()) != 0)
		{
			printf("%s  数据库操作错误\n", __FUNCTION__);
			SendMsg.bSucceed = false;
			break;
		}

		SendMsg.sType = (short)atoi(vStrRecord.at(1).c_str());
	} while(false);

	PackData packData = MsgPackageMgr::Pack(&SendMsg, sizeof(SendMsg), ASSIST_ID_DELETE_SCORE_ACK);
	MsgPackageMgr::Send(SocketId, packData);
}