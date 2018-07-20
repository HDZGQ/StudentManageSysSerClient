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

	//主页选择操作 操作前请求处理
	void GetSubjectsAfterAddSubjectChooseHandle();
	void GetSubjectsAfterDeleteSubjectChooseHandle();

	//选择操作处理
	void AddSubjectChooseHandle(char* pCanAlterSubjects);
	void DeleteSubjectChooseHandle(char* pCanAlterSubjects);

	//返回结果处理
	bool GetSubjectsAfterAlterSubjectRecvHandle(void* vpData, unsigned int DataLen);
	bool AlterSubjectRecvHandle(void* vpData, unsigned int DataLen);

protected:
	int ShowSubjects(map<int, string> mIStr);
};

#endif