#ifndef __MYSQLMGR_H__
#define __MYSQLMGR_H__

#include <queue>
#include "xSingleton.h"
#include "ProcMgr.h"
#include <Windows.h> 
#include <mysql.h>
#include "MysqlDef.h"

class MysqlMgr : public xSingleton<MysqlMgr>
{
public:
	MysqlMgr();
	~MysqlMgr();

	bool MysqlConn();

	void InputMsgQueue(string strMysql, MysqlOper mysqlOper, OperPermission operPer, unsigned __int64 SocketId);
	void MsgQueueHandle();
private:
	queue<MysqlMsgData> m_MysqlMsgQueue;

	bool m_IsConn;
	MYSQL m_MysqlCont; //数据库描述字
	MYSQL_RES *m_MysqlRes;

	//暂时写死，端口也是写为宏
	const string m_sUser;
	const string m_sPswd;
	const string m_sHost;
	const string m_sDB;
};

#endif