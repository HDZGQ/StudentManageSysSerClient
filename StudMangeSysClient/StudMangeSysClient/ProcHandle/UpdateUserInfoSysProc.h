#ifndef __UPDATEUSERINFOSYSPROC_H__
#define __UPDATEUSERINFOSYSPROC_H__

#include "BaseProc.h"

class UpdateUserInfoSysProc : public BaseProc
{
public:
	UpdateUserInfoSysProc(ProcDef nProcDef);
	~UpdateUserInfoSysProc();

	virtual bool initMapChoose();

	virtual void EndProc();

	virtual void StartRecv(void* vpData, unsigned int DataLen, /*int iMianId,*/ int iAssistId);
	virtual void EndRecv();

	virtual void SwitchToOper(OperPermission CurOper);


};

#endif