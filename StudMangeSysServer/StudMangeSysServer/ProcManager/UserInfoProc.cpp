#include "UserInfoProc.h"
#include "StringTool.h"
#include "MysqlMgr.h"
#include "MsgPackageMgr.h"
#include "UserInfoMgr.h"

UserInfoProc::UserInfoProc()
{

}

UserInfoProc::~UserInfoProc()
{

}

void UserInfoProc::AddSingleUserInfoRecvHandle(SOCKET SocketId, void* vpData, unsigned int DataLen)
{
	if ( NULL == vpData)
	{
		printf("%s  消息为空\n", __FUNCTION__);
		return;
	}
	if (DataLen != sizeof(CS_MSG_ADD_SINGLE_USERINFO_REQ))
	{
		printf("%s  长度DataLen[%u]不对，正确长度[%u]\n", __FUNCTION__, DataLen, sizeof(CS_MSG_ADD_SINGLE_USERINFO_REQ));
		return;
	}

	CS_MSG_ADD_SINGLE_USERINFO_REQ* RecvMsg = (CS_MSG_ADD_SINGLE_USERINFO_REQ*)vpData;
	short sMyIdent = UserInfoMgr::GetInstance()->GetIdentBySocketId(SocketId);
	if (!(RecvMsg->sIdent < sMyIdent ||  (sMyIdent==1 && RecvMsg->sIdent == 1 && UserInfoMgr::GetInstance()->IsHaveOneAuthorityBySocketId(SocketId, OPER_PER_ADDSINGLEUSERINFO))))
	{
		SC_MSG_ADD_SINGLE_USERINFO_ACK SendMsg;
		SendMsg.bSucceed = false;
		PackData packData = MsgPackageMgr::Pack(&SendMsg, sizeof(SendMsg), ASSIST_ID_ADD_SINGLE_USERINFO_ACK);
		MsgPackageMgr::Send(SocketId, packData);
		printf("%s  本用户的身边标识[%d]不能添加该身边标识[%u]的用户信息\n", __FUNCTION__, sMyIdent, RecvMsg->sIdent);
		return;
	}

	string strAccount = RecvMsg->cAccount;
	string strName = RecvMsg->cName;
	string strPwd = RecvMsg->cPWD;
	string strSex = StringTool::NumberToStr((int)RecvMsg->sSex);
	string strIdent = StringTool::NumberToStr((int)RecvMsg->sIdent);
	string strMajor = RecvMsg->cMajor;
	string strGrade = RecvMsg->cGrade;
	string strInsertInfo = "''" + strAccount + "'',''" + strName + "'',''" + strPwd + "''," + strSex + "," + strIdent + ",''" + strMajor + "'',''" + strGrade + "''";

	//根据身份使用默认权限
	vector<OperPermission> vecOper;
	UserInfoMgr::GetInstance()->GetDefaultAuthorityByIdent((IdentType)RecvMsg->sIdent, vecOper);
	string strAuthority = StringTool::CombVecToStr(vecOper);

	char strMysql[512];
	memset(strMysql, 0, sizeof(strMysql));
	sprintf_s(strMysql, sizeof(strMysql), "call AddSingleUserInfo('%s', '%s', '%s')", strInsertInfo.c_str(), RecvMsg->cAccount, strAuthority.c_str());

	string strRecord = "~";
	strRecord += RecvMsg->cAccount;

	MysqlMgr::GetInstance()->InputMsgQueue(strMysql, MYSQL_OPER_CALL_PROC, ASSIST_ID_ADD_SINGLE_USERINFO_ACK, SocketId, strRecord);
}

void UserInfoProc::AddBatchUserInfoRecvHandle(SOCKET SocketId, void* vpData, unsigned int DataLen)
{
	if ( NULL == vpData)
	{
		printf("%s  消息为空\n", __FUNCTION__);
		return;
	}
	if (DataLen != sizeof(CS_MSG_ADD_BATCH_USERINFO_REQ))
	{
		printf("%s  长度DataLen[%u]不对，正确长度[%u]\n", __FUNCTION__, DataLen, sizeof(CS_MSG_ADD_BATCH_USERINFO_REQ));
		return;
	}

	CS_MSG_ADD_BATCH_USERINFO_REQ* RecvMsg = (CS_MSG_ADD_BATCH_USERINFO_REQ*)vpData;
	if (RecvMsg->bRecordCount < 1 || RecvMsg->bRecordCount > MAXBATCHREQACKCOUNT)
	{
		printf("%s  客户端传过来的成绩记录数不正确RecvMsg->bRecordCount[%u]\n", __FUNCTION__, (unsigned)RecvMsg->bRecordCount);
		return;
	}
	if (RecvMsg->bRecordNO <= 1)
	{
		UserInfoMgr::GetInstance()->SetStrTmpDataBySocketId(SocketId, "0|0"); //记录成功总次数和当次成功次数
	}
	else
	{
		string strTmpData = UserInfoMgr::GetInstance()->GetStrTmpDataBySocketId(SocketId);
		vector<string> vecStr = StringTool::Splite(strTmpData);
		if (vecStr.size() != 2)
		{
			SC_MSG_ADD_BATCH_USERINFO_ACK SendMsg;
			SendMsg.bSucceed = false;
			PackData packData = MsgPackageMgr::Pack(&SendMsg, sizeof(SendMsg), ASSIST_ID_ADD_BATCH_USERINFO_ACK);
			MsgPackageMgr::Send(SocketId, packData);

			printf("%s  保存的临时数据不正确\n", __FUNCTION__);
			return;
		}
		string newStrTMpData = vecStr.at(0) + "|0";
		UserInfoMgr::GetInstance()->SetStrTmpDataBySocketId(SocketId, newStrTMpData); //记录成功总次数和当次成功次数
	}

	short sMyIdent = UserInfoMgr::GetInstance()->GetIdentBySocketId(SocketId);

	for (unsigned char i=0; i < RecvMsg->bRecordCount; i++)
	{
		if (!(RecvMsg->sIdent[i] < sMyIdent ||  (sMyIdent==1 && RecvMsg->sIdent[i] == 1 && UserInfoMgr::GetInstance()->IsHaveOneAuthorityBySocketId(SocketId, OPER_PER_ADDBATCHUSERINFO))))
		{
			string strRecord = "~";
			strRecord += string(RecvMsg->cAccount[i]) + "~" + StringTool::NumberToStr((int)RecvMsg->bRecordCount) + "~" + StringTool::NumberToStr((int)RecvMsg->bRecordNO) + "~" + StringTool::NumberToStr((int)RecvMsg->bEnd) + "~" + StringTool::NumberToStr((int)((i==RecvMsg->bRecordCount-1) ? 1 : 0));
			MysqlMgr::GetInstance()->InputMsgQueue("", MYSQL_OPER_CALL_PROC, ASSIST_ID_ADD_BATCH_USERINFO_ACK, SocketId, strRecord); //空的数据库语句不执行的

			printf("%s  本用户的身边标识[%d]不能添加该身边标识[%u]的用户信息\n", __FUNCTION__, sMyIdent, RecvMsg->sIdent[i]);
			continue;
		}

		string strAccount = RecvMsg->cAccount[i];
		string strName = RecvMsg->cName[i];
		string strPwd = RecvMsg->cPWD[i];
		string strSex = StringTool::NumberToStr((int)RecvMsg->sSex[i]);
		string strIdent = StringTool::NumberToStr((int)RecvMsg->sIdent[i]);
		string strMajor = RecvMsg->cMajor[i];
		string strGrade = RecvMsg->cGrade[i];
		string strInsertInfo = "''" + strAccount + "'',''" + strName + "'',''" + strPwd + "''," + strSex + "," + strIdent + ",''" + strMajor + "'',''" + strGrade + "''";

		//根据身份使用默认权限
		vector<OperPermission> vecOper;
		UserInfoMgr::GetInstance()->GetDefaultAuthorityByIdent((IdentType)RecvMsg->sIdent[i], vecOper);
		string strAuthority = StringTool::CombVecToStr(vecOper);

		char strMysql[512];
		memset(strMysql, 0, sizeof(strMysql));
		sprintf_s(strMysql, sizeof(strMysql), "call AddSingleUserInfo('%s', '%s', '%s')", strInsertInfo.c_str(), RecvMsg->cAccount[i], strAuthority.c_str());

		string strRecord = "~";
		strRecord += string(RecvMsg->cAccount[i]) + "~" + StringTool::NumberToStr((int)RecvMsg->bRecordCount) + "~" + StringTool::NumberToStr((int)RecvMsg->bRecordNO) + "~" + StringTool::NumberToStr((int)RecvMsg->bEnd) + "~" + StringTool::NumberToStr((int)((i==RecvMsg->bRecordCount-1) ? 1 : 0));

		MysqlMgr::GetInstance()->InputMsgQueue(strMysql, MYSQL_OPER_CALL_PROC, ASSIST_ID_ADD_BATCH_USERINFO_ACK, SocketId, strRecord);
	}
}

void UserInfoProc::SelectSingleUserInfoRecvHandle(SOCKET SocketId, void* vpData, unsigned int DataLen)
{
	if ( NULL == vpData)
	{
		printf("%s  消息为空\n", __FUNCTION__);
		return;
	}
	if (DataLen != sizeof(CS_MSG_SELECT_SINGLE_USERINFO_REQ))
	{
		printf("%s  长度DataLen[%u]不对，正确长度[%u]\n", __FUNCTION__, DataLen, sizeof(CS_MSG_SELECT_SINGLE_USERINFO_REQ));
		return;
	}

	CS_MSG_SELECT_SINGLE_USERINFO_REQ* RecvMsg = (CS_MSG_SELECT_SINGLE_USERINFO_REQ*)vpData;

	if (RecvMsg->uRecordDataTmp == 1)
	{
		UserInfoMgr::GetInstance()->SetStrTmpDataBySocketId(SocketId, "");
	}
	
	char strMysql[512];
	memset(strMysql, 0, sizeof(strMysql));
	sprintf_s(strMysql, sizeof(strMysql), "select userID, account, password, name, sex, Ident, major, grade from userInfo where account='%s'", RecvMsg->cAccount);

	string strRecord = "~";
	strRecord += string(RecvMsg->cAccount) + "~" + StringTool::NumberToStr((int)RecvMsg->uRecordDataTmp) ;

	MysqlMgr::GetInstance()->InputMsgQueue(strMysql, MYSQL_OPER_SELECT, ASSIST_ID_SELECT_SINGLE_USERINFO_ACK, SocketId, strRecord);
}

void UserInfoProc::UpdateSingleUserInfoRecvHandle(SOCKET SocketId, void* vpData, unsigned int DataLen)
{
	if ( NULL == vpData)
	{
		printf("%s  消息为空\n", __FUNCTION__);
		return;
	}
	if (DataLen != sizeof(CS_MSG_UPDATE_SINGLE_USERINFO_REQ))
	{
		printf("%s  长度DataLen[%u]不对，正确长度[%u]\n", __FUNCTION__, DataLen, sizeof(CS_MSG_UPDATE_SINGLE_USERINFO_REQ));
		return;
	}

	CS_MSG_UPDATE_SINGLE_USERINFO_REQ* RecvMsg = (CS_MSG_UPDATE_SINGLE_USERINFO_REQ*)vpData;
	if (RecvMsg->bUserInfoFieldCount == 0)
	{
		SC_MSG_UPDATE_SINGLE_USERINFO_ACK SendMsg;
		SendMsg.bSucceed = false;
		PackData packData = MsgPackageMgr::Pack(&SendMsg, sizeof(SendMsg), ASSIST_ID_UPDATE_SINGLE_USERINFO_ACK);
		MsgPackageMgr::Send(SocketId, packData);

		printf("%s  RecvMsg->bUserInfoFieldCount用户信息字段为0\n", __FUNCTION__);
		return;
	}

	//更改自己的用户信息，会同时更改到数据库和内存；更改其他用户的只会更改到数据库，用户需要重新登录才有效

	//根据身份标识判断是否可以更改对象用户信息  -- 学生教师管理员都有单条更改用户信息的权限。所以只需要只能更改自己和比自己低身份标识的用户，用户可以更改自己的密码，但是不能更改别人的。特殊的是管理员可以更改教师和学的的密码
	vector<string> vecDataTmp = StringTool::Splite(UserInfoMgr::GetInstance()->GetStrTmpDataBySocketId(SocketId)); //取出更新前查询结果保存的临时数据
	UserInfoMgr::GetInstance()->SetStrTmpDataBySocketId(SocketId, "");
	if (vecDataTmp.size() != 2)
	{
		SC_MSG_UPDATE_SINGLE_USERINFO_ACK SendMsg;
		SendMsg.bSucceed = false;
		PackData packData = MsgPackageMgr::Pack(&SendMsg, sizeof(SendMsg), ASSIST_ID_UPDATE_SINGLE_USERINFO_ACK);
		MsgPackageMgr::Send(SocketId, packData);

		printf("%s  更新前查询时保存的临时数据有误（暂时保存被查询身份标识和账号）\n", __FUNCTION__);
		return;
	}
	short sMyIdent = UserInfoMgr::GetInstance()->GetIdentBySocketId(SocketId);
	string strMyAccount = UserInfoMgr::GetInstance()->GetAccountBySocketId(SocketId);
	short sObjIdent = (short)atoi(vecDataTmp.at(0).c_str());
	string strObjAccount = vecDataTmp.at(1);
	bool bCanUpdate[8]; //本用户可更改对象用户的字段
	memset(bCanUpdate, false, sizeof(bCanUpdate));
	if (!UserInfoMgr::GetInstance()->GetCanUpdateField(sMyIdent, strMyAccount, sObjIdent, strObjAccount, bCanUpdate, sizeof(bCanUpdate)/sizeof(bool))) //可更改的用户信息字段
	{
		SC_MSG_UPDATE_SINGLE_USERINFO_ACK SendMsg;
		SendMsg.bSucceed = false;
		PackData packData = MsgPackageMgr::Pack(&SendMsg, sizeof(SendMsg), ASSIST_ID_UPDATE_SINGLE_USERINFO_ACK);
		MsgPackageMgr::Send(SocketId, packData);

		printf("%s  用户名[%s]身份标识[%d]更新不了身份标识[%d]的单条用户信息\n", __FUNCTION__, UserInfoMgr::GetInstance()->GetAccountBySocketId(SocketId).c_str(), sMyIdent, sObjIdent);
		return;
	}

	string strUpdateSet;
	int iUpdateMySelfAccount = 0; //是否更改了自己的账号，标记下。如果执行成功，同步更改用户内存的用户数据
	string strUpdateMySelfAccount; //记录更改自己的账号
	for (unsigned char i=0; i<RecvMsg->bUserInfoFieldCount; i++)
	{
		if (!strUpdateSet.empty())
		{
			strUpdateSet += ",";
		}
		if ((RecvMsg->sUserInfoField[i] == 4 || (RecvMsg->sUserInfoField[i] == 5 && sMyIdent == 3 && string(RecvMsg->cUserInfoValue[i]) != "3")) && bCanUpdate[RecvMsg->sUserInfoField[i]]) //性别和身份标识 --只有管理员才会涉及到身份标识，更改学生教师的
		{
			strUpdateSet += UserInfoMgr::GetInstance()->GetEnglishUserInfoFieldByField(RecvMsg->sUserInfoField[i]);
			strUpdateSet += string("=") + RecvMsg->cUserInfoValue[i];
		}
		else if (((RecvMsg->sUserInfoField[i] > 0 && RecvMsg->sUserInfoField[i] < 4) || (RecvMsg->sUserInfoField[i] > 5  && RecvMsg->sUserInfoField[i] < 8)) && bCanUpdate[RecvMsg->sUserInfoField[i]])
		{
			strUpdateSet += UserInfoMgr::GetInstance()->GetEnglishUserInfoFieldByField(RecvMsg->sUserInfoField[i]);
			strUpdateSet += string("='") + RecvMsg->cUserInfoValue[i] + "'";

			if (RecvMsg->sUserInfoField[i]==1 && sMyIdent==sObjIdent && strMyAccount==strObjAccount)
			{
				iUpdateMySelfAccount = 1;
				strUpdateMySelfAccount = RecvMsg->cUserInfoValue[i];
			}
		}
		else
		{
			SC_MSG_UPDATE_SINGLE_USERINFO_ACK SendMsg;
			SendMsg.bSucceed = false;
			PackData packData = MsgPackageMgr::Pack(&SendMsg, sizeof(SendMsg), ASSIST_ID_UPDATE_SINGLE_USERINFO_ACK);
			MsgPackageMgr::Send(SocketId, packData);

			printf("%s  不能更改该用户信息字段id[%u]，更改新值为[%s]\n", __FUNCTION__, RecvMsg->sUserInfoField[i], RecvMsg->cUserInfoValue[i]);
			return;
		}
	}

	if (strUpdateSet.empty())
	{
		SC_MSG_UPDATE_SINGLE_USERINFO_ACK SendMsg;
		SendMsg.bSucceed = false;
		PackData packData = MsgPackageMgr::Pack(&SendMsg, sizeof(SendMsg), ASSIST_ID_UPDATE_SINGLE_USERINFO_ACK);
		MsgPackageMgr::Send(SocketId, packData);

		printf("%s  没有可更新的东西\n", __FUNCTION__);
		return;
	}

	char strMysql[512];
	memset(strMysql, 0, sizeof(strMysql));
	sprintf_s(strMysql, sizeof(strMysql), "update userInfo set %s where account='%s'", strUpdateSet.c_str(), RecvMsg->cAccount);

	string strRecord = "~";
	strRecord += StringTool::NumberToStr((int)RecvMsg->sType) + "~" + RecvMsg->cAccount + "~" + StringTool::NumberToStr(iUpdateMySelfAccount)+"|"+strUpdateMySelfAccount;

	MysqlMgr::GetInstance()->InputMsgQueue(strMysql, MYSQL_OPER_UPDATE, ASSIST_ID_UPDATE_SINGLE_USERINFO_ACK, SocketId, strRecord);
}

void UserInfoProc::AddSingleUserInfoReplyHandle(SOCKET SocketId, MYSQL_RES *MysqlRes, string strRecord)
{
	SC_MSG_ADD_SINGLE_USERINFO_ACK SendMsg;
	SendMsg.bSucceed = true;
	vector<string> vStrRecord= StringTool::Splite(strRecord, "~");
	do 
	{
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

		strcpy_s(SendMsg.cAccount, sizeof(SendMsg.cAccount), vStrRecord.at(1).c_str());
	} while(false);

	PackData packData = MsgPackageMgr::Pack(&SendMsg, sizeof(SendMsg), ASSIST_ID_ADD_SINGLE_USERINFO_ACK);
	MsgPackageMgr::Send(SocketId, packData);
}

void UserInfoProc::AddBatchUserInfoReplyHandle(SOCKET SocketId, MYSQL_RES *MysqlRes, string strRecord)
{
	unsigned char bSendFlag = 0;
	vector<string> vStrRecord= StringTool::Splite(strRecord, "~");

	string strTmpData = UserInfoMgr::GetInstance()->GetStrTmpDataBySocketId(SocketId);
	vector<string> vecStrTmpData = StringTool::Splite(strTmpData);
	do 
	{
		if (vStrRecord.size() != 6)
		{
			printf("%s  数据项数量[%u] 记录内数据项数量有错strRecord[%s]\n", __FUNCTION__, vStrRecord.size(), strRecord.c_str());
			bSendFlag = 2;
			break;
		}
		if ((int)atoi(vStrRecord.at(0).c_str()) != 0) //这条sql执行失败，但还可以继续插入其他的信息记录
		{
			printf("%s  数据库操作错误\n", __FUNCTION__);
			bSendFlag = 1;
			break;
		}
		if (vecStrTmpData.size() != 2)
		{
			bSendFlag = 2;
			//UserInfoMgr::GetInstance()->SetStrTmpDataBySocketId(SocketId,""); //清空临时数据
			printf("%s  保存的临时数据不正确\n", __FUNCTION__);
			break;
		}
		
		vecStrTmpData.at(0) = StringTool::NumberToStr(((int)atoi(vecStrTmpData.at(0).c_str()))+1);
		vecStrTmpData.at(1) = StringTool::NumberToStr(((int)atoi(vecStrTmpData.at(1).c_str()))+1);
		//记录成功的
		string newStrTmpData = vecStrTmpData.at(0) + "|" + vecStrTmpData.at(1);
		UserInfoMgr::GetInstance()->SetStrTmpDataBySocketId(SocketId, newStrTmpData); //记录成功总次数和当次成功次数


		if (vStrRecord.at(5) != "1") 
		{
			printf("%s  单次批量增加用户信息未完毕，继续增加\n", __FUNCTION__);
			bSendFlag = 0;
		}
	} while(false);

	if ((vStrRecord.size() == 6 && vStrRecord.at(5) == "1") || bSendFlag==2)
	{
		SC_MSG_ADD_BATCH_USERINFO_ACK SendMsg;
		SendMsg.bSucceed = true;
		if (bSendFlag == 2 || (vecStrTmpData.size()==2 && vecStrTmpData.at(1)=="0")) //有严重错误或者当前次添加一条都没有成功，就返回失败
		{
			SendMsg.bSucceed = false;
			UserInfoMgr::GetInstance()->SetStrTmpDataBySocketId(SocketId,""); //清空临时数据
		}

		if (vStrRecord.size() == 6 && vStrRecord.at(5) == "1" && vecStrTmpData.size()==2)//单次添加完毕
		{
			printf("%s  批量分批增加用户信息完毕，这次需要添加%d用户信息记录，成功添加了%s条\n", __FUNCTION__, atoi(vStrRecord.at(2).c_str()), vecStrTmpData.at(1).c_str());
		}
		if (vStrRecord.size() == 6 && vStrRecord.at(4) == "1" && vecStrTmpData.size()==2) //批量增添用户信息完毕
		{
			printf("%s  用户[%s]批量需增加%d条用户信息记录，实际成功增加了%s条记录\n", __FUNCTION__, UserInfoMgr::GetInstance()->GetAccountBySocketId(SocketId).c_str(), (int)(atoi(vStrRecord.at(3).c_str())-1)*MAXBATCHREQACKCOUNT+atoi(vStrRecord.at(2).c_str()), vecStrTmpData.at(0).c_str());
			UserInfoMgr::GetInstance()->SetStrTmpDataBySocketId(SocketId,""); //清空临时数据
		}

		if (vecStrTmpData.size()==2)
		{
			SendMsg.bSucceedRecordCount = (unsigned char)atoi(vecStrTmpData.at(1).c_str());
		}

		PackData packData = MsgPackageMgr::Pack(&SendMsg, sizeof(SendMsg), ASSIST_ID_ADD_BATCH_USERINFO_ACK);
		MsgPackageMgr::Send(SocketId, packData);
	}
}

void UserInfoProc::SelectSingleUserInfoReplyHandle(SOCKET SocketId, MYSQL_RES *MysqlRes, string strRecord)
{
	SC_MSG_SELECT_SINGLE_USERINFO_ACK SendMsg;
	SendMsg.bResCode = 0;
	do 
	{
		vector<string> vStrRecord= StringTool::Splite(strRecord, "~");
		if (vStrRecord.size() != 3)
		{
			printf("%s  数据项数量[%u] 记录内数据项数量有错strRecord[%s]\n", __FUNCTION__, vStrRecord.size(), strRecord.c_str());
			SendMsg.bResCode = 4;
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
			if (j == 8)
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
					SendMsg.bResCode = 4;
					break;
				}

				for (unsigned i=0; i<vecStrField.size(); i++)
				{
					if (StringTool::ToLowercase(vecStrField.at(i)) == StringTool::ToLowercase(string("userID")))
					{
						if (sql_row[i])
						{
							SendMsg.uUserID = (unsigned)atoi(sql_row[i]);
						}
					}
					else if (StringTool::ToLowercase(vecStrField.at(i)) == StringTool::ToLowercase(string("account")))
					{
						if (sql_row[i])
						{
							strcpy_s(SendMsg.cAccount, sizeof(SendMsg.cAccount), sql_row[i]);
						}
						else
						{
							strcpy_s(SendMsg.cAccount, sizeof(SendMsg.cAccount), "NULL");
						}
					}
					else if (StringTool::ToLowercase(vecStrField.at(i)) == StringTool::ToLowercase(string("password")))
					{
						if (sql_row[i])
						{
							strcpy_s(SendMsg.cPWD, sizeof(SendMsg.cPWD), sql_row[i]);
						}
						else
						{
							strcpy_s(SendMsg.cPWD, sizeof(SendMsg.cPWD), "NULL");
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
					else if (StringTool::ToLowercase(vecStrField.at(i)) == StringTool::ToLowercase(string("sex")))
					{
						if (sql_row[i])
						{
							SendMsg.sSex = (unsigned char)atoi(sql_row[i]);
						}
					}
					else if (StringTool::ToLowercase(vecStrField.at(i)) == StringTool::ToLowercase(string("Ident")))
					{
						if (sql_row[i])
						{
							SendMsg.sIdent = (unsigned char)atoi(sql_row[i]);
						}
					}
					else if (StringTool::ToLowercase(vecStrField.at(i)) == StringTool::ToLowercase(string("major")))
					{
						if (sql_row[i])
						{
							strcpy_s(SendMsg.cMajor, sizeof(SendMsg.cMajor), sql_row[i]);
						}
						else
						{
							strcpy_s(SendMsg.cMajor, sizeof(SendMsg.cMajor), "NULL");
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
					else
					{
						printf("%s  没有的字段项[%s]\n", __FUNCTION__, vecStrField.at(i).c_str());
						memset(&SendMsg, 0, sizeof(SendMsg));
						SendMsg.bResCode = 4;
						break;
					}
				}
				
				//根据身边标识判断是否可查 -- 每种身份都有单条查询权限，所以只能查询自己和比自己身份标识低的用户。特殊的是管理者，可以查询学生和教师的密码，教师则不能查询学生的密码，自己也不能查询自己的密码
				short sMyIdent = UserInfoMgr::GetInstance()->GetIdentBySocketId(SocketId);
				if (sMyIdent > SendMsg.sIdent)
				{
					if (sMyIdent != 3)
					{
						memset(SendMsg.cPWD, 0, sizeof(SendMsg.cPWD));
						strcpy_s(SendMsg.cPWD, sizeof(SendMsg.cPWD), "******");
					}

					if (vStrRecord.at(2) == "1")
					{
						UserInfoMgr::GetInstance()->SetStrTmpDataBySocketId(SocketId, StringTool::NumberToStr((int)SendMsg.sIdent) + "|" + SendMsg.cAccount); //保存被查询用户身份标识和账号到个人临时数据
					}
				}
				else if (sMyIdent == SendMsg.sIdent && UserInfoMgr::GetInstance()->GetAccountBySocketId(SocketId) == SendMsg.cAccount) //查询用户自己的信息
				{
					memset(SendMsg.cPWD, 0, sizeof(SendMsg.cPWD));
					strcpy_s(SendMsg.cPWD, sizeof(SendMsg.cPWD), "******");

					if (vStrRecord.at(2) == "1")
					{
						UserInfoMgr::GetInstance()->SetStrTmpDataBySocketId(SocketId, StringTool::NumberToStr((int)SendMsg.sIdent) + "|" + SendMsg.cAccount); //保存被查询用户身份标识和账号到个人临时数据
					}
				}
				else
				{
					printf("%s  用户名[%s]身份标识[%d]查询不了身份标识[%d]的单条用户信息\n", __FUNCTION__, UserInfoMgr::GetInstance()->GetAccountBySocketId(SocketId).c_str(), sMyIdent, SendMsg.sIdent);
					SendMsg.bResCode = 3;
					break;
				}
			}
			else
			{
				printf("%s  数据库返回结果错误，列数错误\n", __FUNCTION__);
				SendMsg.bResCode = 4;
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

	PackData packData = MsgPackageMgr::Pack(&SendMsg, sizeof(SendMsg), ASSIST_ID_SELECT_SINGLE_USERINFO_ACK);
	MsgPackageMgr::Send(SocketId, packData);
}

void UserInfoProc::UpdateSingleUserInfoReplyHandle(SOCKET SocketId, MYSQL_RES *MysqlRes, string strRecord)
{
	SC_MSG_UPDATE_SINGLE_USERINFO_ACK SendMsg;
	SendMsg.bSucceed = true;
	do 
	{
		vector<string> vStrRecord= StringTool::Splite(strRecord, "~");
		if (vStrRecord.size() != 4)
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

		//更改自己的用户名，处理更改数据库的，还要更改内存的
		vector<string> vecStrUpdateMySelfAccount = StringTool::Splite(vStrRecord.at(3));
		if (vecStrUpdateMySelfAccount.size()==2 && vecStrUpdateMySelfAccount.at(0) == "1")
		{
			UserInfoMgr::GetInstance()->SetNewAccountByAccount(vStrRecord.at(2).c_str(), vecStrUpdateMySelfAccount.at(1));
		}

		SendMsg.sType = (short)atoi(vStrRecord.at(1).c_str());
		strcpy_s(SendMsg.cAccount, sizeof(SendMsg.cAccount), vStrRecord.at(2).c_str());
	} while (false);

	PackData packData = MsgPackageMgr::Pack(&SendMsg, sizeof(SendMsg), ASSIST_ID_UPDATE_SINGLE_USERINFO_ACK);
	MsgPackageMgr::Send(SocketId, packData);
}
