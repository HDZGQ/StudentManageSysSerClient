#ifndef __NETSYSMGR_H__
#define __NETSYSMGR_H__

#include "NetDef.h"
#include <Windows.h> 
#include "xSingleton.h"

//工作线程
DWORD WINAPI ServerWorkThread(LPVOID CompletionPortID);

class NetSysMgr : public xSingleton<NetSysMgr>
{
public:
	NetSysMgr();
	~NetSysMgr();

	void StartNet();

private:
	bool m_IsStart;
};

#endif