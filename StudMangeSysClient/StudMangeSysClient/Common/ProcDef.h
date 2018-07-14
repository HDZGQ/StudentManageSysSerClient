
#ifndef __PROCDEF_H__
#define __PROCDEF_H__


#include <string>
#include "ProtoDef.h"

#define OPERINPUTERRORMAXLIMIT 2

//进程（功能界面）标志定义
enum ProcDef
{
	PROC_DEF_INVALID          =    0,
	PROC_DEF_START			  =    0,  //有效值起始值
	PROC_DEF_ENTERSYSPROC     =    1,  //登录注册界面
	PROC_DEF_COMMONSYSPROC    =    2,  //刚进入系统后的界面
	PROC_DEF_SCORESYSPROC     =    3,  //成绩系统主界面
	PROC_DEF_USERINFOSYSPROC  =    4,  //用户信息系统主界面

	PROC_DEF_END                     //有效值终止值
};

struct ChooseData
{
	std::string StrText; //文本描述
	OperPermission OperPer; //操作
	ProcDef NextProc;   //（操作成功后）转到新界面的标识 --一般来说，这个必须值有效才行

	ChooseData()
	{
		init();
	}

	ChooseData(std::string strText, OperPermission operPer, ProcDef nextProc) : StrText(strText), OperPer(operPer), NextProc(nextProc)
	{
	}

	void init()
	{
		StrText = "";
		OperPer = OPER_PER_INVALID;
		NextProc = PROC_DEF_INVALID;
	}
};

#endif