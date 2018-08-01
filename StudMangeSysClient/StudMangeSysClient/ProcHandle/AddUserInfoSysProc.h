#ifndef __ADDUSERINFOSYSPROC_H__
#define __ADDUSERINFOSYSPROC_H__

#include "BaseProc.h"

class AddUserInfoSysProc : public BaseProc
{
public:
	AddUserInfoSysProc(ProcDef nProcDef);
	~AddUserInfoSysProc();

	virtual bool initMapChoose();

	virtual void EndProc();

	virtual void StartRecv(void* vpData, unsigned int DataLen, /*int iMianId,*/ int iAssistId);
	virtual void EndRecv();

	virtual void SwitchToOper(OperPermission CurOper);

	//具体功能选择操作处理
	void AddSingleUserInfoChooseHandle(); //单条增加用户信息请求

	//返回结果处理
	bool AddSingleUserInfoRecvHandle(void* vpData, unsigned int DataLen);//单条增加用户信息回复
};

#endif