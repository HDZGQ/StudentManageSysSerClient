#ifndef __SELECTUSERINFOSYSPROC_H__
#define __SELECTUSERINFOSYSPROC_H__

#include "BaseProc.h"

class SelectUserInfoSysProc : public BaseProc
{
public:
	SelectUserInfoSysProc(ProcDef nProcDef);
	~SelectUserInfoSysProc();

	virtual bool initMapChoose();

	virtual void EndProc();

	virtual void StartRecv(void* vpData, unsigned int DataLen, /*int iMianId,*/ int iAssistId);
	virtual void EndRecv();

	virtual void SwitchToOper(OperPermission CurOper);


};

#endif