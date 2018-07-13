
#ifndef __PROTODEF_H__
#define __PROTODEF_H__

//服务端默认端口号
#define  DefaultPort        6000

//接收数据长度
#define  DefaultRecvMSGLen  2048
//最长接收长度
#define  MaxRecvMSGLen      4098

//发送数据长度 客户端给服务端发送的数据一般不大
#define  DefaultSendMSGLen  2048

//操作权限宏 与服务端对应  登录和注册操作权限，每个玩家都拥有
enum OperPermission
{
	OPER_PER_INVALID        =    0,
	OPER_PER_START	        =    0, //有效值起始值
	OPER_PER_LOGIN          =    1, 
	OPER_PER_REGISTER       =    2,

	OPER_PER_END                    //有效值终止值
};

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
	ASSIST_ID_INVALID          =   0,
	ASSIST_ID_START            =   10000, //有效值起始值
	ASSIST_ID_LOGIN_REQ        =   10001, //登录请求
	ASSIST_ID_LOGIN_ACK        =   10002, //登录回复
	ASSIST_ID_REGISTER_REQ     =   10003, //注册请求
	ASSIST_ID_REGISTER_ACK     =   10004, //注册回复


	ASSIST_ID_END                     //有效值终止值
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

//登录请求   assist[1]
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

//登录回复   assist[2]
struct CS_MSG_LOGIN_ACK
{
	bool bSucceed;
	char cOperPer[150];

	CS_MSG_LOGIN_ACK()
	{
		memset(this, 0, sizeof(SC_MSG_LOGIN_REQ));
	}
};

//注册请求   assist[3]
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

//注册回复   assist[4]
struct CS_MSG_REGISTER_ACK
{
	bool bSucceed;
	char cOperPer[150];

	CS_MSG_REGISTER_ACK()
	{
		memset(this, 0, sizeof(CS_MSG_REGISTER_ACK));
	}
};


#endif