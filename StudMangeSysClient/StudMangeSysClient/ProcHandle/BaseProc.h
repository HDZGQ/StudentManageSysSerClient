#ifndef __BASEPROC_H__
#define __BASEPROC_H__


#include <iostream>
#include <map>
#include "ProcDef.h"

using namespace std;


class BaseProc 
{
public:
	BaseProc(ProcDef nProcDef);
	~BaseProc();

	//派生类公共接口
public:
	void OnStart(bool bChooseAgain = false); //启动进程接口

	virtual void StartProc();
	virtual void EndProc();

	virtual void StartRecv(void* vpData, unsigned int DataLen, /*int iMianId,*/ int iAssistId);
	virtual void EndRecv();

	void SetIEndFlag(int iEndFlag);
	int GetIEndFlag();

	virtual bool initMapChoose() = 0;

	virtual void SwitchToOper(OperPermission CurOper); //进程内部操作切换，适用于有操作的界面，操作是指增删查改等这些操作 （结束当前进程，切换其他（包括当前）进程。调用时候一并要调用EndRecv清除数据）

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

	int GetMyChoose();

	int GetRealChooseByUserChoose(int iChoose);//通过用户输入的权限影响过的choose，找到m_mChoose对应的真正key值
	int GetUserMaxChoose(); //获取用户可选择里的最大选择值

	void ExitSys(); //结束程序，退出系统

	void OperInputErrorHandle(bool bFlag=true, int uMaxInputErrorCount=OPERINPUTERRORMAXLIMIT); //多次输入操作错误处理 要考虑只在请求处选择错误和到了返回结果处选择错误，bFlag标记用于控制是否需要使用该函数的切换进程

	//基类私有接口
protected:
	//返回结果处理
	bool NotifyUserExitSysRecvHandle(void* vpData, unsigned int DataLen); //通知用户退出系统

protected:
	//key就是realChoose
	map<int, ChooseData> m_mChoose;

	//受用户自身权限限制，并不对应m_mChoose中的key值。还需要转换
	int m_iMyChoose;

	//操作输入错误次数限制，会转回到原界面
	int m_iOperInputErrorLimit;
private:
	bool m_IsShow;
	bool m_IsRunning; //接受消息时，检测到运行的进程，就把这个消息投递到对应进程的StartRecv，各StartRecv再根据协议id区分取出消息  --其实感觉这样子不是很好。一般都是先根据协议id区分再做其他事
	OperPermission m_CurOper; //进行的操作
	int m_iEndFlag; //0没结束 1正常结束  -1不正常结束  为了防止不是一次请求一次接受进程就完成而设计的，现在适应所有情况

	//m_mChoose的key值的最大最小值
	int m_iMinRealChoose;
	int m_iMaxRealChoose;

	bool m_IsChooseAgain;

	const ProcDef m_MyProcDef;

};

#endif