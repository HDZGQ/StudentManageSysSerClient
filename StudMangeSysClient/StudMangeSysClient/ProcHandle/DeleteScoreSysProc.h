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
	
	//选择操作处理
	void DeleteScoreChooseHandle();

	//返回结果处理
	bool DeleteScoreRecvHandle(void* vpData, unsigned int DataLen);

};

#endif