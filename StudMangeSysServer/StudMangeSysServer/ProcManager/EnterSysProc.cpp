#include "EnterSysProc.h"
#include "MysqlMgr.h"
//#include "PublicDef.h"
#include "NetDef.h"
#include "UserInfoMgr.h"
#include "MsgPackageMgr.h"
#include "AuthorityMgr.h"
#include "CheckTool.h"

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

	char strMysql[512];
	memset(strMysql, 0, sizeof(strMysql));
	sprintf_s(strMysql, sizeof(strMysql), "insert into userInfo(account, name, password, sex, Ident) values('%s', '%s', '%s', %s, %s)", RecvMsg->cAccount, RecvMsg->cName, RecvMsg->cPWD, RecvMsg->cSex, RecvMsg->cIdent);

	UserInfoMgr::GetInstance()->SetRegNeedCountBySocketId(SocketId, 1); //标记注册需要涉及的数据库操作进行一次了
	UserInfoMgr::GetInstance()->SetAccountBySocketId(SocketId, RecvMsg->cAccount); //先记录下账号
	UserInfoMgr::GetInstance()->SetIdentBySocketId(SocketId, (short)atoi(RecvMsg->cIdent)); //先记下用户身份标识

	MysqlMgr::GetInstance()->InputMsgQueue(strMysql, MYSQL_OPER_INSERT, ASSIST_ID_REGISTER_ACK, SocketId);
}

void EnterSysProc::ExitSysRecvHandle(SOCKET SocketId, void* vpData, unsigned int DataLen)
{
	if ( NULL == vpData)
	{
		printf("%s  消息为空\n", __FUNCTION__);
		return;
	}
	if (DataLen != sizeof(CS_MSG_EXIT_SYS_REQ))
	{
		printf("%s  长度DataLen[%u]不对，正确长度[%u]\n", __FUNCTION__, DataLen, sizeof(CS_MSG_EXIT_SYS_REQ));
		return;
	}

	CS_MSG_EXIT_SYS_REQ* RecvMsg = (CS_MSG_EXIT_SYS_REQ*)vpData;
	if (RecvMsg->bExit)
	{
		UserInfoMgr::GetInstance()->RemoveInfoBySocketId(SocketId);
	}
}

void EnterSysProc::LoginReplyHandle(SOCKET SocketId, MYSQL_RES *MysqlRes, string strRecord)
{
	vector<string> vStrRecord= CheckTool::Splite(strRecord);
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
	vector<string> vStrRecord= CheckTool::Splite(strRecord);
	short sRegNeedCount = UserInfoMgr::GetInstance()->GetRegNeedCountBySocketId(SocketId);
	if (vStrRecord.size() > 0 && (int)atoi(vStrRecord.at(0).c_str()) != 0)
	{
		printf("%s  数据库语句执行失败\n", __FUNCTION__);
		sRegNeedCount = -1;
	}
	switch(sRegNeedCount)
	{
	case 1:
		{
			char strMysql[512];
			memset(strMysql, 0, sizeof(strMysql));
			sprintf_s(strMysql, sizeof(strMysql), "select userID, Ident from userInfo where account='%s'", UserInfoMgr::GetInstance()->GetAccountBySocketId(SocketId).c_str());

			UserInfoMgr::GetInstance()->SetRegNeedCountBySocketId(SocketId, 2); //标记注册需要涉及的数据库操作进行2次了

			MysqlMgr::GetInstance()->InputMsgQueue(strMysql, MYSQL_OPER_SELECT, ASSIST_ID_REGISTER_ACK, SocketId);
			break;
		}
	case 2:
		{
			unsigned rowsNum = (unsigned)mysql_num_rows(MysqlRes);
			if (1 == rowsNum)
			{
				MYSQL_ROW sql_row;
				int j = mysql_num_fields(MysqlRes);
				sql_row=mysql_fetch_row(MysqlRes);
				if (2 == j)
				{
					unsigned userid = (unsigned)atoi(sql_row[0]);
					int Ident = (int)atoi(sql_row[1]);

					UserInfoMgr::GetInstance()->SetUserIdBySocketId(SocketId, userid);

					//根据身份使用默认权限
					vector<OperPermission> vecOper;
					AuthorityMgr::GetDefaultAuthorityByIdent((IdentType)Ident, vecOper);
					string strAuthority = CheckTool::CombVecToStr(vecOper);
					UserInfoMgr::GetInstance()->SetAuthorityBySocketId(SocketId, strAuthority);

					char strMysql[512];
					memset(strMysql, 0, sizeof(strMysql));
					sprintf_s(strMysql, sizeof(strMysql), "insert into userAuthority(userID, Authority) values(%u, '%s')", userid, strAuthority.c_str());

					UserInfoMgr::GetInstance()->SetRegNeedCountBySocketId(SocketId, 3); //标记注册需要涉及的数据库操作进行3次了

					MysqlMgr::GetInstance()->InputMsgQueue(strMysql, MYSQL_OPER_INSERT, ASSIST_ID_REGISTER_ACK, SocketId);
				}
				else
				{
					printf("%s  数据库结果有误，返回结果字段数不等于2  j[%d]\n", __FUNCTION__, j);
				}
			}
			else
			{
				printf("%s  数据库结果有误，返回记录数不等于1  rowsNum[%u]\n", __FUNCTION__, rowsNum);
			}

			break;
		}
	case 3:
		{
			char strMysql[512];
			memset(strMysql, 0, sizeof(strMysql));
			sprintf_s(strMysql, sizeof(strMysql), "select account, Ident, name, sex from userInfo where userID=%u", UserInfoMgr::GetInstance()->GetUserIdBySocketId(SocketId));

			UserInfoMgr::GetInstance()->SetRegNeedCountBySocketId(SocketId, 4); //标记注册需要涉及的数据库操作进行4次了

			MysqlMgr::GetInstance()->InputMsgQueue(strMysql, MYSQL_OPER_SELECT, ASSIST_ID_REGISTER_ACK, SocketId);
			break;
		}
	case 4:
		{
			CS_MSG_REGISTER_ACK sendMsg;
			unsigned rowsNum = (unsigned)mysql_num_rows(MysqlRes);
			if (1 == rowsNum)
			{
				sendMsg.bSucceed = true;

				MYSQL_ROW sql_row;
				int j = mysql_num_fields(MysqlRes);
				sql_row=mysql_fetch_row(MysqlRes);
				if (4 == j)
				{
					string strAccount = sql_row[0];
					short Ident = (short)atoi(sql_row[1]);
					string sName = sql_row[2];
					short sSex = (short)atoi(sql_row[3]);

					strcpy_s(sendMsg.cOperPer, sizeof(sendMsg.cOperPer), UserInfoMgr::GetInstance()->GetAuthorityBySocketId(SocketId).c_str());
					strcpy_s(sendMsg.cName, sizeof(sendMsg.cName), sName.c_str());
					strcpy_s(sendMsg.cAccount, sizeof(sendMsg.cAccount), strAccount.c_str());
					sendMsg.iUserId = UserInfoMgr::GetInstance()->GetUserIdBySocketId(SocketId);
					sendMsg.sIdent = Ident;
					sendMsg.sSex = sSex;
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

			PackData packData = MsgPackageMgr::Pack(&sendMsg, sizeof(sendMsg), ASSIST_ID_REGISTER_ACK);
			MsgPackageMgr::Send(SocketId, packData);

			printf("%s  userid[%u]注册并登录系统成功！\n", __FUNCTION__, UserInfoMgr::GetInstance()->GetUserIdBySocketId(SocketId));

			break;
		}
	default:
		{
			CS_MSG_REGISTER_ACK sendMsg;
			sendMsg.bSucceed = false;

			PackData packData = MsgPackageMgr::Pack(&sendMsg, sizeof(sendMsg), ASSIST_ID_REGISTER_ACK);
			MsgPackageMgr::Send(SocketId, packData);
			
			UserInfoMgr::GetInstance()->SetRegNeedCountBySocketId(SocketId, 0); //初始化
			printf("%s  用户注册错误 SocketId[%ld] userid[%u]\n", __FUNCTION__, SocketId, UserInfoMgr::GetInstance()->GetUserIdBySocketId(SocketId));
			break;
		}
	}
}