#ifndef __BASEPROC_H__
#define __BASEPROC_H__

#include <iostream>
#include <map>
#include "ProcDef.h"

using namespace std;

class ProcMgr;

class BaseProc
{
public:
	BaseProc(ProcDef nProcDef);
	~BaseProc();

	void OnStart(bool bChooseAgain = false); //启动进程接口

	virtual void StartProc();
	virtual void EndProc();

	virtual void StartRecv(void* vpData, unsigned int DataLen, /*int iMianId,*/ int iAssistId) = 0;
	virtual void EndRecv();

	virtual bool initMapChoose() = 0;

	virtual void SwitchToOper(OperPermission CurOper) = 0; //进程内部操作切换，适用于有操作的界面，操作是指增删查改等这些操作

	OperPermission GetCurOper();
	void SetCurOper(OperPermission CurOper);
	
	ProcDef GetNextProc(); //根据当前操作等返回下一步进程的标识
	ProcDef GetProcDefByOper(OperPermission Oper);
	ProcDef GetProcDefByRealChoose(int iRealChoose);
	OperPermission GetOperPerByRealChoose(int iRealChoose);

	bool IsShow();
	bool IsRunning();
	ProcDef GetMyProcDef();

	//m_mChoose的key值的最大最小值
	int& GetMinRealChoose();
	int& GetMaxRealChoose();

	int GetRealChooseByUserChoose(int iChoose);//通过用户输入的权限影响过的choose，找到m_mChoose对应的真正key值
	int GetUserMaxChoose(); //获取用户可选择里的最大选择值

	void ExitSys();

protected:
	//key就是realChoose
	map<int, ChooseData> m_mChoose;

	//受用户自身权限限制，并不对应m_mChoose中的key值。还需要转换
	int m_iMyChoose;

private:
	bool m_IsShow;
	bool m_IsRunning; //接受消息时，检测到运行的进程，就把这个消息投递到对应进程的StartRecv，各StartRecv再根据协议id区分取出消息  --其实感觉这样子不是很好。一般都是先根据协议id区分再做其他事
	OperPermission m_CurOper; //进行的操作

	//m_mChoose的key值的最大最小值
	int m_iMinRealChoose;
	int m_iMaxRealChoose;

	bool m_IsChooseAgain;

	const ProcDef m_MyProcDef;
};

#endif