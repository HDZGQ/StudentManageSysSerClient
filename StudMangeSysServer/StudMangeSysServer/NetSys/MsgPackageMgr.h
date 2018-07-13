#ifndef __MSGPACKAGEMGR_H__
#define __MSGPACKAGEMGR_H__

#include <iostream>
#include "NetDef.h"
#include "ProtoDef.h"

using namespace std;

class MsgPackageMgr
{
public:
	MsgPackageMgr();
	~MsgPackageMgr();

	static UnpackData Unpack(char* OriData, unsigned int OriDataLen);
	static PackData Pack(void* vpData, unsigned int DataLen, Assist_ID iAssistId);

	static bool Send(unsigned __int64 SocketId, PackData &packData);

	static void SetStartNet(bool bIsStartNet);
private:
	static bool m_IsStartNet;
};

#endif