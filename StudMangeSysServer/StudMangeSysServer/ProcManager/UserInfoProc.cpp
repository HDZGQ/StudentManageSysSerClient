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

void UserInfoProc::AddSingleUserInfoReplyHandle(SOCKET SocketId, MYSQL_RES *MysqlRes, string strRecord)
{
	CS_MSG_ADD_SINGLE_USERINFO_ACK SendMsg;
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