#ifndef __ALTERSUBJECTSSYSPROC_H__
#define __ALTERSUBJECTSSYSPROC_H__

#include "BaseProc.h"

class AlterSubjectsSysProc : public BaseProc
{
public:
	AlterSubjectsSysProc(ProcDef nProcDef);
	~AlterSubjectsSysProc();

	virtual bool initMapChoose();

	virtual void EndProc();

	virtual void StartRecv(void* vpData, unsigned int DataLen, /*int iMianId,*/ int iAssistId);
	virtual void EndRecv();

	virtual void SwitchToOper(OperPermission CurOper);


};

#endif