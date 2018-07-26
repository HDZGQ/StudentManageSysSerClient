#ifndef __UPDATESCORESYSPROC_H__
#define __UPDATESCORESYSPROC_H__

#include <vector>
#include <string>
#include "BaseProc.h"

using namespace std;

class UpdateScoreSysProc : public BaseProc
{
public:
	UpdateScoreSysProc(ProcDef nProcDef);
	~UpdateScoreSysProc();

	virtual bool initMapChoose();

	virtual void EndProc();

	virtual void StartRecv(void* vpData, unsigned int DataLen, /*int iMianId,*/ int iAssistId);
	virtual void EndRecv();

	virtual void SwitchToOper(OperPermission CurOper);

	//主页选择操作 操作前请求处理
	void GetSubjectsAfterUpdateSingleScoreChooseHandle(); //单条更改成绩（包括单科和全科）操作前 现有科目请求

	//主页操作请求后的二级请求，现请求被操作用户现有所有分数情况，展示给操作者
	void GetSubjectsScoreAfterUpdateSingleScoreChooseHandle(char* pStrExistSubjects); //单条更改成绩（包括单科和全科）操作二级前 现有科目成绩请求

	//具体功能选择操作处理
	void UpdateSingleScoreByOneSubjectChooseHandle(char* pName, char* pAccount, char* pGrade, unsigned char* pSubjectsId, unsigned char* pScore, unsigned char bSubjectCount); //单科单条更改成绩
	void UpdateSingleScoreBySubjectsChooseHandle(char* pName, char* pAccount, char* pGrade, unsigned char* pSubjectsId, unsigned char* pScore, unsigned char bSubjectCount); //现有所有科目单条更改成绩

	//返回结果处理
	bool GetSubjectsAfterUpdateSingleScoreRecvHandle(void* vpData, unsigned int DataLen); //单条更改成绩（包括单科和全科）操作前 现有科目回复
	bool GetSubjectsScoreAfterUpdateSingleScoreRecvHandle(void* vpData, unsigned int DataLen);//单条更改成绩（包括单科和全科）操作二级前 现有科目成绩回复
	bool UpdateSingleScoreRecvHandle(void* vpData, unsigned int DataLen); //单条更改成绩（包括单科和全科）操作回复

private:
	bool ShowSubjects(unsigned char* pSubjectsId, unsigned char* pScore, unsigned char bSubjectCount, int iField=2);
	bool FindSubjectsId(unsigned char* pSubjectsId, unsigned char bSubjectCount, unsigned char SubjectsId);
	bool CheckScore(string sScore);

private:
	vector<string> m_vecStrChineseName;
};

#endif