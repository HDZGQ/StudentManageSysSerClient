#ifndef __APPRUN_H__
#define __APPRUN_H__

#include "TCPHandle.h"
#include "ProcMgr.h"

class APPRun : public xSingleton<APPRun>
{
public:
	APPRun();
	~APPRun();

	void Run();
};

#endif