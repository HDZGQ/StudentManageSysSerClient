#ifndef __COMMONSYSPROC_H__
#define __COMMONSYSPROC_H__

#include "BaseProc.h"

class CommonSysProc  : public BaseProc
{
public:
	CommonSysProc(ProcDef nProcDef);
	~CommonSysProc();

	virtual bool initMapChoose();

	virtual void EndProc();

	virtual void StartRecv(void* vpData, unsigned int DataLen, /*int iMianId,*/ int iAssistId);
	virtual void EndRecv();

	virtual void SwitchToOper(OperPermission CurOper);

protected:
	void ExitSysHandle(); //通知服务端该用户退出系统
};

#endif