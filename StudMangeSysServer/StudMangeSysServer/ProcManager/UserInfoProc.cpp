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
			CS_MSG_ADD_BATCH_USERINFO_ACK SendMsg;
			SendMsg.bSucceed = false;
			PackData packData = MsgPackageMgr::Pack(&SendMsg, sizeof(SendMsg), ASSIST_ID_ADD_BATCH_USERINFO_ACK);
			MsgPackageMgr::Send(SocketId, packData);

			printf("%s  保存的临时数据不正确\n", __FUNCTION__);
			return;
		}
		string newStrTMpData = vecStr.at(0) + "|0";
		UserInfoMgr::GetInstance()->SetStrTmpDataBySocketId(SocketId, newStrTMpData); //记录成功总次数和当次成功次数
	}

	for (unsigned char i=0; i < RecvMsg->bRecordCount; i++)
	{
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
		CS_MSG_ADD_BATCH_USERINFO_ACK SendMsg;
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