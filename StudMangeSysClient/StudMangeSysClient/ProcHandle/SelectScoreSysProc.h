#ifndef __SELECTSCORESYSPROC_H__
#define __SELECTSCORESYSPROC_H__

#include "BaseProc.h"

class SelectScoreSysProc : public BaseProc
{
public:
	SelectScoreSysProc(ProcDef nProcDef);
	~SelectScoreSysProc();

	virtual bool initMapChoose();

	virtual void EndProc();

	virtual void StartRecv(void* vpData, unsigned int DataLen, /*int iMianId,*/ int iAssistId);
	virtual void EndRecv();

	virtual void SwitchToOper(OperPermission CurOper);


};

#endif