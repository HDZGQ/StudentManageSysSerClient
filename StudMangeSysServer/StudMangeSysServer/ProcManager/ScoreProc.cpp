#include "ScoreProc.h"
#include "SubjectsMgr.h"
#include "MsgPackageMgr.h"
#include "MysqlMgr.h"
#include "UserInfoMgr.h"
#include "SubjectsMgr.h"
#include "AuthorityMgr.h"
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

	CS_MSG_GET_SUBJECTS_ACK SendMsg;
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
		CS_MSG_ALTER_SUBJECTS_ACK SendMsg;
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
	//先查询被增加分数的用户是否存在

	char strMysql[512];
	memset(strMysql, 0, sizeof(strMysql));
	sprintf_s(strMysql, sizeof(strMysql), "select userID from userInfo where account='%s'", RecvMsg->cAccount);
	
	string strRecord = "~";
	strRecord += StringTool::NumberToStr(RecvMsg->sType) + "~" + RecvMsg->cAccount + "~" + RecvMsg->sSubjectId + "~" + RecvMsg->sScore;
	UserInfoMgr::GetInstance()->SetRegNeedCountBySocketId(SocketId, 101); //标记

	MysqlMgr::GetInstance()->InputMsgQueue(strMysql, MYSQL_OPER_SELECT, ASSIST_ID_ADD_SINGLE_SCORE_ACK, SocketId, strRecord);
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
		CS_MSG_SELECT_SINGLE_SCORE_ACK SendMsg;
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
		CS_MSG_SELECT_BATCH_SCORE_ACK SendMsg;
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
		CS_MSG_UPDATE_SINGLE_SCORE_ACK SendMsg;
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
	else if (RecvMsg->sType == 2 && RecvMsg->uUserid[2] == 1)
	{
		memset(ch,0,sizeof(ch));
		strDeleteSql += " from studscore where ";
		sprintf_s(ch, sizeof(ch), "userID>=%u and userID<=%u", RecvMsg->uUserid[0], RecvMsg->uUserid[1]);
		strDeleteSql += ch;
	}
	else
	{
		CS_MSG_DELETE_SCORE_ACK SendMsg;
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

void ScoreProc::AddSingleScoreReplyHandle(SOCKET SocketId, MYSQL_RES *MysqlRes, string strRecord)
{
	bool bIsFailEnd = false;
	do 
	{
		short sRegNeedCount = UserInfoMgr::GetInstance()->GetRegNeedCountBySocketId(SocketId);
		if (sRegNeedCount == 101)
		{
			vector<string> vStrRecord= StringTool::Splite(strRecord, "~");
			if (vStrRecord.size() != 5)
			{
				printf("%s  数据库语句执行失败，sRegNeedCount[%d]\n", __FUNCTION__,sRegNeedCount);
				bIsFailEnd = true;
				break;
			}

			vector<string> vStrRes = StringTool::Splite(vStrRecord.at(0), "|");
			vector<string> vStrType = StringTool::Splite(vStrRecord.at(1), "|");
			vector<string> vStrAccount = StringTool::Splite(vStrRecord.at(2), "|");
			//vector<string> vStrSubjectId = StringTool::Splite(vStrRecord.at(3), "|");
			vector<string> vStrScore = StringTool::Splite(vStrRecord.at(4), "|");

			string strEnglishName = SubjectsMgr::GetInstance()->GetStrEnglishNameByStrType(vStrRecord.at(3), "|");
			vector<string> vStrSubjectEnglishName = StringTool::Splite(strEnglishName, "|");

			if (vStrRes.size() < 1 || (int)atoi(vStrRes.at(0).c_str()) != 0)
			{
				printf("%s  数据库操作错误，sRegNeedCount[%d]\n", __FUNCTION__,sRegNeedCount);
				bIsFailEnd = true;
				break;
			}

			if (vStrType.size() > 0 && vStrAccount.size() > 0 && vStrSubjectEnglishName.size() > 0 && vStrScore.size() > 0 && vStrSubjectEnglishName.size()==vStrScore.size())
			{
				string strSubjectEnglishName = StringTool::CombVecToStr(vStrSubjectEnglishName, ",");
				string strScore = StringTool::CombVecToStr(vStrScore, ",");
				string strUpdateSet = StringTool::CombToSqlUpdateSetStr(strSubjectEnglishName, strScore, ",");

				unsigned rowsNum = (unsigned)mysql_num_rows(MysqlRes);
				if (1 == rowsNum) //玩家已经注册
				{
					MYSQL_ROW sql_row;
					int j = mysql_num_fields(MysqlRes);
					sql_row=mysql_fetch_row(MysqlRes);
					if (1 == j)
					{
						unsigned userid = (unsigned)atoi(sql_row[0]);

						//插入成绩表前先检测成绩表是否已经有对应玩家id
						char strMysql[512];
						memset(strMysql, 0, sizeof(strMysql));
						sprintf_s(strMysql, sizeof(strMysql), "call AddSingleScoreHaveRegister('%s', %u, '%s', '%s')", strSubjectEnglishName.c_str(), userid, strScore.c_str(), strUpdateSet.c_str());

						UserInfoMgr::GetInstance()->SetRegNeedCountBySocketId(SocketId, 102); //标记，玩家已经注册情况下插入成绩表
						string strRecordTmp = "";
						strRecordTmp += StringTool::NumberToStr((int)atoi(vStrType.at(0).c_str())) + "|" + vStrAccount.at(0);

						MysqlMgr::GetInstance()->InputMsgQueue(strMysql, MYSQL_OPER_INSERT, ASSIST_ID_ADD_SINGLE_SCORE_ACK, SocketId, strRecordTmp);
					}
					else
					{
						printf("%s  数据库返回结果错误，列数错误\n", __FUNCTION__);
						bIsFailEnd = true;
						break;
					}
				}
				else if (0 == rowsNum) //玩家没有注册，要先帮忙注册，注册成功还得初始化权限
				{
					string strAccount = vStrAccount.at(0);
					string strName = strAccount; //名称后续自己修改
					string strPwd = "123456";
					string strSex = "0";
					string strIdent = "1";

					string strInsertInfo = "''" + strAccount + "'',''" + strName + "'',''" + strPwd + "'',''" + strSex + "'',''" + strIdent + "''";

					//根据身份使用默认权限
					vector<OperPermission> vecOper;
					AuthorityMgr::GetDefaultAuthorityByIdent((IdentType)atoi(strIdent.c_str()), vecOper);
					string strAuthority = StringTool::CombVecToStr(vecOper);

					char strMysql[512];
					memset(strMysql, 0, sizeof(strMysql));
					//sprintf_s(strMysql, sizeof(strMysql), "insert into userInfo(account, name, password, sex, Ident) values('%s', '%s', '%s', %s, %s)", strAccount.c_str(), strName.c_str(), strPwd.c_str(), strSex.c_str(), strIdent.c_str());
					sprintf_s(strMysql, sizeof(strMysql), "call AddSingleScoreNotRegister('%s', '%s','%s', '%s','%s')", strInsertInfo.c_str(), strAccount.c_str(), strAuthority.c_str(), strSubjectEnglishName.c_str(), strScore.c_str());

					UserInfoMgr::GetInstance()->SetRegNeedCountBySocketId(SocketId, 102); //标记，玩家已经注册情况下插入成绩表

// 					string strRecordTmp = "";
// 					strRecordTmp += StringTool::NumberToStr((int)atoi(vStrType.at(0).c_str())) + "|" + vStrAccount.at(0) + "|" + strSubjectEnglishName + "|" + strScore;
					string strRecordTmp = "";
					strRecordTmp += StringTool::NumberToStr((int)atoi(vStrType.at(0).c_str())) + "|" + vStrAccount.at(0);

					MysqlMgr::GetInstance()->InputMsgQueue(strMysql, MYSQL_OPER_INSERT, ASSIST_ID_ADD_SINGLE_SCORE_ACK, SocketId, strRecordTmp);
				}
				else
				{
					printf("%s  数据库返回结果错误，记录数错误\n", __FUNCTION__);
					bIsFailEnd = true;
					break;
				}

			}
			else
			{
				printf("%s  客户端数据有误错误\n", __FUNCTION__);
				bIsFailEnd = true;
				break;
			}
		}
		else if (sRegNeedCount == 102)
		{
			vector<string> vStrRecord= StringTool::Splite(strRecord);
			if (vStrRecord.size() < 3 || (int)atoi(vStrRecord.at(0).c_str()) != 0)
			{
				printf("%s  数据库语句执行失败，sRegNeedCount[%d]\n", __FUNCTION__,sRegNeedCount);
				bIsFailEnd = true;
				break;
			}
			
			CS_MSG_ADD_SINGLE_SCORE_ACK SendMsg;
			SendMsg.bSucceed = true;
			SendMsg.sType = (short)atoi(vStrRecord.at(1).c_str());
			strcpy_s(SendMsg.cAccount, sizeof(SendMsg.cAccount), vStrRecord.at(2).c_str());
			
			UserInfoMgr::GetInstance()->SetRegNeedCountBySocketId(SocketId, 0); //初始化

			PackData packData = MsgPackageMgr::Pack(&SendMsg, sizeof(SendMsg), ASSIST_ID_ADD_SINGLE_SCORE_ACK);
			MsgPackageMgr::Send(SocketId, packData);
		}
// 		else if (sRegNeedCount == 103)
// 		{
// 			vector<string> vStrRecord= StringTool::Splite(strRecord);
// 			if (vStrRecord.size() < 5 || (int)atoi(vStrRecord.at(0).c_str()) != 0)
// 			{
// 				printf("%s  数据库语句执行失败，sRegNeedCount[%d]\n", __FUNCTION__,sRegNeedCount);
// 				bIsFailEnd = true;
// 				break;
// 			}
// 
// 			short sType = (short)atoi(vStrRecord.at(1).c_str());
// 			string strAccount = vStrRecord.at(2);
// 			string strSubjectEngName = vStrRecord.at(3);
// 			string strScore = vStrRecord.at(4);
// 
// 			//插入成绩表
// 			char strMysql[512];
// 			memset(strMysql, 0, sizeof(strMysql));
// 			sprintf_s(strMysql, sizeof(strMysql), "insert into studScore(userID, %s) (select userID,%s from userInfo where account='%s')",strSubjectEngName.c_str(), strScore.c_str(), strAccount.c_str());
// 
// 			UserInfoMgr::GetInstance()->SetRegNeedCountBySocketId(SocketId, 102); //标记，玩家已经注册情况下插入成绩表
// 			string strRecordTmp = "";
// 			strRecordTmp +=  vStrRecord.at(1) + "|" + vStrRecord.at(2) ;
// 
// 			MysqlMgr::GetInstance()->InputMsgQueue(strMysql, MYSQL_OPER_INSERT, ASSIST_ID_ADD_SINGLE_SCORE_ACK, SocketId, strRecordTmp);
// 
// 		}
		else
		{
			printf("%s  sRegNeedCount[%d] error\n", __FUNCTION__,sRegNeedCount);
			bIsFailEnd = true;
			break;
		}
	} while (false);

	if (bIsFailEnd)
	{
		CS_MSG_ADD_SINGLE_SCORE_ACK SendMsg;
		SendMsg.bSucceed = false;

		UserInfoMgr::GetInstance()->SetRegNeedCountBySocketId(SocketId, 0); //初始化

		PackData packData = MsgPackageMgr::Pack(&SendMsg, sizeof(SendMsg), ASSIST_ID_ADD_SINGLE_SCORE_ACK);
		MsgPackageMgr::Send(SocketId, packData);
	}
}

void ScoreProc::SelectSingleScoreReplyHandle(SOCKET SocketId, MYSQL_RES *MysqlRes, string strRecord)
{
	CS_MSG_SELECT_SINGLE_SCORE_ACK SendMsg;
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
	CS_MSG_SELECT_BATCH_SCORE_ACK SendMsg;
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
						SendMsg.sRank[iRecordCount%MAXBATCHSELECTACKCOUNT] = (unsigned)atoi(sql_row[i]);
					}
				}
				else if (StringTool::ToLowercase(vecStrField.at(i)) == StringTool::ToLowercase(string("userID")))
				{
					if (sql_row[i])
					{
						SendMsg.uUserid[iRecordCount%MAXBATCHSELECTACKCOUNT] = (unsigned)atoi(sql_row[i]);
					}
				}
				else if (StringTool::ToLowercase(vecStrField.at(i)) == StringTool::ToLowercase(string("account")))
				{
					if (sql_row[i])
					{
						strcpy_s(SendMsg.cAccount[iRecordCount%MAXBATCHSELECTACKCOUNT], sizeof(SendMsg.cAccount[iRecordCount%MAXBATCHSELECTACKCOUNT]), sql_row[i]);
					}
					else
					{
						strcpy_s(SendMsg.cAccount[iRecordCount%MAXBATCHSELECTACKCOUNT], sizeof(SendMsg.cAccount[iRecordCount%MAXBATCHSELECTACKCOUNT]), "NULL");
					}
				}
				else if (StringTool::ToLowercase(vecStrField.at(i)) == StringTool::ToLowercase(string("name")))
				{
					if (sql_row[i])
					{
						strcpy_s(SendMsg.cName[iRecordCount%MAXBATCHSELECTACKCOUNT], sizeof(SendMsg.cName[iRecordCount%MAXBATCHSELECTACKCOUNT]), sql_row[i]);
					}
					else
					{
						strcpy_s(SendMsg.cName[iRecordCount%MAXBATCHSELECTACKCOUNT], sizeof(SendMsg.cName[iRecordCount%MAXBATCHSELECTACKCOUNT]), "NULL");
					}
				}
				else if (StringTool::ToLowercase(vecStrField.at(i)) == StringTool::ToLowercase(string("grade")))
				{
					if (sql_row[i])
					{
						strcpy_s(SendMsg.cGrade[iRecordCount%MAXBATCHSELECTACKCOUNT], sizeof(SendMsg.cGrade[iRecordCount%MAXBATCHSELECTACKCOUNT]), sql_row[i]);
					}
					else
					{
						strcpy_s(SendMsg.cGrade[iRecordCount%MAXBATCHSELECTACKCOUNT], sizeof(SendMsg.cGrade[iRecordCount%MAXBATCHSELECTACKCOUNT]), "NULL");
					}
				}
				else if (StringTool::ToLowercase(vecStrField.at(i)) == StringTool::ToLowercase(string("SSum")))
				{
					if (sql_row[i])
					{
						SendMsg.bSum[iRecordCount%MAXBATCHSELECTACKCOUNT] = (unsigned)atoi(sql_row[i]);
					}
				}
				else if (StringTool::ToLowercase(vecStrField.at(i)) == StringTool::ToLowercase(string("average")))
				{
					if (sql_row[i])
					{
						SendMsg.bAverage[iRecordCount%MAXBATCHSELECTACKCOUNT] = (unsigned)atoi(sql_row[i]);
					}
				}
				else if (SUBJECTS_TYPE_INVALID != SubjectsMgr::GetInstance()->GetTypeByEnglishName(StringTool::ToLowercase(vecStrField.at(i))))
				{
					if (sql_row[i])
					{
						SendMsg.bScore[iRecordCount%MAXBATCHSELECTACKCOUNT][SendMsg.bSubjectCount] = (unsigned char)atoi(sql_row[i]);
					}
					else
					{
						SendMsg.bScore[iRecordCount%MAXBATCHSELECTACKCOUNT][SendMsg.bSubjectCount] = 0;
						bNullSubjectCount++;
					}

					SendMsg.bSubjectId[iRecordCount%MAXBATCHSELECTACKCOUNT][SendMsg.bSubjectCount] = (unsigned char)SubjectsMgr::GetInstance()->GetTypeByEnglishName(StringTool::ToLowercase(vecStrField.at(i)));
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
			if (iRecordCount % MAXBATCHSELECTACKCOUNT == 0) //每到达一定数量发送一次记录
			{
				SendMsg.bDataRecord[0] = MAXBATCHSELECTACKCOUNT;
				SendMsg.bDataRecord[1] = (iRecordCount>0 ? iRecordCount-1 : 0)/MAXBATCHSELECTACKCOUNT + 1;
				SendMsg.bDataRecord[2] = 0;

				PackData packData = MsgPackageMgr::Pack(&SendMsg, sizeof(SendMsg), ASSIST_ID_SELECT_BATCH_SCORE_ACK);
				MsgPackageMgr::Send(SocketId, packData);

				//清空
				memset(&SendMsg, 0, sizeof(SendMsg));
				SendMsg.bResCode = 0;
			}
			if (iRecordCount == 1 || iRecordCount % MAXBATCHSELECTACKCOUNT == 0)
			{
				SendMsg.sType = (short)atoi(vStrRecord.at(1).c_str());
				SendMsg.bRankFlag = (unsigned char)atoi(vStrRecord.at(3).c_str());
				StringTool::StrSpliteToUcArray(SendMsg.cCondition, 5, vStrRecord.at(2));
			}
		}
	} while (false);

	if (SendMsg.bResCode == 0)
	{
		SendMsg.bDataRecord[0] = iRecordCount%MAXBATCHSELECTACKCOUNT;
		SendMsg.bDataRecord[1] = (iRecordCount>0 ? iRecordCount-1 : 0)/MAXBATCHSELECTACKCOUNT + 1;
	}
	SendMsg.bDataRecord[2] = 1;

	PackData packData = MsgPackageMgr::Pack(&SendMsg, sizeof(SendMsg), ASSIST_ID_SELECT_BATCH_SCORE_ACK);
	MsgPackageMgr::Send(SocketId, packData);
}

void ScoreProc::UpdateSingleScoreReplyHandle(SOCKET SocketId, MYSQL_RES *MysqlRes, string strRecord)
{
	CS_MSG_UPDATE_SINGLE_SCORE_ACK SendMsg;
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
	CS_MSG_DELETE_SCORE_ACK SendMsg;
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