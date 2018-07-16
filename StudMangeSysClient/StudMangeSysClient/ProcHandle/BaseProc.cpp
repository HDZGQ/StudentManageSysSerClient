#include "BaseProc.h"
#include "ProcMgr.h"
#include "TCPHandle.h"


BaseProc::BaseProc(ProcDef nProcDef) : m_MyProcDef(nProcDef)
{
	m_IsShow = false;
	m_IsRunning = false;
	m_CurOper = OPER_PER_INVALID;
	m_iMyChoose = -1;

	m_iMinRealChoose = 1;
	m_iMaxRealChoose = m_iMinRealChoose;

	m_IsChooseAgain = false;

	m_iOperInputErrorLimit = 0;
}

BaseProc::~BaseProc()
{
}

void BaseProc::OnStart(bool bChooseAgain)
{
	m_IsShow = true;
	m_IsChooseAgain = bChooseAgain;

}


void BaseProc::StartProc()
{
	m_IsShow = false;

	m_IsRunning = true;

	printf("-------------------------------\n");

	//文字选择界面显示
	int iCount = 0;
	for (map<int, ChooseData>::iterator iter = m_mChoose.begin(); iter != m_mChoose.end(); iter++)
	{
		if (iter->second.OperPer != OPER_PER_INVALID && !ProcMgr::GetInstance()->CheckOperPerValid(iter->second.OperPer))
			continue;	

		if (iCount != 0 && iCount % 2 == 0)
			cout << endl;
		else if (iCount % 2 == 1)
			cout << "\t\t";

		iCount++;
		cout << iCount << "--" << iter->second.StrText ;
	}
	cout<<endl;
	if (0 == iCount)
	{
		//没有选择的情况，暂时不可能出现，不处理
	}

	if (!m_IsChooseAgain)
		cout << "请选择：" << endl;
	else
	{ 
		cout << "请重新选择：" << endl;
		m_IsChooseAgain = false;
	}
	cin >> m_iMyChoose;

	//用户选择处理
	int iRealChoose = GetRealChooseByUserChoose(m_iMyChoose);
	if (iRealChoose >= GetMinRealChoose() && iRealChoose <= GetMaxRealChoose())
	{
		if (OPER_PER_INVALID != GetOperPerByRealChoose(iRealChoose))
		{
			SetCurOper(GetOperPerByRealChoose(iRealChoose));
			SwitchToOper(GetOperPerByRealChoose(iRealChoose)); //注意，这个是调其派生类的，因为这个是虚函数
		}
		else if (PROC_DEF_INVALID != GetProcDefByRealChoose(iRealChoose))
		{
			ProcMgr::GetInstance()->ProcSwitch(GetProcDefByRealChoose(iRealChoose));
		}
		else
		{
			//退出系统
			ExitSys();
		}

		EndProc();
	}
	else
	{
		ProcMgr::GetInstance()->ProcSwitch(GetMyProcDef(), true);
	}
}

void BaseProc::EndProc()
{
	//printf("%s\n", __FUNCTION__);
	m_iOperInputErrorLimit = 0;
}

void BaseProc::EndRecv()
{
	//printf("%s\n", __FUNCTION__);

	m_IsRunning = false;
	m_iMyChoose = -1;

	if (m_CurOper != OPER_PER_INVALID)
		m_CurOper = OPER_PER_INVALID;
}

void BaseProc::SwitchToOper(OperPermission CurOper)
{
	printf("-------------------------------\n");
}


OperPermission BaseProc::GetCurOper()
{
	return m_CurOper;
}

void BaseProc::SetCurOper(OperPermission CurOper)
{
	m_CurOper = CurOper;
}

ProcDef BaseProc::GetNextProc()
{
	if (GetCurOper() != OPER_PER_INVALID)
	{
		return GetProcDefByOper(GetCurOper());
	}
	else if (-1 != GetRealChooseByUserChoose(m_iMyChoose))
	{
		return GetProcDefByRealChoose(GetRealChooseByUserChoose(m_iMyChoose));
	}
	else
	{
		return GetMyProcDef();
	}
}

ProcDef BaseProc::GetProcDefByOper(OperPermission Oper)
{
	map<int, ChooseData>::iterator iter = m_mChoose.begin();
	for (; iter != m_mChoose.end(); iter++)
	{
		if (iter->second.OperPer == Oper)
		{
			return iter->second.NextProc;
			break;
		}
	}

	return PROC_DEF_INVALID;
}

ProcDef BaseProc::GetProcDefByRealChoose(int iRealChoose)
{
	if (iRealChoose >= m_iMinRealChoose && iRealChoose <= m_iMaxRealChoose)
		return m_mChoose[iRealChoose].NextProc; //ChooseData必须要有无参数构造函数才行，不然会报错。和map内部的访问机制有关
	else
		return PROC_DEF_INVALID;
}

OperPermission BaseProc::GetOperPerByRealChoose(int iRealChoose)
{
	if (iRealChoose >= m_iMinRealChoose && iRealChoose <= m_iMaxRealChoose)
		return m_mChoose[iRealChoose].OperPer; //ChooseData必须要有无参数构造函数才行，不然会报错。和map内部的访问机制有关
	else
		return OPER_PER_INVALID;
}


bool BaseProc::IsShow()
{
	return m_IsShow;
}

bool BaseProc::IsRunning()
{
	return m_IsRunning;
}

ProcDef BaseProc::GetMyProcDef()
{
	return m_MyProcDef;
}

int& BaseProc::GetMinRealChoose()
{
	return m_iMinRealChoose;
}

int& BaseProc::GetMaxRealChoose()
{
	return m_iMaxRealChoose;
}

int BaseProc::GetRealChooseByUserChoose(int iChoose)
{
	int iCount = 0;

	for (map<int, ChooseData>::iterator iter = m_mChoose.begin(); iter != m_mChoose.end(); iter++)
	{
		if (iter->second.OperPer != OPER_PER_INVALID && !ProcMgr::GetInstance()->CheckOperPerValid(iter->second.OperPer))
			continue;	

		iCount++;

		if (iCount == iChoose)
			return iter->first;
	}

	return -1;
}

int BaseProc::GetUserMaxChoose()
{
	int iCount = 0;

	for (map<int, ChooseData>::iterator iter = m_mChoose.begin(); iter != m_mChoose.end(); iter++)
	{
		if (iter->second.OperPer != OPER_PER_INVALID && !ProcMgr::GetInstance()->CheckOperPerValid(iter->second.OperPer))
			continue;

		iCount++;
	}

	return iCount;
}

void BaseProc::ExitSys()
{
	cout<<"退出系统中..."<<endl;

	//发送服务端
	CS_MSG_EXIT_SYS SendReq;
	SendReq.bExit = true;
	TCPHandle::GetInstance()->Send(&SendReq, sizeof(SendReq), ASSIST_ID_EXIT_SYS);

	system("pause");
	exit(0);
}

void BaseProc::OperInputErrorHandle()
{
	if (++m_iOperInputErrorLimit >= OPERINPUTERRORMAXLIMIT)
	{
		m_iOperInputErrorLimit = 0;
		ProcMgr::GetInstance()->ProcSwitch(GetMyProcDef(), true);  //重新登录注册
	}
	else
	{
		SwitchToOper(GetCurOper());	//注意，这个是调其派生类的，因为这个是虚函数
	}
}