#ifndef __USERINFOSYSPROC_H__
#define __USERINFOSYSPROC_H__

#include "BaseProc.h"

class UserInfoSysProc : public BaseProc
{
public:
	UserInfoSysProc(ProcDef nProcDef);
	~UserInfoSysProc();

	virtual bool initMapChoose();

	virtual void EndProc();

	virtual void StartRecv(void* vpData, unsigned int DataLen, /*int iMianId,*/ int iAssistId);
	virtual void EndRecv();

	virtual void SwitchToOper(OperPermission CurOper);


};

#endif