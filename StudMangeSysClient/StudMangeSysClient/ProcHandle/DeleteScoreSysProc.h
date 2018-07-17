#ifndef __DELETESCORESYSPROC_H__
#define __DELETESCORESYSPROC_H__

#include "BaseProc.h"

class DeleteScoreSysProc : public BaseProc
{
public:
	DeleteScoreSysProc(ProcDef nProcDef);
	~DeleteScoreSysProc();

	virtual bool initMapChoose();

	virtual void EndProc();

	virtual void StartRecv(void* vpData, unsigned int DataLen, /*int iMianId,*/ int iAssistId);
	virtual void EndRecv();

	virtual void SwitchToOper(OperPermission CurOper);


};

#endif