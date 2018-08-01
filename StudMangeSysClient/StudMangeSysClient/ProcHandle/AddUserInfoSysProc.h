#ifndef __ADDUSERINFOSYSPROC_H__
#define __ADDUSERINFOSYSPROC_H__

#include <vector>
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
	void AddBatchUserInfoChooseHandle(); //批量增加用户信息请求


	//返回结果处理
	bool AddSingleUserInfoRecvHandle(void* vpData, unsigned int DataLen);//单条增加用户信息回复
	bool AddBatchUserInfoRecvHandle(void* vpData, unsigned int DataLen);//批量增加用户信息回复

protected:
	bool SendAddBatchUserInfoData();

private:
	vector<vector<string>> m_vvAddBatchUserInfoFileData;//批量添加用户信息数据 如果数据太多，每添加一次服务端返回成功才可以继续添加下一次
	vector<string> m_vecAddBatchUserInfoFeildData; //批量添加用户信息字段
	unsigned m_uAddBatchUserInfoSumCount; //批量添加的动态记录数
	unsigned m_uAddBatchUserInfoSucceedCount; //批量添加成功的动态记录数
};


#endif