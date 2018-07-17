#ifndef __UPDATESCORESYSPROC_H__
#define __UPDATESCORESYSPROC_H__

#include "BaseProc.h"

class UpdateScoreSysProc : public BaseProc
{
public:
	UpdateScoreSysProc(ProcDef nProcDef);
	~UpdateScoreSysProc();

	virtual bool initMapChoose();

	virtual void EndProc();

	virtual void StartRecv(void* vpData, unsigned int DataLen, /*int iMianId,*/ int iAssistId);
	virtual void EndRecv();

	virtual void SwitchToOper(OperPermission CurOper);


};

#endif