#ifndef __SELECTSCORESYSPROC_H__
#define __SELECTSCORESYSPROC_H__

#include <vector>
#include "BaseProc.h"

using namespace std;

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


	//主页选择操作 操作前请求处理
	void GetSubjectsAfterSelectScoreChooseHandle(); //查询成绩（包括批量和单条，单科和全科）操作前 现有科目请求

	//具体功能选择操作处理
	void SelectSingleScoreByOneSubjectChooseHandle(char* pStrExistSubjects); //单科单条查询成绩
	void SelectSingleScoreBySubjectsChooseHandle(char* pStrExistSubjects); //现有所有科目单条查询成绩
	void SelectBatchScoreByOneSubjectChooseHandle(char* pStrExistSubjects); //单科批量查询成绩
	void SelectBatchScoreBySubjectsChooseHandle(char* pStrExistSubjects); //现有所有科目批量查询成绩

	//返回结果处理
	bool GetSubjectsAfterSelectScoreRecvHandle(void* vpData, unsigned int DataLen); //查询成绩（包括批量和单条，单科和全科）操作前 现有科目回复
	bool SelectSingleScoreRecvHandle(void* vpData, unsigned int DataLen); //单条查询成绩返回
	bool SelectBatchScoreRecvHandle(void* vpData, unsigned int DataLen); //批量查询成绩返回

private:
	int ShowSubjects(map<int, string> mIStr, int iField=2);
	void ShowSelectSingleScoreResult(vector<string> vSelectFieldShow, vector<string> vSelectResultShow, bool bShowDescribe=false, bool bShowLine=true);
};

#endif