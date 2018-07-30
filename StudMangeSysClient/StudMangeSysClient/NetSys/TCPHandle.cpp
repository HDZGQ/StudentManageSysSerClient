#include "TCPHandle.h"
#include "ProcMgr.h"

#pragma comment(lib, "Ws2_32.lib")      // Socket编程需用的动态链接库


/*
* socket接收数据有三种情况:
* 1.在接收缓存大小范围内（windows的是4K），服务端发送的数据和客户端接收都在函数设置消息长度内，这种是正常接收情况
* 2.如果如果服务端发送消息很大，超过接收缓存，但是客户端recv设置的接收长度也很大，足够接收。系统会把所有的数据都接受完，recv再返回
* 3.如果服务端发送的数据很大，超过客户端recv设置的接收长度，那么recv会接收多次，即返回多次。这样子，需要客户端用户自己实现连续接收代码，把数据接受完了
*/
DWORD WINAPI RecvThread(LPVOID IpParameter)
{
#if 0
	bool bIsWaitRecv = false;
	//char MaxRecvData[MaxRecvMSGLen];
	char* MaxRecvData = new char[MaxRecvMSGLen];
	memset(MaxRecvData, 0, sizeof(MaxRecvData));

	unsigned int DataLen = 0;
	unsigned int RemainLen = 0;
	int iMainId = MAIN_ID_INVALID;
	int iAssistId = ASSIST_ID_INVALID;
#endif
	while(1)
	{
		char recvBuf[DefaultRecvMSGLen];  
		memset(recvBuf, 0, sizeof(recvBuf));
		int res =  recv(TCPHandle::GetInstance()->GetSocketID(), recvBuf, DefaultRecvMSGLen, 0); //会阻塞，要做判断检测，做服务端断线等异常处理。需要加锁？
		if (-1 == res)
		{
			DWORD dwErr = GetLastError();
			cerr << "ReceiveMessageThread recv Error: " << dwErr << endl;
			if (WSAECONNRESET == dwErr)// 远程主机关闭现有连接，一般为服务端突然关闭了
			{
				cerr << "RecvThread recv Error: An existing connection was forcibly closed by the remote hos" << endl;
				//delete MaxRecvData[];
			}
			system("pause");
			exit(0);
		}
#if 0
		//处理服务端发送数据过大，但是客户端设置接收的buffer长度过小问题。（TCP原本就有接收缓存（windows为4K）不足时，系统会多次接受直到数据接收完recv再返回，但是recv接的接收长度其实不能设置过大，并不是所有所有数据都很大）
		if (!bIsWaitRecv)
		{
			char strHead[15];
			memset(strHead, 0, sizeof(strHead));
			memset(MaxRecvData, 0, sizeof(MaxRecvData));

			memcpy(strHead, recvBuf, sizeof(HeadData));
			HeadData* pHeadData = (HeadData*)strHead;

			if (pHeadData->Len > DefaultRecvMSGLen)
				bIsWaitRecv = true;
			iMainId = pHeadData->MainID;
			iAssistId = pHeadData->AssistID;
			
			DataLen = pHeadData->Len - sizeof(HeadData);

			if (bIsWaitRecv)
			{	
				memcpy(MaxRecvData, &recvBuf[sizeof(HeadData)], DefaultRecvMSGLen-sizeof(HeadData));
				RemainLen = pHeadData->Len-DefaultRecvMSGLen;
				continue;
			}
			else
			{
				memcpy(MaxRecvData, &recvBuf[sizeof(HeadData)], DataLen);
			}
		}
		else
		{
			if (RemainLen > DefaultRecvMSGLen)
			{
				memcpy(&MaxRecvData[DataLen-RemainLen], recvBuf, DefaultRecvMSGLen);
				RemainLen -= DefaultRecvMSGLen;
				continue;
			}
			else
			{
				memcpy(&MaxRecvData[DataLen-RemainLen], recvBuf, RemainLen);
				RemainLen = 0;
				bIsWaitRecv = false;
			}
		}
#endif

		//解包。如果包过大，由服务端多次send发送（在协议里面设置是否是连续包）。不采用等待所有数据完成才处理的实现方式，避免高并发时候引起问题
		char strHead[15];
		memset(strHead, 0, sizeof(strHead));
		memcpy(strHead, recvBuf, sizeof(HeadData)); //不安全
		HeadData* pHeadData = (HeadData*)strHead;

		char strData[DefaultRecvMSGLen];
		unsigned int DataLen = 0;
		memset(strData, 0, sizeof(strData));
		if (pHeadData->Len > sizeof(HeadData))
		{
			DataLen = pHeadData->Len-sizeof(HeadData);
			memcpy(strData, &recvBuf[sizeof(HeadData)], DataLen); //不安全
		}
		else if (pHeadData->Len == sizeof(HeadData))
			cout<<"Data Len == 0"<<endl;
		else
		{
			cout<<"Data Len error"<<endl;
			continue;
		}

		//根据进程管理器，遍历一次所有进程。只要运行中的，都把消息投进去
		ProcMgr::GetInstance()->Recv(strData, DataLen, /*pHeadData->MainID,*/ pHeadData->AssistID);
#if 0
		iMainId = MAIN_ID_INVALID;
		iAssistId = ASSIST_ID_INVALID;
		DataLen = 0;
		RemainLen = 0;
		bIsWaitRecv = false;
#endif
	}

	//delete MaxRecvData[];
	return 0;
}

TCPHandle::TCPHandle()
{
	m_IsConn = false;
	m_SocketId = 0; // 连接生成的SOCKET可能为0或者负数吗？
}

TCPHandle::~TCPHandle()
{
	TCPDisConn();
}

bool TCPHandle::TCPConn()
{
	if (m_IsConn)
	{
		printf(" TCP Have Connect\n");   
		return true;
	}

	// 加载socket动态链接库(dll)  
	WORD wVersionRequested;  
	WSADATA wsaData;    // 这结构是用于接收Wjndows Socket的结构信息的  
	wVersionRequested = MAKEWORD( 2, 2 );   // 请求2.2版本的WinSock库  
	int err = WSAStartup( wVersionRequested, &wsaData );  
	if ( err != 0 ) 
	{   // 返回值为零的时候是表示成功申请WSAStartup  
		return false;  
	}  
	if ( LOBYTE( wsaData.wVersion ) != 2 || HIBYTE( wsaData.wVersion ) != 2 ) 
	{ // 检查版本号是否正确  
		WSACleanup( );  
		return false;   
	}  

	// 创建socket操作，建立流式套接字，返回套接字号m_SocketId  
	m_SocketId = socket(AF_INET, SOCK_STREAM, 0);  
	if(m_SocketId == INVALID_SOCKET) 
	{   
		printf("Error at socket():%ld\n", WSAGetLastError());   
		WSACleanup();   
		return false;   
	}   

	// 将套接字m_SocketId与远程主机相连  
	// int connect( SOCKET s,  const struct sockaddr* name,  int namelen);  
	// 第一个参数：需要进行连接操作的套接字  
	// 第二个参数：设定所需要连接的地址信息  
	// 第三个参数：地址的长度  
	SOCKADDR_IN addrSrv;  
	addrSrv.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");      // 本地回路地址是127.0.0.1;   
	addrSrv.sin_family = AF_INET;  
	addrSrv.sin_port = htons(DefaultPort);  
	while(SOCKET_ERROR == connect(m_SocketId, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR)))
	{  
		// 如果还没连接上服务器则要求重连  
		cout << "服务器连接失败，是否重新连接？（Y/N):";  
		char choice;  
		while(cin >> choice && (!((choice != 'Y' && choice == 'N') || (choice == 'Y' && choice != 'N'))))
		{  
			cout << "输入错误，请重新输入:";  
			cin.sync();  
			cin.clear();  
		}  
		if (choice == 'Y'){  
			continue;  
		}  
		else{  
			cout << "退出系统中..."<<endl;    
			return false;  
		}  
	}  
	cin.sync();  
	cout << "连接服务端成功。\n"; 

	m_IsConn = true;
	return true;
}

bool TCPHandle::TCPDisConn()
{
	if (m_IsConn)
	{
		closesocket(m_SocketId);  
		WSACleanup();   // 终止对套接字库的使用 
		m_IsConn = false;
	}

	return true;
}

SOCKET TCPHandle::GetSocketID()
{
	return m_SocketId;
}

bool TCPHandle::Send(void* vpData, unsigned int DataLen, /*int iMianId,*/ int iAssistId)
{
	if (!m_IsConn)
	{
		cout<<"客户端还没有连接服务器！"<<endl;
		return false;
	}
	//vpData可以为NULL，但是对应的DataLen必须也为0。否则错误
	if (NULL == vpData && DataLen > 0)
	{	
		cout<<"消息为空，但是消息长度大于0！"<<endl;
		return false;
	}

	HeadData hd;
	//hd.MainID = iMianId;
	hd.AssistID = iAssistId;
	hd.Len = sizeof(HeadData) + DataLen;
	
	if (hd.Len > DefaultSendMSGLen)
	{
		cout<<"发送消息过大，现在长度为："<<hd.Len<<endl;
		return false;
	}

	char SendMsg[DefaultSendMSGLen];
	memset(SendMsg, 0, sizeof(SendMsg));
	memcpy(SendMsg, &hd, sizeof(HeadData));
	if (DataLen > 0)
		memcpy(&SendMsg[sizeof(HeadData)], vpData, DataLen);

	::send(m_SocketId, SendMsg, hd.Len, 0);  // 发送信息 

	return true;
}

void TCPHandle::SetIsConn(bool bIsConn)
{
	m_IsConn = bIsConn;
}

bool TCPHandle::GetIsConn()
{
	return m_IsConn;
}