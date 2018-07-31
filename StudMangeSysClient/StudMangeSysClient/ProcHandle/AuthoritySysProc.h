#ifndef __AUTHORITYSYSPROC_H__
#define __AUTHORITYSYSPROC_H__

#include "BaseProc.h"

class AuthoritySysProc : public BaseProc
{
public:
	AuthoritySysProc(ProcDef nProcDef);
	~AuthoritySysProc();

	virtual bool initMapChoose();

	virtual void EndProc();

	virtual void StartRecv(void* vpData, unsigned int DataLen, /*int iMianId,*/ int iAssistId);
	virtual void EndRecv();

	virtual void SwitchToOper(OperPermission CurOper);

	//主页选择操作 操作前请求处理
	void GetAuthoritbyAfterEditAuthoritbyChooseHandle(); //获取现有或者可增加权限 请求

	//具体功能选择操作处理
	void EditAuthoritChooseHandle(unsigned char sType, char* cAccount, unsigned char* cAuthority, unsigned char cAuthorityCount); //增删权限请求

	//返回结果处理
	bool GetAuthoritbyAfterEditAuthoritbyRecvHandle(void* vpData, unsigned int DataLen); //获取现有或者可增加权限 回复
	bool EditAuthoritRecvHandle(void* vpData, unsigned int DataLen); //增删权限回复

private:
	void ShowAuthoritby(unsigned char* cAuthority, unsigned char cAuthorityCount, int iField=2);
	bool FindOneInArray(unsigned char* pArray, unsigned char iCount, unsigned char iMaxCount, unsigned char Obj);
	bool CheckStringVaildRemoveSpl(string str, string strSpl, string strVaild);
};

#endif