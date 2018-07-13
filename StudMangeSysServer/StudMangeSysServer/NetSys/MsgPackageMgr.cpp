#include "MsgPackageMgr.h"
#include "ProtoDef.h"

bool MsgPackageMgr::m_IsStartNet = false;

MsgPackageMgr::MsgPackageMgr()
{
	
}	

MsgPackageMgr::~MsgPackageMgr()
{

}

UnpackData MsgPackageMgr::Unpack( char* OriData, unsigned int OriDataLen)
{
	//注意包过大的情况，虽然客户端包一般不大。设置接收函数在tcp缓存取数据，可能还得有连续收包处理，或者客户端超过约定的最大包，就多次发送

	UnpackData unpackData;
	if (NULL == OriData ) //有包头，一定不能为空
	{
		printf("接收的数据包为空\n");
		return unpackData;
	}
	if (0 == OriDataLen || OriDataLen > DefaultRecvMSGLen)
	{
		printf("接收的数据包长度错误，OriDataLen[%u]\n", OriDataLen);
		return unpackData;
	}

	char strHead[15];
	memset(strHead, 0, sizeof(strHead));
	memcpy(strHead, OriData, sizeof(HeadData)); //不安全
	HeadData* pHeadData = (HeadData*)strHead;

	if (pHeadData->Len > sizeof(HeadData))
	{
		unpackData.AssistID = pHeadData->AssistID;
		unpackData.DataLen = pHeadData->Len-sizeof(HeadData);
		memcpy(unpackData.MsgData, &OriData[sizeof(HeadData)], unpackData.DataLen); //不安全
	}
	else if (pHeadData->Len == sizeof(HeadData))
	{
		unpackData.AssistID = pHeadData->AssistID;
		cout<<"Data Len == 0"<<endl;
	}
	else
	{
		cout<<"Data Len error"<<endl;
	}

	return unpackData;
}

PackData MsgPackageMgr::Pack(void* vpData, unsigned int DataLen, Assist_ID iAssistId)
{
	PackData packData;
	if (NULL == vpData && 0 != DataLen)
	{
		printf("数据消息为空，但是长度不为0 DataLen[%u]\n", DataLen);
		return packData;
	}
	if (iAssistId <= ASSIST_ID_START || iAssistId >= ASSIST_ID_END)
	{
		printf("iAssistId[%d] error\n", iAssistId);
		return packData;
	}

	HeadData hd;
	hd.AssistID = iAssistId;
	hd.Len = sizeof(HeadData) + DataLen;

	if (hd.Len > DefaultSendMSGLen)
	{
		cout<<"发送消息过大，现在长度为："<<hd.Len<<endl;
		return packData;
	}

	packData.FullDataLen = hd.Len;
	memcpy(packData.FullMsgData, &hd, sizeof(HeadData));
	if (DataLen > 0)
		memcpy(&packData.FullMsgData[sizeof(HeadData)], vpData, DataLen);

	return packData;
}

bool MsgPackageMgr::Send(unsigned __int64 SocketId, PackData &packData)
{
	bool bRes = true;

	do 
	{
		if (!m_IsStartNet)
		{
			printf("网络没有开启\n");
			bRes = false;
			break;
		}

		if (packData.FullDataLen > 0)
		{
			::send((SOCKET)SocketId, packData.FullMsgData, packData.FullDataLen, 0);  // 发送信息
			bRes = true;
		}
		else
		{
			printf("发送失败，数据长度为0\n");
			bRes = false;
		}
	} while (false);

	return bRes;
}

void MsgPackageMgr::SetStartNet(bool bIsStartNet)
{
	m_IsStartNet = bIsStartNet;
}