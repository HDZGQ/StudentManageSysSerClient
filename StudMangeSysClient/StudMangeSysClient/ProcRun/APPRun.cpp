#include "APPRun.h"


APPRun::APPRun()
{

}

APPRun::~APPRun()
{

}

void APPRun::Run()
{
	//tcp连接
	if (!TCPHandle::GetInstance()->TCPConn())
	{
		cout<< " TCP connect fail, exit system" <<endl;
  		system("pause");
		exit(0);
	}

	//把所有功能界面进程压入监控器
	ProcMgr::GetInstance()->initProc();

	//创建接受信息线程和发送心跳包线程 --心跳包线程后续加上，先做基本功能
	HANDLE recvThread = CreateThread(NULL, 0, RecvThread, NULL, 0, NULL);

	//启动登录注册进程，之后再获取操作权限，操作权限不需要弄出一个类，可以直接在登录注册进程里面处理
	ProcMgr::GetInstance()->ProcSwitch(PROC_DEF_ENTERSYSPROC);

	/*
	* 监控功能界面进程 - - 会阻塞，但是暂时还是使用进程管理器单实例（后续通过指针（引用）获取进程管理器里的进程管理器成员，在这里进行监控）
	*/
	ProcMgr::GetInstance()->Notify();
}