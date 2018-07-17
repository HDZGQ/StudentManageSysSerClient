#ifndef __ADDSCORESYSPROC_H__
#define __ADDSCORESYSPROC_H__

#include "BaseProc.h"

class AddScoreSysProc : public BaseProc
{
public:
	AddScoreSysProc(ProcDef nProcDef);
	~AddScoreSysProc();

	virtual bool initMapChoose();

	virtual void EndProc();

	virtual void StartRecv(void* vpData, unsigned int DataLen, /*int iMianId,*/ int iAssistId);
	virtual void EndRecv();

	virtual void SwitchToOper(OperPermission CurOper);


};

#endif