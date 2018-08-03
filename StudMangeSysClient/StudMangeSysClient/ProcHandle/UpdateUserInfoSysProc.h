#ifndef __UPDATEUSERINFOSYSPROC_H__
#define __UPDATEUSERINFOSYSPROC_H__

#include <map>
#include <string>
#include "BaseProc.h"

using namespace std;

class UpdateUserInfoSysProc : public BaseProc
{
public:
	UpdateUserInfoSysProc(ProcDef nProcDef);
	~UpdateUserInfoSysProc();

	virtual bool initMapChoose();

	virtual void EndProc();

	virtual void StartRecv(void* vpData, unsigned int DataLen, /*int iMianId,*/ int iAssistId);
	virtual void EndRecv();

	virtual void SwitchToOper(OperPermission CurOper);

	//主页选择操作 操作前请求处理
	void GetUserInfoAfterUpdateSingleUserInfoChooseHandle(); //单条更改用户信息前请求用户信息

	//具体功能选择操作处理
	void UpdateSingleUserInfoByOneFieldChooseHandle(unsigned uUserID, char* pAccount, char* pPwd, char* pName, unsigned char uSex, unsigned char uIdent, char* pMajor, char* pGrade); //单条单字段更改用户信息请求
	void UpdateSingleUserInfoByFixedFieldChooseHandle(unsigned uUserID, char* pAccount, char* pPwd, char* pName, unsigned char uSex, unsigned char uIdent, char* pMajor, char* pGrade); //单条固定字段更改用户信息请求

	//返回结果处理
	bool GetUserInfoAfterUpdateSingleUserInfoRecvHandle(void* vpData, unsigned int DataLen); //单条更改用户信息返回用户信息
	bool UpdateSingleUserInfoRecvHandle(void* vpData, unsigned int DataLen); //单条更改用户信息返回

protected:
	void InitUserInfoFieldName();
	string GetEnglishUserInfoFieldByField(int iFeildId);
	string GetChineseUserInfoFieldByField(int iFeildId);
	bool IsPwdUserInfoField(int iField);

	int GetChooseUserInfoFieldId(string strShowFieldID, int iField=2); //展示字段并选项 1-account 2-password 3-name 4-sex 5-Ident 6-major 7-grade
	bool InputAllUserInfoFieldValues(string strShowFieldID, map<int, string>& mapNewAllFieldValue, int iField=2); //用于固定字段更改。要求根据传入的fieldid输入新的值，都合法就放到map里面去
		
	bool CheckUserInfoFieldValue(string strFeildValue, int iFieldId); //根据fieldid检测输入的字符串是否合法
	string GetOldUserInfoFieldValue(char* pAccount, char* pPwd, char* pName, unsigned char uSex, unsigned char uIdent, char* pMajor, char* pGrade, int iFieldId); //根据fieldid获取旧用户信息值
private:
	map<int, string> m_mUserInfoFieldName; 
	int m_maxUserInfoFieldID;

	//更新用户信息时候的临时数据
	unsigned char m_sUserInfoField[MAXUSERINFOFIELDCOUNT]; //用户信息字段id
	char m_cUserInfoValue[MAXUSERINFOFIELDCOUNT][41];
	unsigned char m_bUserInfoFieldCount; //用户信息字段数
};

#endif