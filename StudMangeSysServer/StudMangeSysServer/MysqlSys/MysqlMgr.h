#ifndef __MYSQLMGR_H__
#define __MYSQLMGR_H__

#include <queue>
#include "xSingleton.h"
#include "ProcMgr.h"
#include <Windows.h> 
#include <mysql.h>
#include "MysqlDef.h"
#include "ProtoDef.h"

class MysqlMgr : public xSingleton<MysqlMgr>
{
public:
	MysqlMgr();
	~MysqlMgr();

	bool MysqlConn();

	void InputMsgQueue(string strMysql, MysqlOper mysqlOper, Assist_ID AssistId, unsigned __int64 SocketId, string strRecord="");
	void MsgQueueHandle();
private:
	queue<MysqlMsgData> m_MysqlMsgQueue;

	bool m_IsConn;
	MYSQL m_MysqlCont; //���ݿ�������
	MYSQL_RES *m_MysqlRes;

	//��ʱд�����˿�Ҳ��дΪ��
	const string m_sUser;
	const string m_sPswd;
	const string m_sHost;
	const string m_sDB;

	CRITICAL_SECTION  _critical;
};

#endif