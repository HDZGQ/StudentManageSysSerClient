#ifndef __SELECTUSERINFOSYSPROC_H__
#define __SELECTUSERINFOSYSPROC_H__

#include <vector>
#include "BaseProc.h"

class SelectUserInfoSysProc : public BaseProc
{
public:
	SelectUserInfoSysProc(ProcDef nProcDef);
	~SelectUserInfoSysProc();

	virtual bool initMapChoose();

	virtual void EndProc();

	virtual void StartRecv(void* vpData, unsigned int DataLen, /*int iMianId,*/ int iAssistId);
	virtual void EndRecv();

	virtual void SwitchToOper(OperPermission CurOper);

	//具体功能选择操作处理
	void SelectSingleUserInfoChooseHandle(); //单条查询用户信息请求
	void SelectBatchUserInfoChooseHandle(); //批量查询用户信息请求

	//返回结果处理
	bool SelectSingleUserInfoRecvHandle(void* vpData, unsigned int DataLen); //单条查询用户信息返回
	bool SelectBatchUserInfoRecvHandle(void* vpData, unsigned int DataLen); //批量查询用户信息返回

protected:
	void ShowSelectSingleScoreResult(vector<string> vSelectFieldShow, vector<string> vSelectResultShow, bool bShow=true);
};

#endif