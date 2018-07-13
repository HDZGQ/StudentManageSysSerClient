#ifndef __SERVERMGR_H__
#define __SERVERMGR_H__

#include "xSingleton.h"

class ServerMgr : public xSingleton<ServerMgr>
{
public:
	ServerMgr();
	~ServerMgr();
	
	void ServerRun();

};



#endif