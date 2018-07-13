
#ifndef __PROCDEF_H__
#define __PROCDEF_H__


#include <string>
#include "ProtoDef.h"

//���̣����ܽ��棩��־����
enum ProcDef
{
	PROC_DEF_INVALID          =    0,
	PROC_DEF_START			  =    0,  //��Чֵ��ʼֵ
	PROC_DEF_ENTERSYSPROC     =    1,  //��¼ע�����
	PROC_DEF_COMMONSYSPROC    =    2,  //�ս���ϵͳ��Ľ���
	PROC_DEF_SCORESYSPROC     =    3,  //�ɼ�ϵͳ������
	PROC_DEF_USERINFOSYSPROC  =    4,  //�û���Ϣϵͳ������

	PROC_DEF_END                     //��Чֵ��ֵֹ
};

struct ChooseData
{
	std::string StrText; //�ı�����
	OperPermission OperPer; //����
	ProcDef NextProc;   //�������ɹ���ת���½���ı�ʶ --һ����˵���������ֵ��Ч����

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