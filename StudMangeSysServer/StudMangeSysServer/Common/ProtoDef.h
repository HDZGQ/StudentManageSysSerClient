
#ifndef __PROTODEF_H__
#define __PROTODEF_H__

#if 0
//协议主ID
enum Main_ID
{
	MAIN_ID_INVALID          =   0,
	MAIN_ID_START            =   0, //有效值起始值
	MAIN_ID_LOGINREGISTER    =   1,
	MAIN_ID_SCORE            =   2,
	MAIN_ID_USERINFO         =   3,

	MAIN_ID_END                     //有效值终止值
};
#endif

//协议辅助ID
enum Assist_ID
{
	/*
	* 服务端自己使用
	*/
	ASSIST_ID_INVALID								 =   0,
	ASSIST_ID_SPECIAL_START                          =   0,     //特殊有效值起始值
	ASSIST_ID_SPECIAL_GET_EXIAT_SUBJECTS             =   1,     //获取存在的科目


	ASSIST_ID_SPECIAL_END                                 ,     //特殊有效值终止值


	/*------------------------------------------------------------------------------*/

	/*
	* 与客户端交互
	*/
	ASSIST_ID_START									 =   10000, //有效值起始值
	ASSIST_ID_LOGIN_REQ								 =   10001, //登录请求
	ASSIST_ID_LOGIN_ACK								 =   10002, //登录回复
	ASSIST_ID_REGISTER_REQ							 =   10003, //注册请求
	ASSIST_ID_REGISTER_ACK							 =   10004, //注册回复
	ASSIST_ID_EXIT_SYS_REQ							 =   10005, //退出系统
	ASSIST_ID_GET_CAN_ALTER_AFTER_ALTER_SUBJECTS_REQ =   10006, //增减科目前请求可增减的科目请求
	ASSIST_ID_GET_CAN_ALTER_AFTER_ALTER_SUBJECTS_ACK =   10007, //增减科目前请求可增减的科目回复
	ASSIST_ID_ALTER_SUBJECTS_REQ					 =   10008, //增减科目请求
	ASSIST_ID_ALTER_SUBJECTS_ACK					 =   10009, //增减科目回复


	ASSIST_ID_END												//有效值终止值
};


//协议头定义
struct HeadData
{
//	int MainID;
	int AssistID;
	int Len;

	HeadData()
	{
		memset(this, 0, sizeof(HeadData));
	}
};

//登录请求   assist[10001]
struct SC_MSG_LOGIN_REQ
{
	char cAccount[31];
	char cPWD[31];
//	int  OperPerId;


	SC_MSG_LOGIN_REQ()
	{
		memset(this, 0, sizeof(SC_MSG_LOGIN_REQ));
	}
};

//登录回复   assist[10002]
struct CS_MSG_LOGIN_ACK
{
	bool bSucceed;
	unsigned int iUserId;
	char cAccount[31];
	char cName[31];
	short sIdent;
	short sSex;
	char cOperPer[150];

	CS_MSG_LOGIN_ACK()
	{
		memset(this, 0, sizeof(SC_MSG_LOGIN_REQ));
	}
};

//注册请求   assist[10003]
struct SC_MSG_REGISTER_REQ
{
	char cName[31];
	char cAccount[31];
	char cPWD[31];
	char cSex[3];
	char cIdent[3];
//	int  OperPerId;

	SC_MSG_REGISTER_REQ()
	{
		memset(this, 0, sizeof(SC_MSG_REGISTER_REQ));
	}
};

//注册回复   assist[10004]
struct CS_MSG_REGISTER_ACK
{
	bool bSucceed;
	unsigned int iUserId;
	char cAccount[31];
	char cName[31];
	short sIdent;
	short sSex;
	char cOperPer[150];

	CS_MSG_REGISTER_ACK()
	{
		memset(this, 0, sizeof(CS_MSG_REGISTER_ACK));
	}
};

//退出系统   assist[10005]
struct CS_MSG_EXIT_SYS_REQ
{
	bool bExit;
	CS_MSG_EXIT_SYS_REQ()
	{
		memset(this, 0, sizeof(CS_MSG_EXIT_SYS_REQ));
	}
};

//增减科目前请求可增减的科目请求   assist[10006]
struct CS_MSG_GET_CAN_ALTER_AFTER_ALTER_SUBJECTS_REQ
{
	short sGetType; //1增加科目前获取  2删除科目前获取
	CS_MSG_GET_CAN_ALTER_AFTER_ALTER_SUBJECTS_REQ()
	{
		memset(this, 0, sizeof(CS_MSG_GET_CAN_ALTER_AFTER_ALTER_SUBJECTS_REQ));
	}
};

//增减科目前请求可增减的科目回复   assist[10007]
struct CS_MSG_GET_CAN_ALTER_AFTER_ALTER_SUBJECTS_ACK
{
	bool bSucceed;
	short sGetType; //1增加科目前获取  2删除科目前获取
	char cCanAlterSubjects[60];
	CS_MSG_GET_CAN_ALTER_AFTER_ALTER_SUBJECTS_ACK()
	{
		memset(this, 0, sizeof(CS_MSG_GET_CAN_ALTER_AFTER_ALTER_SUBJECTS_ACK));
	}
};

//增减科目请求   assist[10008]
struct CS_MSG_ALTER_SUBJECTS_REQ
{
	short sGetType; //1增加科目  2删除科目
	short sSubjectId;
	CS_MSG_ALTER_SUBJECTS_REQ()
	{
		memset(this, 0, sizeof(CS_MSG_ALTER_SUBJECTS_REQ));
	}
};

//增减科目回复   assist[10009]
struct CS_MSG_ALTER_SUBJECTS_ACK
{
	bool bSucceed;
	short sGetType;
	short sSubjectId;
	CS_MSG_ALTER_SUBJECTS_ACK()
	{
		memset(this, 0, sizeof(CS_MSG_ALTER_SUBJECTS_ACK));
	}
};

#endif