#ifndef __SCORESYSPROC_H__
#define __SCORESYSPROC_H__

#include "BaseProc.h"

class ScoreSysProc : public BaseProc
{
public:
	ScoreSysProc(ProcDef nProcDef);
	~ScoreSysProc();

	virtual bool initMapChoose();

	virtual void EndProc();

	virtual void StartRecv(void* vpData, unsigned int DataLen, /*int iMianId,*/ int iAssistId);
	virtual void EndRecv();

	virtual void SwitchToOper(OperPermission CurOper);


};


#endif