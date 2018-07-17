#ifndef __AUTHORITYSYSPROC_H__
#define __AUTHORITYSYSPROC_H__

#include "BaseProc.h"

class AuthoritySysProc : public BaseProc
{
public:
	AuthoritySysProc(ProcDef nProcDef);
	~AuthoritySysProc();

	virtual bool initMapChoose();

	virtual void EndProc();

	virtual void StartRecv(void* vpData, unsigned int DataLen, /*int iMianId,*/ int iAssistId);
	virtual void EndRecv();

	virtual void SwitchToOper(OperPermission CurOper);


};

#endif