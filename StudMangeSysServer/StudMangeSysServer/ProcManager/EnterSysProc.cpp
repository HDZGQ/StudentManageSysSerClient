#include "EnterSysProc.h"
#include "MysqlMgr.h"
#include "PublicDef.h"
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
	sprintf_s(strMysql, sizeof(strMysql), "select a.userID as userID, a.account as account, b.Authority as Authority from userInfo as a, userAuthority as b where a.account='%s' and a.password='%s' and a.userID=b.userID", RecvMsg->cAccount, RecvMsg->cPWD);

	MysqlMgr::GetInstance()->InputMsgQueue(strMysql, MYSQL_OPER_SELECT, OPER_PER_LOGIN, SocketId);
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

	MysqlMgr::GetInstance()->InputMsgQueue(strMysql, MYSQL_OPER_INSERT, OPER_PER_REGISTER, SocketId);

	UserInfoMgr::GetInstance()->SetRegNeedCountBySocketId(SocketId, 1); //标记注册需要涉及的数据库操作进行一次了
	UserInfoMgr::GetInstance()->SetAccountBySocketId(SocketId, RecvMsg->cAccount); //先记录下账号
}

void EnterSysProc::LoginReplyHandle(SOCKET SocketId, MYSQL_RES *MysqlRes, int iRes)
{
	if (NULL == MysqlRes || 0 != iRes)
	{
		printf("%s  数据库语句执行失败\n", __FUNCTION__);
		return;
	}

	CS_MSG_LOGIN_ACK sendMsg;
	unsigned rowsNum = (unsigned)mysql_num_rows(MysqlRes);
	if (1 == rowsNum)
	{
		sendMsg.bSucceed = true;

		MYSQL_ROW sql_row;
		int j = mysql_num_fields(MysqlRes);
		sql_row=mysql_fetch_row(MysqlRes);
		if (3 == j)
		{
			unsigned userid = (unsigned)atoi(sql_row[0]);
			string strAccount = sql_row[1];
			string strAuthority = sql_row[2];

			UserInfoMgr::GetInstance()->SetUserIdBySocketId(SocketId, userid);
			UserInfoMgr::GetInstance()->SetAccountBySocketId(SocketId, strAccount);
			UserInfoMgr::GetInstance()->SetAuthorityBySocketId(SocketId, strAuthority);
			strcpy_s(sendMsg.cOperPer, sizeof(sendMsg.cOperPer), strAuthority.c_str());

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

void EnterSysProc::RegisterReplyHandle(SOCKET SocketId, MYSQL_RES *MysqlRes, int iRes)
{
	if (0 != iRes)
	{
		printf("%s  数据库语句执行失败\n", __FUNCTION__);
		return;
	}
	short sRegNeedCount = UserInfoMgr::GetInstance()->GetRegNeedCountBySocketId(SocketId);
	switch(sRegNeedCount)
	{
	case 1:
		{
			char strMysql[512];
			memset(strMysql, 0, sizeof(strMysql));
			sprintf_s(strMysql, sizeof(strMysql), "select userID, Ident from userInfo where account='%s'", UserInfoMgr::GetInstance()->GetAccountBySocketId(SocketId).c_str());

			MysqlMgr::GetInstance()->InputMsgQueue(strMysql, MYSQL_OPER_SELECT, OPER_PER_REGISTER, SocketId);

			UserInfoMgr::GetInstance()->SetRegNeedCountBySocketId(SocketId, 2); //标记注册需要涉及的数据库操作进行2次了
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

					MysqlMgr::GetInstance()->InputMsgQueue(strMysql, MYSQL_OPER_INSERT, OPER_PER_REGISTER, SocketId);

					UserInfoMgr::GetInstance()->SetRegNeedCountBySocketId(SocketId, 3); //标记注册需要涉及的数据库操作进行3次了
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
			CS_MSG_REGISTER_ACK sendMsg;
			sendMsg.bSucceed = true;
			strcpy_s(sendMsg.cOperPer, sizeof(sendMsg.cOperPer), UserInfoMgr::GetInstance()->GetAuthorityBySocketId(SocketId).c_str());

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