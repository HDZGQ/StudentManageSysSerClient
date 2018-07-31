#ifndef __NETDEF_H__
#define __NETDEF_H__

//#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>  
#include <string>
using namespace std;

//服务端默认端口号
#define  DefaultPort        6000

//接收数据长度 客户端给服务端发送的数据一般不大
#define  DefaultRecvMSGLen  2048

//发送数据长度
#define  DefaultSendMSGLen  2048

//接收消息队列最大数量
#define MaxRecvMsgCount     50

/**
* 结构体名称：PER_IO_DATA
* 结构体功能：重叠I/O需要用到的结构体，临时记录IO数据
**/
//const int DataBuffSize = DefaultRecvMSGLen;
typedef struct
{
	OVERLAPPED overlapped;
	WSABUF databuff;
	char buffer[DefaultRecvMSGLen];
	int BufferLen;
	int operationType;
}PER_IO_OPERATEION_DATA, *LPPER_IO_OPERATION_DATA, *LPPER_IO_DATA, PER_IO_DATA;


/**
* 结构体名称：PER_HANDLE_DATA
* 结构体存储：记录单个套接字的数据，包括了套接字的变量及套接字的对应的客户端的地址。
* 结构体作用：当服务器连接上客户端时，信息存储到该结构体中，知道客户端的地址以便于回访。
**/
typedef struct
{
	SOCKET socket;
	SOCKADDR_STORAGE ClientAddr;
}PER_HANDLE_DATA, *LPPER_HANDLE_DATA;

//接收消息队列结构体
typedef struct 
{
	unsigned __int64 SocketId;
	char RecvMsg[DefaultRecvMSGLen];
	unsigned int DataLen;
	int AssistID;
} RecvMsgData, *PtrRecvMsgData;


struct UnpackData
{
	char MsgData[DefaultRecvMSGLen];
	unsigned DataLen;
	int AssistID;

	UnpackData()
	{
		memset(this, 0, sizeof(UnpackData));
	}
};

struct PackData
{
	char FullMsgData[DefaultSendMSGLen];
	unsigned FullDataLen;

	PackData()
	{
		memset(this, 0, sizeof(PackData));
	}
};

struct UserInfo
{
	LPPER_IO_OPERATION_DATA PerIoData;
	PER_HANDLE_DATA* PerHandleData;

	//用户常记录
	unsigned int iUserId;
	string strAccount;
	string strIP;
	unsigned short sPort;
	string strAuthority;
	short sIdent;


	//用户动态数据记录
	short sRegNeedCount;  //
	string strTmpData; //临时数据

	UserInfo()
	{
		PerIoData = NULL;
		PerHandleData = NULL;

		iUserId = 0;
		strAccount = "";
		strIP = "";
		sPort = 0;
		strAuthority = "";
		sIdent = 1;


		sRegNeedCount = 0;
		strTmpData = "";
	}
};


#endif