#include "EnterSysProc.h"
#include "MysqlMgr.h"
//#include "PublicDef.h"
#include "NetDef.h"
#include "UserInfoMgr.h"
#include "MsgPackageMgr.h"
#include "StringTool.h"

EnterSysProc::EnterSysProc()
{

}

EnterSysProc::~EnterSysProc()
{

}

void EnterSysProc::LoginRecvHandle(SOCKET SocketId, void* vpData, unsigned int DataLen)
{
	if ( NULL == vpData)
	{
		printf("%s  消息为空\n", __FUNCTION__);
		return;
	}
	if (DataLen != sizeof(SC_MSG_LOGIN_REQ))
	{
		printf("%s  长度DataLen[%u]不对，正确长度[%u]\n", __FUNCTION__, DataLen, sizeof(SC_MSG_LOGIN_REQ));
		return;
	}
	
	SC_MSG_LOGIN_REQ* RecvMsg = (SC_MSG_LOGIN_REQ*)vpData;

	char strMysql[512];
	memset(strMysql, 0, sizeof(strMysql));
	sprintf_s(strMysql, sizeof(strMysql), "select a.userID as userID, a.account as account, a.Ident as Ident, b.Authority as Authority, a.name as name, a.sex as sex  from userInfo as a, userAuthority as b where a.account='%s' and a.password='%s' and a.userID=b.userID", RecvMsg->cAccount, RecvMsg->cPWD);

	MysqlMgr::GetInstance()->InputMsgQueue(strMysql, MYSQL_OPER_SELECT, ASSIST_ID_LOGIN_ACK, SocketId);
}

void EnterSysProc::RegisterRecvHandle(SOCKET SocketId, void* vpData, unsigned int DataLen)
{
	if ( NULL == vpData)
	{
		printf("%s  消息为空\n", __FUNCTION__);
		return;
	}
	if (DataLen != sizeof(SC_MSG_REGISTER_REQ))
	{
		printf("%s  长度DataLen[%u]不对，正确长度[%u]\n", __FUNCTION__, DataLen, sizeof(SC_MSG_REGISTER_REQ));
		return;
	}
	
	SC_MSG_REGISTER_REQ* RecvMsg = (SC_MSG_REGISTER_REQ*)vpData;

	vector<OperPermission> vecOper;
	UserInfoMgr::GetInstance()->GetDefaultAuthorityByIdent((IdentType)atoi(RecvMsg->cIdent), vecOper);
	string strAuthority = StringTool::CombVecToStr(vecOper);

	char strOutUserid[61];
	memset(strOutUserid, 0, sizeof(strOutUserid));
	sprintf_s(strOutUserid, sizeof(strOutUserid), "@userid_by_scoket_%llu", (unsigned __int64)SocketId);

	char strMysql[512];
	memset(strMysql, 0, sizeof(strMysql));
	sprintf_s(strMysql, sizeof(strMysql), "call UserRegister('%s', '%s', '%s', %d, %d, '%s', %s)", RecvMsg->cAccount, RecvMsg->cName, RecvMsg->cPWD, (int)atoi(RecvMsg->cSex), (int)atoi(RecvMsg->cIdent), strAuthority.c_str(), strOutUserid);

	UserInfoMgr::GetInstance()->SetRegNeedCountBySocketId(SocketId, 0); 

	string strRecord = "~";
	strRecord += string(RecvMsg->cAccount) + "~" + RecvMsg->cName + "~" + RecvMsg->cIdent +  "~" + RecvMsg->cSex +  "~" + strAuthority;

	MysqlMgr::GetInstance()->InputMsgQueue(strMysql, MYSQL_OPER_CALL_PROC, ASSIST_ID_REGISTER_ACK, SocketId, strRecord+"~1");

	memset(strMysql, 0, sizeof(strMysql));
	sprintf_s(strMysql, sizeof(strMysql), "select %s", strOutUserid);
	MysqlMgr::GetInstance()->InputMsgQueue(strMysql, MYSQL_OPER_SELECT, ASSIST_ID_REGISTER_ACK, SocketId, strRecord+"~2");
}

// void EnterSysProc::ExitSysRecvHandle(SOCKET SocketId, void* vpData, unsigned int DataLen)
// {
// 	if ( NULL == vpData)
// 	{
// 		printf("%s  消息为空\n", __FUNCTION__);
// 		return;
// 	}
// 	if (DataLen != sizeof(CS_MSG_EXIT_SYS_REQ))
// 	{
// 		printf("%s  长度DataLen[%u]不对，正确长度[%u]\n", __FUNCTION__, DataLen, sizeof(CS_MSG_EXIT_SYS_REQ));
// 		return;
// 	}
// 
// 	CS_MSG_EXIT_SYS_REQ* RecvMsg = (CS_MSG_EXIT_SYS_REQ*)vpData;
// 	if (RecvMsg->bExit)
// 	{
// 		UserInfoMgr::GetInstance()->RemoveInfoBySocketId(SocketId);
// 	}
// }

void EnterSysProc::LoginReplyHandle(SOCKET SocketId, MYSQL_RES *MysqlRes, string strRecord)
{
	vector<string> vStrRecord= StringTool::Splite(strRecord);
	bool bExecute = true;
	if (NULL == MysqlRes || (vStrRecord.size() > 0 && (int)atoi(vStrRecord.at(0).c_str()) != 0))
	{
		printf("%s  数据库语句执行失败\n", __FUNCTION__);
		bExecute = false;
	}

	CS_MSG_LOGIN_ACK sendMsg;
	unsigned rowsNum = (unsigned)mysql_num_rows(MysqlRes);
	if (1 == rowsNum && bExecute)
	{
		sendMsg.bSucceed = true;

		MYSQL_ROW sql_row;
		int j = mysql_num_fields(MysqlRes);
		sql_row=mysql_fetch_row(MysqlRes);
		if (6 == j)
		{
			unsigned userid = (unsigned)atoi(sql_row[0]);
			string strAccount = sql_row[1];
			short Ident = (short)atoi(sql_row[2]);
			string strAuthority = sql_row[3];
			string sName = sql_row[4];
			short sSex = (short)atoi(sql_row[5]);

			UserInfoMgr::GetInstance()->SetUserIdBySocketId(SocketId, userid);
			UserInfoMgr::GetInstance()->SetAccountBySocketId(SocketId, strAccount);
			UserInfoMgr::GetInstance()->SetIdentBySocketId(SocketId, Ident);
			UserInfoMgr::GetInstance()->SetAuthorityBySocketId(SocketId, strAuthority);

			strcpy_s(sendMsg.cName, sizeof(sendMsg.cName), sName.c_str());
			strcpy_s(sendMsg.cAccount, sizeof(sendMsg.cAccount), strAccount.c_str());
			strcpy_s(sendMsg.cOperPer, sizeof(sendMsg.cOperPer), strAuthority.c_str());
			sendMsg.iUserId = userid;
			sendMsg.sIdent = Ident;
			sendMsg.sSex = sSex;

			printf("%s  userid[%u]登录系统！\n", __FUNCTION__, userid);
		}
		else
		{
			sendMsg.bSucceed = false;
		}
	}
	else
	{
		sendMsg.bSucceed = false;
	}

	PackData packData = MsgPackageMgr::Pack(&sendMsg, sizeof(sendMsg), ASSIST_ID_LOGIN_ACK);
	MsgPackageMgr::Send(SocketId, packData);
}

void EnterSysProc::RegisterReplyHandle(SOCKET SocketId, MYSQL_RES *MysqlRes, string strRecord)
{
	CS_MSG_REGISTER_ACK SendMsg;
	unsigned char bSendFlag = 0;
	short sRegNeed = UserInfoMgr::GetInstance()->GetRegNeedCountBySocketId(SocketId);
	vector<string> vStrRecord= StringTool::Splite(strRecord, "~");
	do 
	{
		if (vStrRecord.size() != 7)
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
		if (sRegNeed==0 && vStrRecord.at(6)=="1") //第一步调用存储过程
		{
			bSendFlag = 1;
			UserInfoMgr::GetInstance()->SetRegNeedCountBySocketId(SocketId, sRegNeed+(short)atoi(vStrRecord.at(6).c_str()));
			break;
		}
		else if (sRegNeed==1 && vStrRecord.at(6)=="2") //调用成功，取出userid
		{
			MYSQL_ROW sql_row;
			int j = mysql_num_fields(MysqlRes);
			sql_row=mysql_fetch_row(MysqlRes);
			if (1 == j)
			{
				unsigned userid = (unsigned)atoi(sql_row[0]);

				if(userid < 1)
				{
					printf("%s  取到的userid[%u]异常\n", __FUNCTION__, userid);
					bSendFlag = 2;
					break;
				}

				SendMsg.iUserId = userid;
				SendMsg.sIdent = (short)atoi(vStrRecord.at(3).c_str());
				SendMsg.sSex = (short)atoi(vStrRecord.at(4).c_str());
				strcpy_s(SendMsg.cAccount, sizeof(SendMsg.cAccount), vStrRecord.at(1).c_str());
				strcpy_s(SendMsg.cName, sizeof(SendMsg.cName), vStrRecord.at(2).c_str());
				strcpy_s(SendMsg.cOperPer, sizeof(SendMsg.cOperPer), vStrRecord.at(5).c_str());

				UserInfoMgr::GetInstance()->SetUserIdBySocketId(SocketId, userid); //记下用户id
				UserInfoMgr::GetInstance()->SetAccountBySocketId(SocketId, SendMsg.cAccount); //记下账号
				UserInfoMgr::GetInstance()->SetIdentBySocketId(SocketId, SendMsg.sIdent); //记下用户身份标识
				UserInfoMgr::GetInstance()->SetAuthorityBySocketId(SocketId, SendMsg.cOperPer); //记下权限
			}
			else
			{
				printf("%s  数据返回字段数不对\n", __FUNCTION__);
				bSendFlag = 2;
			}
			break;
		}
		else
		{
			printf("%s  数据库执行有误，错误原因：%s\n", __FUNCTION__, (vStrRecord.at(6)=="1"?"执行注册存储过程有误":(vStrRecord.at(6)=="2"?"取userid语句有误":"未知错误")));
			bSendFlag = 2;
			break;
		}

	} while(false);

	if (bSendFlag==0 || bSendFlag==2)
	{
		UserInfoMgr::GetInstance()->SetRegNeedCountBySocketId(SocketId, 0); 

		if (bSendFlag == 0)
		{
			SendMsg.bSucceed = true;
		}
		else
		{
			SendMsg.bSucceed = false;
		}
		PackData packData = MsgPackageMgr::Pack(&SendMsg, sizeof(SendMsg), ASSIST_ID_REGISTER_ACK);
		MsgPackageMgr::Send(SocketId, packData);
	}
}