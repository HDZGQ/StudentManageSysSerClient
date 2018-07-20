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


	//主页选择操作 操作前请求处理
	void GetSubjectsAfterAddSingleScoreChooseHandle(); //单条增加成绩（包括单科和全科）操作前 现有科目请求

	//具体功能选择操作处理
	void AddSingleScoreByOneSubjectChooseHandle(char* pStrExistSubjects); //单科单条增加成绩
	void AddSingleScoreBySubjectsChooseHandle(char* pStrExistSubjects); //现有所有科目单条增加成绩

	//返回结果处理
	bool GetSubjectsAfterAddSingleScoreRecvHandle(void* vpData, unsigned int DataLen); //单条增加成绩（包括单科和全科）操作前 现有科目回复
	bool AddSingleScoreRecvHandle(void* vpData, unsigned int DataLen); //单条增加成绩（包括单科和全科）操作回复
	
protected:
	int ShowSubjects(map<int, string> mIStr, int iField=2);
	bool CheckScore(string sScore);
};

#endif